#pragma once
#include <Windows.h>
#include <d2d1_1.h>
#include <dwrite_3.h>
#include <d2d1helper.h>
#include <wincodec.h>
#include <iostream>
#include "Calculator.h"

std::stack<Type> Calculator::operatorStack;

class Graphics {
public:
	static inline bool InitGraphics(HWND hwnd) {
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
	static inline ID2D1RenderTarget* GetRenderTarget() {
		return renderTarget;
	}
	static inline ID2D1Factory1* GetFactory() {
		return factory;
	}
	static inline IDWriteFactory5* getWriteFactory() {
		return dWriteFactory;
	}
	static inline HRESULT InitWritingFactory() {
		HRESULT hr;
		hr = DWriteCreateFactory(
			DWRITE_FACTORY_TYPE_SHARED,
			__uuidof(IDWriteFactory5),
			reinterpret_cast<IUnknown**>(&dWriteFactory));
		if (SUCCEEDED(hr)) {
			hr = dWriteFactory->CreateTextFormat(L"Arial", NULL, DWRITE_FONT_WEIGHT_REGULAR,
				DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, 12.0f, L"en-us", &textFormat);
		}
		if (SUCCEEDED(hr)) { hr = textFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER); }
		if (SUCCEEDED(hr)) { textFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER); }
		return hr;
	}
	static inline void CreateGraph(std::string infix, double xmin, double xmax, double ymin, double ymax) {
		float x, y;
		for (x = xmin; x <= xmax; x += 1)
		{
			y = Calculator::GetValue(infix, x);
			D2D1_POINT_2F point1 = D2D1::Point2F((x - xmin) / (xmax - xmin) * (float)600, (1 - (y - ymin) / (ymax - ymin)) * (float)600);
			D2D1_POINT_2F point2 = D2D1::Point2F((x + 1 - xmin) / (xmax - xmin) * (float)600, (1 - Calculator::GetValue(infix, x+1) - ymin) / (ymax - ymin) * (float)600);
			renderTarget->DrawLine(point1, point2, blackColor, 1.f);
		}
	}
	static ID2D1SolidColorBrush* blackColor;
	static ID2D1SolidColorBrush* whiteColor;
	static ID2D1SolidColorBrush* snowColor;
	static inline void DrawTextF(std::wstring text, float x, float y, float width, float height, ID2D1Brush* color) {
		HRESULT hr;
		if (SUCCEEDED(InitWritingFactory())) {
			hr = dWriteFactory->CreateTextLayout(text.c_str(), (UINT)wcslen(text.c_str()), textFormat,
				width, height, &textLayout);
		}
		if (SUCCEEDED(hr)) {
			renderTarget->DrawTextLayout(D2D1::Point2F(x, y), textLayout, color, D2D1_DRAW_TEXT_OPTIONS_NONE);
		}
		textLayout->Release();
	}
	static inline void DrawGrid(float xmin, float xmax, float ymin, float ymax, float interval) {
		for (float y = ymin + interval; y < ymax; y += interval)
		{
			renderTarget->DrawLine(D2D1::Point2F(xmin, y), D2D1::Point2F(xmax, y), snowColor, 0.5f);
		}
		for (float x = xmin + interval; x < xmax; x += interval)
		{
			renderTarget->DrawLine(D2D1::Point2F(x, ymin), D2D1::Point2F(x, ymax), snowColor, 0.5f);
		}
	}
	static inline void Free() {
		if (factory)factory->Release();
		if(dWriteFactory)dWriteFactory->Release();
		if (textFormat)textFormat->Release();
		if(textLayout)textLayout->Release();
		if(renderTarget)renderTarget->Release();
		if (blackColor) blackColor->Release();
		if (whiteColor) whiteColor->Release();
		if (snowColor) snowColor->Release();

	}
private:
	static ID2D1Factory1* factory;
	static IDWriteFactory5* dWriteFactory;
	static IDWriteTextFormat* textFormat;
	static IDWriteTextLayout* textLayout;
	static ID2D1HwndRenderTarget* renderTarget;
};