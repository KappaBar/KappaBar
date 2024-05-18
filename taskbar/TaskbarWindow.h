#pragma once

#include "pch.h"

namespace KappaBar
{
namespace Taskbar
{


class CTaskbarWindow : public ATL::CWindowImpl<CTaskbarWindow>
{
public:
	DECLARE_WND_CLASS(L"Shell_TrayWnd")
};

} // namespace Taskbar
} // namespace KappaBar