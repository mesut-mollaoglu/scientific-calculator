#include "TextBox.h"

TextBox::TextBox(Graphics* graphics, HWND windowHandle, float tX, float tY, float tWidth, float tHeight, 
	ID2D1SolidColorBrush* textColor, ID2D1SolidColorBrush* boxColor, std::wstring textStr): hwnd(windowHandle), gfx(graphics), x(tX),
	y(tY), width(tWidth), height(tHeight), textBrush(textColor), backColor(boxColor), text(textStr) {
	textRect = D2D1::RectF(x, y, x + width, y + height);
}

TextBox::~TextBox() {

}

std::string TextBox::GetText() {
	std::string s = std::string(this->text.begin(), this->text.end());
	return s;
}

void TextBox::RenderText() {
	gfx->GetRenderTarget()->FillRectangle(textRect, textBrush);
	gfx->DrawTextF(text, textRect.left, textRect.top, textRect.right - textRect.left, textRect.bottom - textRect.top, backColor);
}

void TextBox::UpdateText(MSG message) {
	this->CheckActivation();
	if(this->activated){
		switch (message.message) {
		case WM_CHAR:
			switch (message.wParam) {
			case 0x08:
				if (!this->text.empty()) this->text.pop_back();
				break;
			default:
				text += (wchar_t)message.wParam;
				break;
			}
		}
	}
}

bool TextBox::isEmpty() {
	return this->text.empty() ? true : false;
}

void TextBox::CheckActivation() {
	POINT pt;
	GetCursorPos(&pt);
	ScreenToClient(this->hwnd, &pt);
	if (GetAsyncKeyState(VK_LBUTTON)) {
		if (pt.x >= textRect.left &&
			pt.x <= textRect.right &&
			pt.y >= textRect.top &&
			pt.y <= textRect.bottom) {
			this->activated = true;
		}else {
			this->activated = false;
		}
	}
}

void TextBox::SetText(string str) {
	wstring s = wstring(str.begin(), str.end());
	this->text = s;
}