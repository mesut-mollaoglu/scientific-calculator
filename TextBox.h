#pragma once
#include "Graphics.h"

class TextBox {
public:
	TextBox(Graphics* graphics, HWND windowHandle, float tX, float tY, float tWidth, float tHeight,
		ID2D1SolidColorBrush* TextColor, ID2D1SolidColorBrush* boxColor, std::wstring textStr);
	~TextBox();
	void CheckActivation();
	void RenderText();
	void UpdateText(MSG message);
	std::string GetText();
	void SetText(string str);
	bool isEmpty();
private:
	bool activated = false;
	Graphics* gfx;
	D2D1_RECT_F textRect;
	float x, y, width, height;
	std::wstring text;
	ID2D1Brush* backColor;
	ID2D1Brush* textBrush;
	HWND hwnd;
};