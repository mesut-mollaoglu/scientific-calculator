#include "Button.h"

Button::Button(Graphics* graphics, HWND windowHandle, float buttonX, float buttonY, float buttonWidth,
	float buttonHeight, std::wstring buttonName, ID2D1SolidColorBrush* buttonTextColor, ID2D1SolidColorBrush* buttonColor)
	:hwnd(windowHandle), gfx(graphics), x(buttonX), y(buttonY), height(buttonHeight), width(buttonWidth), color(buttonColor),
	textColor(buttonTextColor), name(buttonName) {
	rect = D2D1::RectF(x, y, x + width, y + height);
}

Button::~Button()
{
}

bool Button::CheckForCollisions() {
	POINT pt;
	GetCursorPos(&pt);
	ScreenToClient(hwnd, &pt);
	if (pt.x >= rect.left &&
		pt.x <= rect.right &&
		pt.y >= rect.top &&
		pt.y <= rect.bottom) {
		if (GetAsyncKeyState(VK_LBUTTON)) {
			return true;
		}
	}
	return false;
}
void Button::RenderButton() {
	gfx->GetRenderTarget()->DrawRectangle(rect, gfx->blackColor, 0.5f);
	gfx->GetRenderTarget()->FillRectangle(rect, color);
	gfx->DrawTextF(name, rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top, textColor);
}