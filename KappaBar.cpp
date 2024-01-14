#include "pch.h"
#include "framework.h"
#include "resource.h"
#include "KappaBar_i.h"

#include "testwindow/TestWindow.h"

CAppModule g_appModule;

// Insertion point
extern "C" int WINAPI wWinMain(
		HINSTANCE hInstance,
		HINSTANCE hPrevInstance,
		LPTSTR lpCmdLine, 
		int nShowCmd
)
{
	g_appModule.Init(NULL, hInstance);

	CTestWindow *mainWindow = new CTestWindow;
	CMessageLoop messageLoop;
	HWND hWndMainWindow = mainWindow->Create(
		NULL,
		NULL,
		L"Hi",
		WS_OVERLAPPEDWINDOW,
		NULL,
		nullptr,
		NULL
	);
	ShowWindow(hWndMainWindow, SW_SHOW);
	UpdateWindow(hWndMainWindow);

	messageLoop.Run();

	g_appModule.Term();
	return 0;
}