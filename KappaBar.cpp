// KappaBar.cpp : Implementation of WinMain


#include "pch.h"
#include "framework.h"
#include "resource.h"
#include "KappaBar_i.h"


using namespace ATL;


class CKappaBarModule : public ATL::CAtlExeModuleT< CKappaBarModule >
{
public :
	DECLARE_LIBID(LIBID_KappaBarLib)
	DECLARE_REGISTRY_APPID_RESOURCEID(IDR_KAPPABAR, "{5a79b3c9-dd2b-453d-9a24-e33d82c56570}")
};

CKappaBarModule _AtlModule;



//
extern "C" int WINAPI _tWinMain(HINSTANCE /*hInstance*/, HINSTANCE /*hPrevInstance*/,
								LPTSTR /*lpCmdLine*/, int nShowCmd)
{
	return _AtlModule.WinMain(nShowCmd);
}

