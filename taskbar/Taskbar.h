#pragma once

#include "pch.h"

class CTaskbar
{
	HWND m_hWndOwner = nullptr;

public:
	CTaskbar(HWND owner);
	HRESULT Initialize();
};
