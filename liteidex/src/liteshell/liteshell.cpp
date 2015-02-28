// liteshell.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"

#include <windows.h>
#include <shlobj.h>

BOOL APIENTRY DllMain( HANDLE hModule, 
					  DWORD  ul_reason_for_call, 
					  LPVOID lpReserved
					  )
{
	if (ul_reason_for_call == DLL_PROCESS_ATTACH) {
		CoInitialize(NULL);
	}
    return TRUE;
}

extern "C" 
_declspec(dllexport) BOOL BrowseToFile(const wchar_t* filename)
{
	ITEMIDLIST *pidl = ILCreateFromPath(filename);
    if(pidl) {
        SHOpenFolderAndSelectItems(pidl,0,0,0);
        ILFree(pidl);
		return TRUE;
    }
	return FALSE;
}

