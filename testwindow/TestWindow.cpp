#include "pch.h"
#include "TestWindow.h"

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

	DrawTextW(ps.hdc, L"Hello", 5, &rcClient, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

	SelectObject(ps.hdc, hfOld);
	DeleteObject(hfMsg);
	EndPaint(&ps);

	bHandled = true;
	return 0;
}

LRESULT CTestWindow::OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
	PostQuitMessage(0);
	bHandled = true;
	return 0;
}