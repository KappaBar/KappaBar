#pragma once

#include "pch.h"

namespace KappaBar
{
namespace Taskbar
{

extern bool g_taskbarInitialized;
extern UINT g_msgSHELLHOOK;
extern UINT g_msgTASKBARCREATED;
extern UINT g_msgTASKBARBUTTONCREATED;

bool InitializeTaskbarGlobalState();

} // namespace Taskbar
} // namespace KappaBar