#pragma once

#include "pch.h"

namespace KappaBar
{
namespace Taskbar
{
namespace Tasks
{

class CWindowTask
{
private:
	HWND m_hWnd;
	DWORD m_processId;
	WCHAR m_title[1024];

public:
	CWindowTask(HWND hWnd);

	HRESULT GetPath(LPWSTR out, DWORD cchMax);
	bool IsUWP();
};

} // namespace Tasks
} // namespace Taskbar
} // namespace KappaBar