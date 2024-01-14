#pragma once

#include "pch.h"

class CTestWindow : public ATL::CWindowImpl<CTestWindow>
{
public:
	BEGIN_MSG_MAP(CTestWindow)
		MESSAGE_HANDLER(WM_PAINT, OnPaint)
	END_MSG_MAP()

	LRESULT OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled);
};