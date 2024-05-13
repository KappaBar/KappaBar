#include "pch.h"
#include "Taskbar.h"
#include "util/debug.h"

using namespace KappaBar::Taskbar;

CTaskbar::CTaskbar(HWND owner)
	: m_hWndOwner(owner)
{}

HRESULT CTaskbar::Initialize()
{
	/*
	 * Contributors (aubrey), please read:
	 * 
	 * Something tells me that this is a weird approach to go about this. I think
	 * that it'd produce a bad architecture.
	 * 
	 * Most of the taskbar works by receiving window messages directly about state
	 * mutations. That means that, at the very least, new state would have to be
	 * bounced from the window to this code, only for us to most likely feed the
	 * information back to the window anyways.
	 * 
	 * Quite possibly it would just be better to create a base class from which we
	 * could derive classic taskbar and superbar window implementations? At least,
	 * in that case, we could inherit the message handler map and provide common
	 * implementations with virtual methods.
	 * 
	 * Although one goal of my design here was to be able to reuse the same taskbar
	 * class instance (this controller class) for multiple taskbar windows for
	 * simple multi-monitor support. It seems that such an architecture would be a
	 * bit of a mess anyways though, since that would require a listener window.
	 * 
	 * ^^ For multi-monitor support, I think having one central taskbar that can
	 * message information (even things that aren't officially supported like tray
	 * item state) to sibling taskbars could work out pretty well.
	 */
	RegisterShellHookWindow(m_hWndOwner);

	// nikonote: Permissions must be set so we can receive messages from unprivileged
	// processes if we ourselves are privileged.
	// https://stackoverflow.com/questions/28697559/registered-window-message-taskbarbuttoncreated-not-received

	return S_OK;
}

LRESULT CTaskbar::HandleTaskbarMessage(HWND hWndTaskbar, UINT uMsg, WPARAM wParam, LPARAM lParam, OUT bool *shouldContinue)
{
	switch (uMsg)
	{
		case HSHELL_WINDOWCREATED:
		{
			KappabarDebugPrint(L"HandleTaskbarMessage: window created");
			break;
		}

		case HSHELL_WINDOWDESTROYED:
		{
			KappabarDebugPrint(L"HandleTaskbarMessage: window replaced");
			break;
		}

		case HSHELL_WINDOWREPLACING:
		{
			KappabarDebugPrint(L"HandleTaskbarMessage: window replacing");
			break;
		}

		case HSHELL_WINDOWREPLACED:
		{
			KappabarDebugPrint(L"HandleTaskbarMessage: window replaced");
			break;
		}

		case HSHELL_WINDOWACTIVATED:
		{
			KappabarDebugPrint(L"HandleTaskbarMessage: window activated");
			break;
		}

		case HSHELL_RUDEAPPACTIVATED:
		{
			KappabarDebugPrint(L"HandleTaskbarMessage: rude app activated");
			break;
		}

		case HSHELL_FLASH:
		{
			KappabarDebugPrint(L"HandleTaskbarMessage: flash event");
			break;
		}

		case HSHELL_ACTIVATESHELLWINDOW:
		{
			KappabarDebugPrint(L"HandleTaskbarMessage: shell window activated");
			break;
		}

		case HSHELL_ENDTASK:
		{
			KappabarDebugPrint(L"HandleTaskbarMessage: task ended");
			break;
		}

		case HSHELL_GETMINRECT:
		{
			KappabarDebugPrint(L"HandleTaskbarMessage: getting minimize rect");
			break;
		}

		case HSHELL_REDRAW:
		{
			KappabarDebugPrint(L"HandleTaskbarMessage: redraw command");
			break;
		}

		case HSHELL_TASKMAN:
		{
			KappabarDebugPrint(L"HandleTaskbarMessage: task manager command");
			break;
		}
	}

	if (shouldContinue)
	{
		*shouldContinue = true;
	}

	return 0;
}