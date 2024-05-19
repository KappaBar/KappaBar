#pragma once

#include "pch.h"
#include "atlctrls.h"

class CTestWindow : public ATL::CWindowImpl<CTestWindow>
{
	WTL::CButton m_colorPickerButton;
	COLORREF m_color;
	COLORREF m_highlightColor;

	WCHAR m_versionString[1024];

public:
	DECLARE_WND_CLASS(L"KappaBar_TestWindow")

	BEGIN_MSG_MAP(CTestWindow)
		MESSAGE_HANDLER(WM_CREATE, OnCreate)
		MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
		MESSAGE_HANDLER(WM_PAINT, OnPaint)
		MESSAGE_HANDLER(WM_MOUSEMOVE, OnMouseMove)
		MESSAGE_HANDLER(WM_COMMAND, OnCommand)
	END_MSG_MAP()


	LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled);
	LRESULT OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled);
	LRESULT OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled);
	LRESULT OnMouseMove(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled);
	LRESULT OnCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled);
	void DrawRadialGradientBackground(HDC hdc);
	void DrawRadialGradient(int xMouse, int yMouse);
	void OpenColorPicker();
	void UpdateGradientColor(COLORREF color);
};