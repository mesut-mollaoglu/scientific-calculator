#include "Graphics.h"

Graphics::Graphics() {
	this->textFormat = NULL;
	this->dWriteFactory = NULL;
	this->factory = NULL;
	this->renderTarget = NULL;
	this->textLayout = NULL;
}

Graphics::~Graphics() {
	if (this->factory) this->factory->Release();
	if (this->renderTarget) this->renderTarget->Release();
	if (this->dWriteFactory) this->dWriteFactory->Release();
	if (this->textFormat) this->textFormat->Release();
}

HRESULT Graphics::InitWritingFactory() {
	HRESULT hr;
	hr = DWriteCreateFactory(
		DWRITE_FACTORY_TYPE_SHARED,
		__uuidof(IDWriteFactory5),
		reinterpret_cast<IUnknown**>(&dWriteFactory));
	if (SUCCEEDED(hr)) {
		hr = this->dWriteFactory->CreateTextFormat(L"Arial", NULL, DWRITE_FONT_WEIGHT_REGULAR,
			DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, 12.0f, L"en-us", &textFormat);
	}
	if (SUCCEEDED(hr)) { hr = this->textFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER); }
	if (SUCCEEDED(hr)) { this->textFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER); }
	return hr;
}

void Graphics::DrawTextF(std::wstring text, float x, float y, float width, float height, ID2D1Brush* color) {
	HRESULT hr;
	if (SUCCEEDED(this->InitWritingFactory())) {
		hr = this->dWriteFactory->CreateTextLayout(text.c_str(), (UINT)wcslen(text.c_str()), this->textFormat,
			width, height, &textLayout);
	}
	if (SUCCEEDED(hr)) {
		this->renderTarget->DrawTextLayout(D2D1::Point2F(x, y), textLayout, color, D2D1_DRAW_TEXT_OPTIONS_NONE);
	}
	this->textLayout->Release();
}

bool Graphics::InitGraphics(HWND hwnd) {
	HRESULT hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &factory);
	if (hr != S_OK) return false;
	RECT rect;
	GetClientRect(hwnd, &rect);
	hr = factory->CreateHwndRenderTarget(D2D1::RenderTargetProperties(D2D1_RENDER_TARGET_TYPE_DEFAULT, 
		D2D1::PixelFormat(DXGI_FORMAT_UNKNOWN, D2D1_ALPHA_MODE_UNKNOWN)), 
		D2D1::HwndRenderTargetProperties(hwnd, D2D1::SizeU(rect.right, rect.bottom)), &renderTarget);
	if (hr != S_OK) return false;
	renderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Black), &blackColor);
	renderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::White), &whiteColor);
	renderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Gray, 0.3f), &snowColor);
	if (hr != S_OK) return false;
	return true;
}

void Graphics::CreateGraph(Calculator* calculator, string infix, double xmin, double xmax, double ymin, double ymax) {
	float x, y;
	for (x = xmin; x <= xmax; x += 1)
	{
		y = calculator->calculate(x, calculator->convert(infix));
		D2D1_POINT_2F point1 = D2D1::Point2F((x - xmin) / (xmax - xmin) * (float)600, (1 - (y - ymin) / (ymax - ymin)) * (float)600);
		D2D1_POINT_2F point2 = D2D1::Point2F((x + 1 - xmin) / (xmax - xmin) * (float)600, 
			(1 - (calculator->calculate((x + 1), calculator->convert(infix)) - ymin) / (ymax - ymin)) * (float)600);
		this->renderTarget->DrawLine(point1, point2, this->blackColor, 1.0f);
	}
}

void Graphics::DrawGrid(float xmin, float xmax, float ymin, float ymax, float interval)
{
	for (float y = ymin + interval; y < ymax; y += interval)
	{
		this->renderTarget->DrawLine(
			D2D1::Point2F(xmin, y),
			D2D1::Point2F(xmax, y),
			this->snowColor,
			0.5f);
	}
	for (float x = xmin + interval; x < xmax; x += interval)
	{
		this->renderTarget->DrawLine(
			D2D1::Point2F(x, ymin),
			D2D1::Point2F(x, ymax),
			this->snowColor,
			0.5f);
	}
}