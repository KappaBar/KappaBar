#include "pch.h"
#include "framework.h"
#include "resource.h"
#include "KappaBar_i.h"
#include <uxtheme.h>

#include "taskbar/GlobalManager.h"
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
	BufferedPaintInit();

	InitializeTaskbarGlobalState();

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
	return 0;
}