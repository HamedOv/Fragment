// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "stdafx.h"
#include "TVPHook.h"
#include "AutoUpdCore.h"
#pragma comment(lib, "AutoUpdCore.lib")
TVPHook* objTVPhook = nullptr;
bool InitPatch()
{
	objTVPhook = GetobjTVPhook();
	return objTVPhook->init();
}

bool unInitPatch()
{
	if (objTVPhook!=nullptr)
	{
		objTVPhook->uninit();
	}
	return true;
}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
		CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)VersionCheck, NULL, 0, 0);
		InitPatch();
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

extern "C" __declspec(dllexport) void dummy(void) {
	return;
}

