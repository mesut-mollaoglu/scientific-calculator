#include "Calculator.h"
#include "TextBox.h"
#include "Button.h"
#include <chrono>

using namespace std;
using namespace literals;
Calculator* calculator;
HWND windowHandle = nullptr;
WNDPROC windowProc = nullptr;
double result = 0.0;
bool drawGraph = false;
string s = "";
using Clock = std::chrono::steady_clock;
std::chrono::time_point<std::chrono::steady_clock> start;
std::chrono::time_point<std::chrono::steady_clock> now;
std::chrono::milliseconds duration;
bool bInit = false;

LRESULT OnWindowResize(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
	LRESULT result = CallWindowProc(windowProc, hwnd, message, wParam, lParam);
	RECT rect;
	GetClientRect(hwnd, &rect);
	InvalidateRect(hwnd, &rect, false);
	return result;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
	if (message == WM_DESTROY && hwnd == windowHandle) PostQuitMessage(0); DefWindowProc(hwnd, message, wParam, lParam);
	if (message == WM_SIZE && hwnd == windowHandle) return OnWindowResize(hwnd, message, wParam, lParam);
	return CallWindowProc(windowProc, hwnd, message, wParam, lParam);
}

int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
	WNDCLASSEX wc = { 0 };
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.lpfnWndProc = WndProc;
	wc.hInstance = hInstance;
	wc.lpszClassName = L"ClassName";
	wc.lpszMenuName = nullptr;
	wc.style = CS_HREDRAW | CS_VREDRAW;
	RegisterClassEx(&wc);
	RECT rect = { 0, 0, 600, 600 };
	AdjustWindowRectEx(&rect, WS_OVERLAPPEDWINDOW, false, WS_EX_OVERLAPPEDWINDOW);
	windowHandle = CreateWindowEx(WS_EX_OVERLAPPEDWINDOW, WC_DIALOG, L"Calculator", WS_OVERLAPPEDWINDOW, 100, 100, rect.right - rect.left, rect.bottom - rect.top, nullptr, nullptr, hInstance, nullptr);
	Graphics* graphics = new Graphics();
	if (!graphics->InitGraphics(windowHandle)) {
		delete graphics;
		return -1;
	}
	TextBox* valueBox = new TextBox(graphics, windowHandle, 370.0f, 10.0f, 225.0f, 12.0f, graphics->snowColor, graphics->blackColor, L"Value:");
	TextBox* inputBox = new TextBox(graphics, windowHandle, 370.0f, 25.0f, 225.0f, 12.0f, graphics->snowColor, graphics->blackColor, L"Input:");
	TextBox* fromBox = new TextBox(graphics, windowHandle, 370.0f, 40.0f, 225.0f, 12.0f, graphics->snowColor, graphics->blackColor, L"From(Integration):");
	TextBox* toBox = new TextBox(graphics, windowHandle, 370.0f, 55.0f, 225.0f, 12.0f, graphics->snowColor, graphics->blackColor, L"To(Integration):");
	Button* graphButton = new Button(graphics, windowHandle, 5.0f, 70.0f, 60.0f, 20.0f, L"Graph", graphics->blackColor, graphics->whiteColor);
	Button* integrateButton = new Button(graphics, windowHandle, 5.0f, 10.0f, 60.0f, 20.0f, L"Integrate", graphics->blackColor, graphics->whiteColor);
	Button* derivativeButton = new Button(graphics, windowHandle, 5.0f, 30.0f, 60.0f, 20.0f, L"Derivative", graphics->blackColor, graphics->whiteColor);
	Button* calculateButton = new Button(graphics, windowHandle, 5.0f, 50.0f, 60.0f, 20.0f, L"Calculate", graphics->blackColor, graphics->whiteColor);
	windowProc = (WNDPROC)SetWindowLongPtrW(windowHandle, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(WndProc));
	ShowWindow(windowHandle, nCmdShow);

	MSG message = { 0 };
	message.message == WM_NULL;
	while (message.message != WM_QUIT) {
		if (PeekMessage(&message, nullptr, 0, 0, PM_REMOVE)) {
			TranslateMessage(&message);
			DispatchMessage(&message);
			inputBox->UpdateText(message);
			valueBox->UpdateText(message);
			fromBox->UpdateText(message);
			toBox->UpdateText(message);
		}
		else {
			graphics->GetRenderTarget()->BeginDraw();
			graphics->GetRenderTarget()->Clear(D2D1::ColorF(D2D1::ColorF::White));
			inputBox->RenderText();
			valueBox->RenderText();
			fromBox->RenderText();
			toBox->RenderText();
			integrateButton->RenderButton();
			derivativeButton->RenderButton();
			calculateButton->RenderButton();
			graphButton->RenderButton();
			graphics->DrawGrid(0, 600, 0, 600, 15);
			if(drawGraph) graphics->CreateGraph(calculator, s, -600, 600, 0, 600);
			graphics->DrawTextF(L"Result=" + to_wstring(result), 500, 585, 110, 10, graphics->blackColor);
			graphics->GetRenderTarget()->EndDraw();
			if (!bInit)
			{
				start = now = Clock::now();
				bInit = true;
			}
			now = Clock::now();
			duration = std::chrono::duration_cast<std::chrono::milliseconds>(now - start);
			if (duration.count() > 150)
			{
				if (!inputBox->isEmpty() && !valueBox->isEmpty() && calculateButton->CheckForCollisions())
					result = calculator->calculate(stod(valueBox->GetText()), calculator->convert(inputBox->GetText()));
				if (!inputBox->isEmpty() && !fromBox->isEmpty() && !toBox->isEmpty() && integrateButton->CheckForCollisions())
					result = calculator->integrate(inputBox->GetText(), stod(fromBox->GetText()), stod(toBox->GetText()),
						calculator->getDegree(calculator->convert(inputBox->GetText())));
				if (!inputBox->isEmpty() && !valueBox->isEmpty() && derivativeButton->CheckForCollisions())
					result = calculator->derivate(inputBox->GetText(), stod(valueBox->GetText()), 10);
				if (!inputBox->isEmpty() && graphButton->CheckForCollisions()){
					s = inputBox->GetText();
					drawGraph = true;
				}
			}
		}
	}
}