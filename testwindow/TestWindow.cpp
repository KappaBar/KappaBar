#include "pch.h"
#include "TestWindow.h"
#include "Version.h"
#include "taskbar/superbar/RadialGradient.h"

using KappaBar::Taskbar::Superbar::CRadialGradient;

LRESULT CTestWindow::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
	m_colorPickerButton = CreateWindowExW(
		0,
		L"BUTTON",
		L"Change color",
		WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
		50,
		125 + 44 + 5,
		120,
		30,
		m_hWnd,
		NULL,
		(HINSTANCE)GetWindowLongPtrW(GWLP_HINSTANCE),
		NULL
	);

	UpdateGradientColor(RGB(255, 106, 0));

	WCHAR szDate[256]    = { 0 };
	WCHAR szVersion[256] = { 0 };
	
	KappaBar::Util::GetBuildDateFull(szDate, 256);
	KappaBar::Util::GetVersion(szVersion, 256);

	wsprintfW(
		m_versionString,
		L"KappaBar %s (build %d, %s)",
		szVersion,
		KappaBar::Util::GetBuildNumber(),
		szDate
	);

	bHandled = true;
	return 0;
}

LRESULT CTestWindow::OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
	PostQuitMessage(0);
	bHandled = true;
	return 0;
}

LRESULT CTestWindow::OnCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
	if ((HWND)lParam == m_colorPickerButton)
	{
		OpenColorPicker();
	}

	bHandled = true;
	return 0;
}

LRESULT CTestWindow::OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
	PAINTSTRUCT ps;
	BeginPaint(&ps);

	RECT rcClient;
	GetClientRect(&rcClient);

	NONCLIENTMETRICSW ncm = { 0 };
	ncm.cbSize = sizeof(ncm);
	SystemParametersInfoW(
		SPI_GETNONCLIENTMETRICS,
		sizeof(ncm),
		&ncm,
		NULL
	);
	HFONT hfMsg = CreateFontIndirectW(&ncm.lfMessageFont);
	HFONT hfOld = (HFONT)SelectObject(ps.hdc, hfMsg);

	BITMAP bmp = { 0 };
	HGDIOBJ obj = GetCurrentObject(ps.hdc, OBJ_BITMAP);
	GetObjectW(obj, sizeof(BITMAP), &bmp);

	DrawTextW(ps.hdc, m_versionString, -1, &rcClient, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

	RECT a = { 50, 105, rcClient.right, rcClient.bottom };
	DrawTextW(ps.hdc, L"Superbar radial gradient test:", 31, &a, DT_SINGLELINE);
	DrawRadialGradientBackground(ps.hdc);

	SelectObject(ps.hdc, hfOld);
	DeleteObject(hfMsg);

	EndPaint(&ps);

	bHandled = true;
	return 0;
}

LRESULT CTestWindow::OnMouseMove(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
	int xPos = GET_X_LPARAM(lParam);
	int yPos = GET_Y_LPARAM(lParam);

	DrawRadialGradient(xPos, yPos);

	bHandled = true;
	return 0;
}

void CTestWindow::DrawRadialGradientBackground(HDC hdc)
{
	RECT rcClient;
	GetClientRect(&rcClient);

	RECT drawingRect = {
		rcClient.left + 50,
		rcClient.top + 125,
		rcClient.left + 50 + 60,
		rcClient.top + 125 + 44
	};

	FillRect(hdc, &drawingRect, (HBRUSH)LTGRAY_BRUSH);
}

/**
 * Draws a radial gradient like the Windows 7 taskbar.
 * 
 * This reference implementation tries to replicate something closely, so it might also act
 * as a decent enough reference for future implementation.
 */
void CTestWindow::DrawRadialGradient(int xMouse, int yMouse)
{
	RECT rcClient;
	GetClientRect(&rcClient);
	HDC dc = GetDC();
	RECT drawingRect = {
		rcClient.left + 50,
		rcClient.top + 125,
		rcClient.left + 50 + 60,
		rcClient.top + 125 + 44
	};

	HDC memDc = CreateCompatibleDC(dc);
	HBITMAP memBmp = CreateCompatibleBitmap(dc, drawingRect.right - rcClient.left, drawingRect.bottom - rcClient.top);
	HGDIOBJ oldObj = SelectObject(memDc, memBmp);

	DrawRadialGradientBackground(memDc);

	CRadialGradient gradientTest = { 0 };

	int offsetX = 0, offsetY = 0;
	if (xMouse > drawingRect.left && xMouse < drawingRect.right)
	{
		offsetX = xMouse;
	}
	if (yMouse > drawingRect.top && yMouse < drawingRect.bottom)
	{
		offsetY = drawingRect.bottom;
	}

	if (offsetX && offsetY)
	{
		int sizeBase = 7 * (drawingRect.bottom - drawingRect.top) / 4;
		
		// For drawing the gradient, the Windows 7 taskbar passes in the alpha weight
		// (which is the first 0xFF here) as an argument. Here, it's just hardcoded.
		int width = sizeBase * 0xFF / 0xFF;
		int height = sizeBase * 0xFF / 0xFF;

		gradientTest.m_flags = CRadialGradient::DRAW_RADIAL_GRADIENT | CRadialGradient::DRAW_BORDER_GRADIENT;
		gradientTest.m_baseColor = m_color;
		gradientTest.m_highlightColor = m_highlightColor;
		gradientTest.m_highlightRadius = 30;
		gradientTest.m_borderTop = 2;
		gradientTest.m_borderLeft = 2;
		gradientTest.m_borderBottom = 2;
		gradientTest.m_borderRight = 2;
		gradientTest.m_width = width;
		gradientTest.m_height = height;
		gradientTest.m_alphaWeight = 0xFF;
		gradientTest.m_borderWidth = 3 * width / 2;
		gradientTest.m_borderHeight = 3 * height / 2;
		gradientTest.m_borderAlphaWeight = 0xFF;
		gradientTest.m_offsetX = offsetX;
		gradientTest.m_offsetY = offsetY;
		gradientTest.Paint(memDc, &drawingRect);
	}

	BitBlt(
		dc,
		drawingRect.left, drawingRect.top,
		drawingRect.right - drawingRect.left,
		drawingRect.bottom - drawingRect.top,
		memDc,
		drawingRect.left, drawingRect.top,
		SRCCOPY
	);

	SelectObject(memDc, oldObj);
	DeleteObject(memBmp);
	DeleteDC(memDc);
}

void CTestWindow::OpenColorPicker()
{
	CHOOSECOLORW chooseColor = { 0 };
	chooseColor.lStructSize = sizeof(CHOOSECOLORW);
	chooseColor.hwndOwner = m_hWnd;
	chooseColor.rgbResult = m_color;
	chooseColor.Flags = CC_FULLOPEN | CC_RGBINIT;

	// This is of course undocumented, but you need to fill this or an access violation
	// will occur:
	// Thank you to this forum post from 2005: 
    // https://forums.codeguru.com/showthread.php?347910-ChooseColor-Dialog-Crashes-Straight-from-MSDN
	COLORREF FUCKYOU = 0;
	chooseColor.lpCustColors = &FUCKYOU;

	if (ChooseColorW(&chooseColor))
	{
		UpdateGradientColor(chooseColor.rgbResult);
	}
}

void CTestWindow::UpdateGradientColor(COLORREF color)
{
	m_color = color;

	// Calculation algorithm from Windows 7 explorer (CTaskBtnGroup::_DrawHotTrackingLight)
	// It looks a little off, but presumably that's just because it isn't meant to be used
	// with dark colors.
	m_highlightColor =
		(((BYTE)(GetRValue(m_color) + (3 * (255 - GetRValue(m_color)) / 4)))) |
		(((BYTE)(GetGValue(m_color) + (3 * (255 - GetGValue(m_color)) / 4))) << 8) |
		(((BYTE)(GetBValue(m_color) + (3 * (255 - GetBValue(m_color)) / 4))) << 16);
}