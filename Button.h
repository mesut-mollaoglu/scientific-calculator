#pragma once
#include "Graphics.h"

class Button {
public:
	Button(Graphics* graphics, HWND windowHandle, float x, float y, float width, float height, std::wstring name,
		ID2D1SolidColorBrush* textColor, ID2D1SolidColorBrush* color);
	~Button();
	bool CheckForCollisions();
	void RenderButton();
private: 
	Graphics* gfx;
	D2D1_RECT_F rect;
	float x, y, width, height;
	std::wstring name; 
	ID2D1Brush* color; 
	ID2D1Brush* textColor;
	HWND hwnd;
};