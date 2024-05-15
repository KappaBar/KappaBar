#include "pch.h"
#include "Taskbar.h"
#include "util/debug.h"

using namespace KappaBar::Taskbar;

#pragma region "private:"
/**
  * In the future, this method should be updated to check for and avoid KappaBar's
  * taskbar window. Since this doesn't exist right now, this is fine.
  */
HWND CTaskbar::_GetNativeTaskbarHandle()
{
	return FindWindowW(L"Shell_TrayWnd", NULL);
}

/* See HideNativeTaskbar and ShowNativeTaskbar */
HRESULT CTaskbar::_SetTaskbarVisibility(int visibility)
{
	HWND hTaskbar = _GetNativeTaskbarHandle();
	if (hTaskbar)
	{
		if (!SetWindowPos(
			hTaskbar,
			HWND_BOTTOM,
			0, 0, 0, 0,
			visibility | SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE
		))
		{
			return HRESULT_FROM_WIN32(GetLastError());
		}

		/* The start button isn't owned by the taskbar, presumably a leftover
		   from the Windows 7 orb start button style.*/
		HWND hStartButton = FindWindowExW(NULL, NULL, (LPCWSTR)0xC017, NULL);
		if (hStartButton)
		{
			if (!SetWindowPos(
				hTaskbar,
				HWND_BOTTOM,
				0, 0, 0, 0,
				visibility | SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE
			))
			{
				return HRESULT_FROM_WIN32(GetLastError());
			}
		}

		_SetSecondaryTaskbarVisibility(visibility);
	}

	return S_OK;
}

HRESULT CTaskbar::_SetSecondaryTaskbarVisibility(int visibility)
{
	HWND hWndSecondaryTaskbar = nullptr;

	while (hWndSecondaryTaskbar = FindWindowExW(nullptr, hWndSecondaryTaskbar, L"Shell_SecondaryTrayWnd", nullptr))
	{
		if (!SetWindowPos(
			hWndSecondaryTaskbar,
			HWND_BOTTOM,
			0, 0, 0, 0,
			visibility | SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE
		))
		{
			return HRESULT_FROM_WIN32(GetLastError());
		}

		// Like ManagedShell, we don't set the visibility of the start button on secondary
		// taskbars. I'm not sure why this design decision was made, but I didn't want to
		// look into it either.
	}

	return S_OK;
}

/*
 * Hack for testing purposes because we don't have a HWND yet. 
 */
static CTaskbar *g_latestInstance = nullptr;

void CALLBACK CTaskbar::s_EnsureTaskbarHiddenTimerProc(HWND hWnd, UINT uMsg, UINT_PTR uEventId, DWORD dwTime)
{
	// ManagedShell duplicates this procedure rather than calling into these functions, but I don't
	// know if there is any significance to this.
	// https://github.com/cairoshell/ManagedShell/blob/master/src/ManagedShell.AppBar/ExplorerHelper.cs#L164-L191
	// Also, it would probably be a good idea to rewrite this behaviour to cache the known HWNDs instead of
	// searching over and over again, but it should be fine for now.
	if (g_latestInstance)
	{
		g_latestInstance->_SetTaskbarVisibility(SWP_HIDEWINDOW);
		g_latestInstance->_SetSecondaryTaskbarVisibility(SWP_HIDEWINDOW);
	}
}

/* See HideNativeTaskbar and ShowNativeTaskbar */
void CTaskbar::_SetTaskbarState(int state)
{
	APPBARDATA abd = { sizeof(APPBARDATA) };
	abd.hWnd = _GetNativeTaskbarHandle();
	abd.lParam = state;
	
	/* SHAppBarMessage always returns TRUE for ABM_SETSTATE, so there's no way to
	   tell if this succeeded or not. */
	SHAppBarMessage(ABM_SETSTATE, &abd);
}
#pragma endregion // "private:"

#pragma region "public:"
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

/* These two functions only handle the main taskbar right now.
   Should probably be reworked in the future. */

HRESULT CTaskbar::HideNativeTaskbar()
{
	HRESULT hr = E_FAIL;

	if (!m_taskbarInitialState)
	{
		APPBARDATA abd = { sizeof(APPBARDATA) };
		abd.hWnd = _GetNativeTaskbarHandle();
		m_taskbarInitialState = SHAppBarMessage(
			ABM_GETSTATE, &abd
		);
	}
	_SetTaskbarState(ABS_AUTOHIDE);
	hr = _SetTaskbarVisibility(SWP_HIDEWINDOW);

	// As stated above, the weird code involving a global reference to the instance
	// can be resolved once we have access to a proper object that can store this
	// information, such as a window. Because we're testing with no window, global
	// state is used instead.
	g_latestInstance = this;
	SetTimer(
		/* hWnd: */ nullptr,
		/* nIDEvent: */ (UINT_PTR)this,
		/* uElapse: */ 100,
		/* lpTimerFunc: */ s_EnsureTaskbarHiddenTimerProc
	);

	return hr;
}

HRESULT CTaskbar::ShowNativeTaskbar()
{
	HRESULT hr = E_FAIL;

	_SetTaskbarState(m_taskbarInitialState);
	hr = _SetTaskbarVisibility(SWP_SHOWWINDOW);

	KillTimer(
		/* hWnd: */ nullptr,
		/* uIDEvent: */ (UINT_PTR)this
	);

	return hr;
}
#pragma endregion // "public:"