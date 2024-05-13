#include "pch.h"
#include "WindowTask.h"

using namespace KappaBar::Taskbar::Tasks;

CWindowTask::CWindowTask(HWND hWnd)
	: m_hWnd(hWnd)
{
	GetWindowThreadProcessId(hWnd, &m_processId);
}

HRESULT CWindowTask::GetPath(LPWSTR out, DWORD cchMax)
{
	if (!m_processId)
	{
		return E_FAIL;
	}

	HANDLE hProc = OpenProcess(
		PROCESS_QUERY_LIMITED_INFORMATION,
		FALSE,
		m_processId
	);

	QueryFullProcessImageNameW(hProc, NULL, out, &cchMax);

	// Is there any particular reason why ManagedShell doesn't do this?
	CloseHandle(hProc);

	return S_OK;
}

bool CWindowTask::IsUWP()
{
	WCHAR path[1024];
	GetPath(path, 1024);

	return _wcsicmp(path, L"applicationframehost.exe");
}