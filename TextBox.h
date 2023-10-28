#pragma once
#include "Graphics.h"

class TextBox {
public:
	TextBox(HWND windowHandle, float xPosition, float yPosition, float boxWidth, float boxHeight, std::wstring str)
	: hwnd(windowHandle), x(xPosition), y(yPosition), width(boxWidth), height(boxHeight), text(str) {
		rect = D2D1::RectF(x, y, x + width, y + height);
	}
	virtual ~TextBox(){}
	void CheckActivation() {
		RECT rc = { x, y, x + width, y + height };
		POINT pt;
		GetCursorPos(&pt);
		ScreenToClient(hwnd, &pt);
		if (GetAsyncKeyState(VK_LBUTTON)) { activated = PtInRect(&rc, pt); }
	}
	void Render() {
		Graphics::GetRenderTarget()->DrawRectangle(rect, Graphics::blackColor, activated ? 1.f : 0.7f);
		Graphics::GetRenderTarget()->FillRectangle(rect, Graphics::whiteColor);
		Graphics::DrawTextF(text, rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top, Graphics::blackColor);
	}
	void UpdateText(MSG message) {
		CheckActivation();
		if (!activated) return;
		switch (message.message) {
		case WM_CHAR: {
			switch (message.wParam) {
			case 0x08: {if (!text.empty()) text.pop_back(); } break;
			default: {text += (wchar_t)message.wParam; } break;
			}
		}
		}
	}
	std::string GetText() {
		return std::string(text.begin(), text.end());
	}
	void SetText(std::string str) {
		text = std::wstring(str.begin(), str.end());
	}
	bool empty() {
		return text.empty();
	}
private:
	bool activated;
	D2D1_RECT_F rect;
	float x, y, width, height;
	std::wstring text;
	HWND hwnd;
};