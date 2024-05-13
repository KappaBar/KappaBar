#pragma once

#include "pch.h"

namespace KappaBar
{
namespace Taskbar
{

class CTaskbar
{
	HWND m_hWndOwner = nullptr;

public:
	CTaskbar(HWND owner);
	HRESULT Initialize();
	LRESULT HandleTaskbarMessage(HWND hWndTaskbar, UINT uMsg, WPARAM wParam, LPARAM lParam, OUT bool *shouldContinue);
};

} // namespace Taskbar
} // namespace KappaBar