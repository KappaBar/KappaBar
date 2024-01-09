/**
  * CThemeManager
  * 
  * A wrapper class for an undocumented UXTheme function, LoaderLoadTheme
  * which allow the loading and using of external theme files. Luckily for
  * us, LoaderLoadTheme skips the signature check enforced by the global
  * theme entirely, so we can load any theme as long as it's in a valid format.
  */

#include <windows.h>
#include <uxtheme.h>

typedef struct _UXTHEMEFILE
{
	char header[7]; // must be "thmfile"
	LPVOID sharableSectionView;
	HANDLE hSharableSection;
	LPVOID nsSectionView;
	HANDLE hNsSection;
	char end[3]; // must be "end"
} UXTHEMEFILE, *LPUXTHEMEFILE;

typedef HRESULT (WINAPI *GetThemeDefaults_t)(
	LPCWSTR pszThemeFileName,
	LPWSTR  pszColorName,
	DWORD   dwColorNameLen,
	LPWSTR  pszSizeName,
	DWORD   dwSizeNameLen
);

typedef HRESULT (WINAPI *LoaderLoadTheme_t)(
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
);

typedef HTHEME (WINAPI *OpenThemeDataFromFile_t)(
	LPUXTHEMEFILE lpThemeFile,
	HWND        hWnd,
	LPCWSTR     pszClassList,
	DWORD       dwFlags
);

class CThemeManager
{
private:
	GetThemeDefaults_t m_pGetThemeDefaults;
	LoaderLoadTheme_t m_pLoaderLoadTheme;
	OpenThemeDataFromFile_t m_pOpenThemeDataFromFile;

	WCHAR m_szThemePath[MAX_PATH];
	UXTHEMEFILE m_themeFile;

	void _SetTheme(LPCWSTR pszThemePath);
	HRESULT _LoadTheme(void);
	void _UnloadTheme(void);

	HRESULT _GetThemeDefaults(
		LPCWSTR pszThemeFileName,
		LPWSTR  pszColorName,
		DWORD   dwColorNameLen,
		LPWSTR  pszSizeName,
		DWORD   dwSizeNameLen
	) const;

	HRESULT _LoaderLoadTheme(
		HANDLE      hThemeFile,
		HINSTANCE	hThemeLibrary,
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
	) const;

	HTHEME _OpenThemeDataFromFile(
		LPUXTHEMEFILE lpThemeFile,
		HWND          hWnd,
		LPCWSTR       pszClassList,
		DWORD         dwFlags
	) const;

public:
	CThemeManager(LPCWSTR pszThemePath);
	
	HRESULT Initialize(void);
	HTHEME OpenThemeData(HWND hWnd, LPCWSTR pszClassList, DWORD dwFlags);
	HRESULT SwitchTheme(LPCWSTR pszThemePath);
	DWORD GetThemePath(LPWSTR pszOut, DWORD cchMax);
};