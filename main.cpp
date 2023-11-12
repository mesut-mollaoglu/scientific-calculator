#pragma comment(lib, "comctl32")
#include "TextBox.h"
#include "Button.h"
#include <chrono>

ID2D1SolidColorBrush* Graphics::blackColor;
ID2D1SolidColorBrush* Graphics::whiteColor;
ID2D1SolidColorBrush* Graphics::snowColor;
ID2D1Factory1* Graphics::factory;
IDWriteFactory5* Graphics::dWriteFactory;
IDWriteTextFormat* Graphics::textFormat;
IDWriteTextLayout* Graphics::textLayout;
ID2D1HwndRenderTarget* Graphics::renderTarget;

HWND windowHandle = nullptr;
WNDPROC windowProc = nullptr;
double result = 0.0;
bool drawGraph = false;
std::string s = "";
using Clock = std::chrono::steady_clock;
std::chrono::time_point<std::chrono::steady_clock> now;

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
	Graphics::InitGraphics(windowHandle);
	TextBox value = TextBox(windowHandle, 5.f, 10.f, L"Variable Value:");
	TextBox input = TextBox(windowHandle, 5.f, 40.f, L"Input:");
	Button calculate = Button(windowHandle, 5.f, 70.f, 60.f, 20.f, L"Calculate");
	Button graph = Button(windowHandle, 5.f, 100.f, 40.f, 20.f, L"Graph");
	windowProc = (WNDPROC)SetWindowLongPtrW(windowHandle, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(WndProc));
	ShowWindow(windowHandle, nCmdShow);
	MSG message = { 0 };
	now = Clock::now();
	message.message == WM_NULL;
	while (message.message != WM_QUIT) {
		if (PeekMessage(&message, nullptr, 0, 0, PM_REMOVE)) {
			input.UpdateText(message);
			value.UpdateText(message);
			TranslateMessage(&message);
			DispatchMessage(&message);
		}
		else {
			Graphics::GetRenderTarget()->BeginDraw();
			Graphics::GetRenderTarget()->Clear(D2D1::ColorF(D2D1::ColorF::White));
			input.Render();
			value.Render();
			calculate.Render();
			graph.Render();
			Graphics::DrawGrid(0, 600, 0, 600, 15);
			if (drawGraph) Graphics::CreateGraph(s, -600, 600, 0, 600);
			Graphics::DrawTextF(L"Result = " + std::to_wstring(result), 0, 585, 600, 10, Graphics::blackColor);
			Graphics::GetRenderTarget()->EndDraw();
			if (std::abs(std::chrono::duration<float>(now - Clock::now()).count()) > 0.15)
			{
				if (!input.empty() && !value.empty() && calculate.clicked()) result = Calculator::GetValue(input.GetText(), atof(value.GetText().c_str()));
				if (!input.empty() && graph.clicked()) { s = input.GetText(); drawGraph = true; }
				now = Clock::now();
			}
		}
	}
	return message.wParam;
}

int main() {
	return 0;
}