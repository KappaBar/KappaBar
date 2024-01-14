#pragma once

#include "pch.h"

class CTestWindow : public ATL::CWindowImpl<CTestWindow>
{
public:
	DECLARE_WND_CLASS(L"KappaBar_TestWindow")

	BEGIN_MSG_MAP(CTestWindow)
		MESSAGE_HANDLER(WM_PAINT, OnPaint)
		MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
	END_MSG_MAP()

	LRESULT OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled);
	LRESULT OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled);
};