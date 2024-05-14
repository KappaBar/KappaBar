#pragma once

#include "pch.h"

namespace KappaBar
{
namespace Taskbar
{

class CTaskbar
{
	HWND m_hWndOwner = nullptr;
	int m_taskbarInitialState = 0;

private:
	HWND _GetNativeTaskbarHandle();

	HRESULT _SetTaskbarVisibility(int visibility);
	void _SetTaskbarState(int state);

public:
	CTaskbar(HWND owner);
	HRESULT Initialize();
	LRESULT HandleTaskbarMessage(HWND hWndTaskbar, UINT uMsg, WPARAM wParam, LPARAM lParam, OUT bool *shouldContinue);

	HRESULT HideNativeTaskbar();
	HRESULT ShowNativeTaskbar();
};

} // namespace Taskbar
} // namespace KappaBar