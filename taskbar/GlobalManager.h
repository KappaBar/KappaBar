#pragma once

#include "pch.h"

extern bool g_taskbarInitialized;
extern UINT g_msgSHELLHOOK;
extern UINT g_msgTASKBARCREATED;
extern UINT g_msgTASKBARBUTTONCREATED;

bool InitializeTaskbarGlobalState();