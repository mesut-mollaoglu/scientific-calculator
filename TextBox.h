#pragma once
#include "Graphics.h"

constexpr float charWidth = 5.5f;
constexpr float charHeight = 20.f;

class TextBox {
public:
	TextBox(HWND windowHandle, float xPosition, float yPosition, std::wstring str)
	: hwnd(windowHandle), x(xPosition), y(yPosition), text(str) {
		SetSize();
	}
	virtual ~TextBox(){}
	void SetSize() {
		rect.left = x;
		rect.top = y;
		rect.right = x + float(GetMaxLine() + 1) * charWidth;
		rect.bottom = y + float(GetLineCount() + 1) * charHeight;
	}
	void CheckActivation() {
		RECT rc = { rect.left, rect.top, rect.right, rect.bottom };
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
	int GetMaxLine() {
		int maxCount = 0, buffer = 0;
		for (auto &c : text) {
			if (c == L'\n') { maxCount = fmax(maxCount, buffer); buffer = 0; }
			else ++buffer;
		}
		maxCount = fmax(maxCount, buffer);
		return maxCount;
	}
	int GetLineCount() {
		int count = 0;
		for (wchar_t &c : text)
			if (c == L'\n')
				count++;
		return count;
	}
	void UpdateText(MSG message) {
		CheckActivation();
		if (!activated) return;
		switch (message.message) {
		case WM_CHAR: {
			switch (message.wParam) {
			case 0x08: {if (!text.empty()) text.pop_back(); } break;
			case 0x0D: {text += L'\n'; } break;
			default: {text += (wchar_t)message.wParam; } break;
			}
		}
		}
		SetSize();
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