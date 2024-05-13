#pragma once

#include "pch.h"
#include <uxtheme.h>

namespace KappaBar
{
namespace Taskbar
{
namespace Superbar
{

class CRadialGradient
{
public:
	int m_flags;
	COLORREF m_baseColor;
	COLORREF m_highlightColor;
	int m_highlightRadius;

	int m_offsetX;
	int m_offsetY;

	int m_borderLeft;
	int m_borderRight;
	int m_borderTop;
	int m_borderBottom;

	int m_width;
	int m_height;
	int m_alphaWeight;

	// TODO (kirasicecreamm): While this API is still volatile, should we consider a
	// better name for these? I think they can be easily confused for the border
	// properties above, which actually control the thickness of the border. This
	// is specifically for the drawing container.
	int m_borderWidth;
	int m_borderHeight;
	int m_borderAlphaWeight;

	enum RadialGradientFlags
	{
		NULL_FLAGS,
		DRAW_RADIAL_GRADIENT,
		DRAW_BORDER_GRADIENT,
	};

	HRESULT Paint(HDC hdc, const RECT *prcDest);

private:
	void DoDrawGradient(RGBQUAD *pRgbQuad, const BYTE colorsArr[], int colorIndex, int width, int height, int iteration, int offsetX, int offsetY);
};

} // namespace Superbar
} // namespace Taskbar
} // namespace KappaBar