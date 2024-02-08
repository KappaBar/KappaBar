#include "GlobalManager.h"

bool g_taskbarInitialized = false;
UINT g_msgSHELLHOOK = 0;
UINT g_msgTASKBARCREATED = 0;
UINT g_msgTASKBARBUTTONCREATED = 0;

bool InitializeTaskbarGlobalState()
{
	// I see a lot of implementations that just do this during window creation,
	// but I designed this in such a way just in case.
	g_msgSHELLHOOK = RegisterWindowMessageW(L"SHELLHOOK");
	g_msgTASKBARCREATED = RegisterWindowMessageW(L"TaskbarCreated");
	g_msgTASKBARBUTTONCREATED = RegisterWindowMessageW(L"TaskbarButtonCreated");
	
	g_taskbarInitialized = g_msgSHELLHOOK && g_msgTASKBARCREATED && g_msgTASKBARBUTTONCREATED;
	return g_taskbarInitialized;
}