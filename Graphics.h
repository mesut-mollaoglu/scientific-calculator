#pragma once
#include <Windows.h>
#include <d2d1_1.h>
#include <dwrite_3.h>
#include <d2d1helper.h>
#include <wincodec.h>
#include <iostream>
#include "Calculator.h"

class Graphics {
public:
	bool InitGraphics(HWND hwnd);
	Graphics();
	~Graphics();
	ID2D1RenderTarget* GetRenderTarget() {
		return renderTarget;
	}
	ID2D1Factory1* GetFactory() {
		return factory;
	}
	IDWriteFactory5* getWriteFactory() {
		return dWriteFactory;
	}
	HRESULT InitWritingFactory();
	ID2D1SolidColorBrush* blackColor;
	ID2D1SolidColorBrush* whiteColor;
	ID2D1SolidColorBrush* snowColor;
	void DrawTextF(std::wstring text, float x, float y, float width, float height, ID2D1Brush* color);
	void CreateGraph(Calculator* calculator, string infix, double xmin, double xmax, double ymin, double ymax);
	void DrawGrid(float xmin, float xmax, float ymin, float ymax, float interval);
private:
	ID2D1Factory1* factory;
	IDWriteFactory5* dWriteFactory;
	IDWriteTextFormat* textFormat;
	IDWriteTextLayout* textLayout;
	ID2D1HwndRenderTarget* renderTarget;
};