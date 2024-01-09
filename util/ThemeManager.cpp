#include "pch.h"
#include "thememanager.h"
#include "util.h"

/* Set the current theme. This does not load the theme. */
void CThemeManager::_SetTheme(LPCWSTR pszThemePath)
{
	if (pszThemePath && *pszThemePath)
	{
		wcsncpy_s(m_szThemePath, pszThemePath, ARRAYSIZE(m_szThemePath) - 1);
	}
	else
	{
		ZeroMemory(m_szThemePath, sizeof(m_szThemePath));
	}
}

/* Load the currently selected theme. */
HRESULT CThemeManager::_LoadTheme(void)
{
	HRESULT hr = S_OK;
	
	_UnloadTheme();

	if (*m_szThemePath)
	{
		WCHAR szColor[MAX_PATH];
		WCHAR szSize[MAX_PATH];

		hr = _GetThemeDefaults(
			m_szThemePath,
			szColor,
			ARRAYSIZE(szColor),
			szSize,
			ARRAYSIZE(szSize)
		);
		RETURN_IF_FAILED(hr);

		HANDLE hSharable, hNonSharable;

		hr = _LoaderLoadTheme(
			NULL, NULL,
			m_szThemePath,
			szColor, szSize,
			&hSharable,
			NULL, 0,
			&hNonSharable,
			NULL, 0, NULL, NULL,
			NULL, NULL, FALSE
		);
		RETURN_IF_FAILED(hr);

		ZeroMemory(&m_themeFile, sizeof(UXTHEMEFILE));
		memcpy(m_themeFile.header, "thmfile", 7);
		memcpy(m_themeFile.end, "end", 3);
		m_themeFile.sharableSectionView = MapViewOfFile(hSharable, 4, 0, 0, 0);
		m_themeFile.hSharableSection = hSharable;
		m_themeFile.nsSectionView = MapViewOfFile(hNonSharable, 4, 0, 0, 0);
		m_themeFile.hNsSection = hNonSharable;
	}
	else
	{
		_UnloadTheme();
	}

	return S_OK;
}

void CThemeManager::_UnloadTheme(void)
{
	if (m_themeFile.sharableSectionView)
	{
		UnmapViewOfFile(m_themeFile.sharableSectionView);
	}
	if (m_themeFile.nsSectionView)
	{
		UnmapViewOfFile(m_themeFile.nsSectionView);
	}
	ZeroMemory(&m_themeFile, sizeof(UXTHEMEFILE));
}

CThemeManager::CThemeManager(LPCWSTR pszThemePath)
	: m_szThemePath{ 0 }
	, m_themeFile{ 0 }
{
	_SetTheme(pszThemePath);
}

HRESULT CThemeManager::_GetThemeDefaults(
	LPCWSTR pszThemeFileName,
	LPWSTR  pszColorName,
	DWORD   dwColorNameLen,
	LPWSTR  pszSizeName,
	DWORD   dwSizeNameLen
) const
{
	if (m_pGetThemeDefaults)
	{
		return m_pGetThemeDefaults(
			pszThemeFileName,
			pszColorName,
			dwColorNameLen,
			pszSizeName,
			dwSizeNameLen
		);
	}
	return E_POINTER;
}

HRESULT CThemeManager::_LoaderLoadTheme(
	HANDLE      hThemeFile,
	HINSTANCE   hThemeLibrary,
	LPCWSTR     pszThemeFileName,
	LPCWSTR     pszColorParam,
	LPCWSTR     pszSizeParam,
	OUT HANDLE *hSharableSection,
	LPWSTR      pszSharableSectionName,
	int         cchSharableSectionName,
	OUT HANDLE *hNonsharableSection,
	LPWSTR      pszNonsharableSectionName,
	int         cchNonsharableSectionName,
	PVOID       pfnCustomLoadHandler,
	OUT HANDLE *hReuseSection,
	int         a,
	int         b,
	BOOL        fEmulateGlobal
) const
{
	if (m_pLoaderLoadTheme)
	{
		return m_pLoaderLoadTheme(
			hThemeFile,
			hThemeLibrary,
			pszThemeFileName,
			pszColorParam,
			pszSizeParam,
			hSharableSection,
			pszSharableSectionName,
			cchSharableSectionName,
			hNonsharableSection,
			pszNonsharableSectionName,
			cchNonsharableSectionName,
			pfnCustomLoadHandler,
			hReuseSection,
			a,
			b,
			fEmulateGlobal
		);
	}
	return E_POINTER;
}

HTHEME CThemeManager::_OpenThemeDataFromFile(
	LPUXTHEMEFILE lpThemeFile,
	HWND          hWnd,
	LPCWSTR       pszClassList,
	DWORD         dwFlags
) const
{
	if (m_pOpenThemeDataFromFile)
	{
		return m_pOpenThemeDataFromFile(
			lpThemeFile,
			hWnd,
			pszClassList,
			dwFlags
		);
	}
	return NULL;
}

HRESULT CThemeManager::Initialize(void)
{
	HMODULE hUxTheme = GetModuleHandleW(L"uxtheme.dll");
	if (!hUxTheme)
	{
		return E_UNEXPECTED;
	}

	m_pGetThemeDefaults = (GetThemeDefaults_t)GetProcAddress(hUxTheme, (LPCSTR)7);
	m_pLoaderLoadTheme = (LoaderLoadTheme_t)GetProcAddress(hUxTheme, (LPCSTR)92);
	m_pOpenThemeDataFromFile = (OpenThemeDataFromFile_t)GetProcAddress(hUxTheme, (LPCSTR)16);

	if (!m_pGetThemeDefaults || !m_pLoaderLoadTheme || !m_pOpenThemeDataFromFile)
	{
		return E_POINTER;
	}

	HRESULT hr = _LoadTheme();
	RETURN_IF_FAILED(hr);

	return S_OK;
}

HTHEME CThemeManager::OpenThemeData(HWND hWnd, LPCWSTR pszClassList, DWORD dwFlags)
{
	if (*m_szThemePath)
	{
		return _OpenThemeDataFromFile(&m_themeFile, hWnd, pszClassList, dwFlags);
	}
	else
	{
		return OpenThemeDataEx(hWnd, pszClassList, dwFlags);
	}
}

/* Set the current theme and reload it. */
HRESULT CThemeManager::SwitchTheme(LPCWSTR pszThemePath)
{
	_SetTheme(pszThemePath);
	return _LoadTheme();
}

/**
  * Get the current theme path.
  * Return value is the amount of characters copied.
  */
DWORD CThemeManager::GetThemePath(LPWSTR pszOut, DWORD cchMax)
{
	if (pszOut && cchMax && m_szThemePath && *m_szThemePath)
	{
		DWORD strlen = (DWORD)wcslen(m_szThemePath);
		DWORD dwLen = MIN(strlen, MAX(strlen, cchMax));
		wcsncpy_s(pszOut, dwLen, m_szThemePath, dwLen);
		return MIN(strlen, cchMax);
	}
	return 0;
}