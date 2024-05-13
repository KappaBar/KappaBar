#include "pch.h"
#include "RadialGradient.h"

using namespace KappaBar::Taskbar::Superbar;

constexpr double PI = 3.1415927;
constexpr int GRADIENT_STEPS_NUM = 100;

/**
 * Draw a radial gradient with an optional border like superbar items on Windows 7.
 * 
 * This draws two things: a background radial gradient, and a border radial gradient.
 * The border is used to accentuate the content, which is used as the background of
 * highlighted (active or hovered) taskbar items in the superbar.
 */
HRESULT CRadialGradient::Paint(HDC hdc, const RECT *prcDest)
{
	// Initialization
	HRESULT hr = E_FAIL;
	bool bDrawRadialGradient = false;
	bool bDrawBorderGradient = false;

	if (
		m_flags & DRAW_RADIAL_GRADIENT
		&& m_width > 0
		&& m_height > 0
		&& m_alphaWeight > 0
	)
	{
		bDrawRadialGradient = true;
	}

	if (
		m_flags & DRAW_BORDER_GRADIENT
		&& m_borderWidth > 0
		&& m_borderHeight > 0
		&& m_borderAlphaWeight > 0
	)
	{
		bDrawBorderGradient = true;
	}

	if (!bDrawRadialGradient && !bDrawBorderGradient)
	{
		return hr;
	}

	// Recalculate the gradient width:
	int drawingWidth = 0;
	int drawingHeight = 0;

	if (bDrawRadialGradient)
	{
		drawingWidth = m_width;
		drawingHeight = m_height;
	}

	if (bDrawBorderGradient)
	{
		// If the size of the background is smaller than the border frame, then
		// coerce the size to the size of the border frame. We always use the largest
		// possible size.
		if (drawingWidth < m_borderWidth)
			drawingWidth = m_borderWidth;
		if (drawingHeight < m_borderHeight)
			drawingHeight = m_borderHeight;
	}

	// TODO (kirasicecreamm): This (hopefully) reverses the layout on RTL systems.
	// But I don't know if it's necessary. If it's done erroneously, then the gradient
	// highlight offset will be on the opposite end of where it should be. This needs
	// to be tested on an RTL system in context.
	int offsetX = (GetLayout(hdc) & LAYOUT_RTL)
		? prcDest->left + prcDest->right - m_offsetX
		: m_offsetX;
	int offsetY = m_offsetY;

	RECT rcDest;
	CopyRect(&rcDest, prcDest);

	// Prepare the destination rectangle that we will use for drawing later on:
	RECT rcTemp = { 0 };
	rcTemp.left = offsetX;
	rcTemp.right = offsetX + 1;
	rcTemp.top = offsetY;
	rcTemp.bottom = offsetY + 1;
	InflateRect(&rcTemp, drawingWidth, drawingHeight);
	if (!IntersectRect(&rcDest, prcDest, &rcTemp))
	{
		return hr;
	}

	// =========================================================================================================
	// Prepare colors:
	// ---------------------------------------------------------------------------------------------------------

	// In the case where the background radial gradient and the border gradient have
	// differing colors, we add an extra set of colors to the end of the colors array.
	// The number of steps are fixed, so we can simply handle it by appending an extra
	// number of steps to the end. This code is possible to clean up, but I don't feel
	// like doing it.
	int borderGradientIndex = 0;

	if (bDrawRadialGradient && bDrawBorderGradient && m_alphaWeight != m_borderAlphaWeight)
	{
		borderGradientIndex = GRADIENT_STEPS_NUM;
	}

	int numIterations = GRADIENT_STEPS_NUM + borderGradientIndex;

	// We allocate memory below, so you must be mindful of that before returning.
#pragma warning(suppress:26451) // pointless warning about i32 -> i64 cast
	BYTE *colorsArr = new BYTE[numIterations * 4];
	ZeroMemory(colorsArr, numIterations * 4);

	if (!colorsArr)
	{
		// No delete since the memory didn't actually get allocated.
		return E_OUTOFMEMORY;
	}

	// Next, we iterate through the colors array and fill out a linear gradient in
	// memory, which we pull from later in order draw the radial gradient:
	BYTE *colorIdx = colorsArr;
	for (int i = 0; i < numIterations; i++, colorIdx += 4)
	{
		int curPercent = i % 100;
		float cosine = cosf((float)((float)curPercent * PI) / 100.0);
		BYTE alpha = 0;
		
		if (i % 100 > 50)
		{
			alpha = (int)(float)((float)(cosine + 1.0) * 64.0);
		}
		else
		{
			alpha = 64 - (int)(float)(cosine * -191.0);
		}

		if (!bDrawRadialGradient || i >= 100 || m_alphaWeight == 0xFF)
		{
			if (bDrawBorderGradient && m_borderAlphaWeight != 0xFF)
			{
				int alphaWeight = m_borderAlphaWeight;
				// nikonote: if *= has bad effects, revert to alpha = alpha * ...
				// It seems to be alright and syntactically makes sense, but I just
				// want to be sure.
				alpha *= alphaWeight / 0xFF;
			}
			else
			{
				int alphaWeight = m_alphaWeight;
				alpha *= alphaWeight / 0xFF;
			}
		}

		// We don't run any of this code if the alpha is 0, since that means a fully
		// transparent pixel.
		if (alpha)
		{
			BYTE red = GetRValue(m_baseColor);
			BYTE green = GetGValue(m_baseColor);
			BYTE blue = GetBValue(m_baseColor);

			BYTE redHighlight = GetRValue(m_highlightColor);
			BYTE greenHighlight = GetGValue(m_highlightColor);
			BYTE blueHighlight = GetBValue(m_highlightColor);

			// Blend the highlight color with the base color:
			if (curPercent < m_highlightRadius)
			{
				int radius = m_highlightRadius;

				red += (radius - curPercent) * (redHighlight - red) / radius;
				green += (radius - curPercent) * (greenHighlight - green) / radius;
				blue += (radius - curPercent) * (blueHighlight - blue) / radius;
			}
			
			// Alpha blending:
			if (alpha == 0xFF)
			{
				colorIdx[2] = red;
				colorIdx[1] = green;
				colorIdx[0] = blue;
			}
			else
			{
				colorIdx[2] = alpha * red / 0xFF;
				colorIdx[1] = alpha * green / 0xFF;
				colorIdx[0] = alpha * blue / 0xFF;
			}
			colorIdx[3] = alpha;
		}
	}

	// =========================================================================================================
	// Prepare paint:
	// ---------------------------------------------------------------------------------------------------------

	BLENDFUNCTION blendFunction = { 0 };
	blendFunction.BlendOp = AC_SRC_OVER; // only valid option
	blendFunction.BlendFlags = 0; // only valid option
	blendFunction.SourceConstantAlpha = 0xFF;
	blendFunction.AlphaFormat = AC_SRC_ALPHA;

	BP_PAINTPARAMS paintParams = { 0 };
	paintParams.cbSize = sizeof(BP_PAINTPARAMS);
	paintParams.pBlendFunction = &blendFunction;
	paintParams.dwFlags = BPPF_ERASE;

	HDC bufferedDc;
	HPAINTBUFFER hpb = BeginBufferedPaint(hdc, prcDest, BPBF_TOPDOWNDIB, &paintParams, &bufferedDc);

	if (hpb)
	{
		RGBQUAD *pRgbQuad;
		int pcxRow = 0;
		HRESULT hrGetBits = GetBufferedPaintBits(hpb, &pRgbQuad, &pcxRow);

		if (FAILED(hrGetBits))
		{
			DWORD lastError = GetLastError();
			hr = HRESULT_FROM_WIN32(lastError);
			delete[] colorsArr;
			return hr;
		}

		DWORD dwLayout = GetLayout(hdc);

		const DWORD left = prcDest->left;
		const DWORD right = prcDest->right;
		const DWORD top = prcDest->top;
		const DWORD bottom = prcDest->bottom;

		if (dwLayout & LAYOUT_RTL)
		{
			int flippedIndex = pcxRow + left - right;
			pRgbQuad += flippedIndex;
		}

		if (top >= bottom)
		{
			goto BAIL;
		}

		int subtractedWidth = left - right;
		DWORD height = bottom - top;
		offsetY = rcDest.top - m_offsetY;

		// Iterate columns
		for (DWORD itTop = top; itTop <= bottom; itTop++)
		{
			// Iterate rows
			for (DWORD itLeft = left; itLeft < right; itLeft++)
			{
				// This code just seems to be bad. Everything seems to work alright without it.
				/*if (
					bDrawBorderGradient &&
					(
						m_borderLeft > 1 && itLeft == prcDest->left ||
						m_borderRight > 1 && itLeft == prcDest->right - 1
					) &&
					(
						m_borderTop > 1 && itTop == prcDest->top ||
						m_borderBottom > 1 && itTop == prcDest->bottom - 1
					)
				)
				{
					continue;
				}*/

				// The "background" radial gradient is drawn first so that it doesn't overlap
				// the border if we are to draw that.
				if (bDrawRadialGradient)
				{
					DoDrawGradient(
						pRgbQuad,
						colorsArr,
						0,
						m_width,
						m_height,
						itLeft,
						offsetX,
						offsetY
					);
				}

				if (
					itLeft <  prcDest->left + m_borderLeft ||
					itLeft >= prcDest->right - m_borderRight ||
					itTop < prcDest->top + m_borderTop ||
					itTop >= prcDest->bottom - m_borderBottom
				)
				{
					if (bDrawBorderGradient)
					{
						DoDrawGradient(
							pRgbQuad,
							colorsArr,
							borderGradientIndex,
							m_borderWidth,
							m_borderHeight,
							itLeft,
							offsetX,
							offsetY
						);
					}
				}
				
				++pRgbQuad;
			}

			++offsetY;
			pRgbQuad += pcxRow + subtractedWidth;
		}

BAIL:
		EndBufferedPaint(hpb, TRUE);
	}
	else
	{
		delete[] colorsArr;
		return GetLastError();
	}

	delete[] colorsArr;

	return S_OK;
}

/**
 * Performs the drawing of the radial gradient.
 * 
 * This is an insane part of the algorithm which I couldn't understand very well, so the code
 * may be difficult to follow along with.
 */
void CRadialGradient::DoDrawGradient(RGBQUAD *pRgbQuad, const BYTE colorsArr[], int colorIndex, int width, int height, int iteration, int offsetX, int offsetY)
{
	DWORD newHeight = height;
	DWORD newWidth = newHeight;

	if (width > newHeight)
	{
		newWidth = width;
	}

	int roundedWidth = (int)(newWidth * abs((int)(iteration - offsetX))) / width;
	int roundedHeight = (int)(newWidth * abs(offsetY));
	int c = roundedWidth * roundedWidth + roundedHeight / newHeight * (roundedHeight / newHeight);

	if (c <= newWidth * newWidth)
	{
		double theSqrt = sqrt((double)c);
		int i = 100 * (int)theSqrt / newWidth;
		if (i < 100)
		{
			// Save the original contents of the paint buffer. We may use this later.
			RGBQUAD originalBackground = *pRgbQuad;

			int index = i + colorIndex;
			BYTE alphaValue = -1 - colorsArr[4 * index + 3];
			bool isOpaque = colorsArr[4 * index + 3] == 0xFF;

			// Draw over the paint buffer:
			*pRgbQuad = *(RGBQUAD *)&colorsArr[4 * index];

			// This algorithm darkens the intersecting pixels of the background, which
			// is noticable on the border gradient if both the border and background
			// gradients are enabled. I really tried to make sense of it, but I just couldn't.
			// If someone can recognize this algorithm, it would be really nice to put a clean
			// implementation here.
			if (!isOpaque)
			{
				DWORD bgColor = *(unsigned int *)&originalBackground;

				*(DWORD *)pRgbQuad += (alphaValue * ((bgColor >> 8) & 0xFF00FF) + 0x800080 + (((alphaValue * ((bgColor >> 8) & 0xFF00FF) + 0x800080) >> 8) & 0xFF00FF)) ^ ((alphaValue * ((bgColor >> 8) & 0xFF00FF) + 0x800080 + (((alphaValue * ((bgColor >> 8) & 0xFF00FF) + 0x800080) >> 8) & 0xFFFF00FF)) ^ ((alphaValue * (bgColor & 0xFF00FF) + 0x800080 + (((alphaValue * (bgColor & 0xFF00FF) + 0x800080) >> 8) & 0xFF00FF)) >> 8)) & 0xFF00FF;
			}
		}
	}
}