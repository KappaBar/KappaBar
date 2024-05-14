#include "pch.h"
#include "framework.h"
#include "resource.h"
#include "KappaBar_i.h"
#include <uxtheme.h>

#include "taskbar/GlobalManager.h"
#include "taskbar/Taskbar.h"
#include "testwindow/TestWindow.h"

const WCHAR szMutexName[] = L"KappaBar";

CAppModule g_appModule;

// Insertion point
extern "C" int WINAPI wWinMain(
		HINSTANCE hInstance,
		HINSTANCE hPrevInstance,
		LPTSTR lpCmdLine, 
		int nShowCmd
)
{
	/* Disallow multiple instances */
	HANDLE hMutex = CreateMutexW(NULL, TRUE, szMutexName);
	if (GetLastError() == ERROR_ALREADY_EXISTS)
	{
		/* TODO: localize */
		MessageBoxW(NULL, L"KappaBar is already running", L"KappaBar", MB_ICONERROR);
		return 1;
	}

	g_appModule.Init(NULL, hInstance);
	BufferedPaintInit();

	KappaBar::Taskbar::InitializeTaskbarGlobalState();
	
	KappaBar::Taskbar::CTaskbar taskbar(NULL);
	taskbar.HideNativeTaskbar();

	CTestWindow *mainWindow = new CTestWindow;
	CMessageLoop messageLoop;
	HWND hWndMainWindow = mainWindow->Create(
		NULL,
		NULL,
		L"Test",
		WS_OVERLAPPEDWINDOW,
		NULL,
		nullptr,
		NULL
	);
	mainWindow->ShowWindow(SW_SHOW);
	mainWindow->UpdateWindow();

	messageLoop.Run();

	BufferedPaintUnInit();
	g_appModule.Term();

	taskbar.ShowNativeTaskbar();

	/* Allow new instance again */
	ReleaseMutex(hMutex);
	CloseHandle(hMutex);

	return 0;
}