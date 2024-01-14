#include "pch.h"
#include "TestWindow.h"

LRESULT CTestWindow::OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
	PAINTSTRUCT ps;
	BeginPaint(&ps);

	RECT rcClient;
	GetClientRect(&rcClient);

	DrawTextW(ps.hdc, L"Hello", 5, &rcClient, DT_CENTER);

	EndPaint(&ps);

	bHandled = true;
	return 0;
}