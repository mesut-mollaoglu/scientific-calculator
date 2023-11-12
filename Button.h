#pragma once
#include "Graphics.h"

class Button {
public:
	Button(HWND windowHandle, float xPosition, float yPosition, float buttonWidth, float buttonHeight, std::wstring str) : 
	hwnd(windowHandle), x(xPosition), y(yPosition), width(buttonWidth), height(buttonHeight), name(str) {
		rect = D2D1::RectF(x, y, x + width, y + height);
	}
	virtual ~Button() {}
	bool hover() {
		RECT rc = { x, y, x + width, y + height };
		POINT pt;
		GetCursorPos(&pt);
		ScreenToClient(hwnd, &pt);
		return PtInRect(&rc, pt);
	}
	bool clicked() {
		return GetAsyncKeyState(VK_LBUTTON) && hover();
	}
	void Render() {
		Graphics::GetRenderTarget()->DrawRectangle(rect, Graphics::blackColor, hover() ? 0.7f : 0.5f);
		Graphics::GetRenderTarget()->FillRectangle(rect, Graphics::snowColor);
		Graphics::DrawTextF(name, rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top, Graphics::blackColor);
	}
private:
	D2D1_RECT_F rect;
	HWND hwnd;
	float x, y, width, height;
	std::wstring name;
};