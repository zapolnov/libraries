//-----------------------------------------------------------------------------
//
// ImageLib Utility Sources
// Copyright (C) 2000-2002 by Denton Woods
// Last modified: 05/20/2001 <--Y2K Compliant! =]
//
// Filename: src-ILU/src/ilu_main.c
//
// Description: Startup functions
//
//-----------------------------------------------------------------------------


#include "ilu_internal.h"
#include "ilu_states.h"
#ifdef _WIN32
	#define WIN32_LEAN_AND_MEAN
	#include <windows.h>
#endif//_WIN32

#ifdef _WIN32
	#if (defined(IL_USE_PRAGMA_LIBS))
		#if defined(_MSC_VER) || defined(__BORLANDC__)
			#pragma comment(lib, "DevIL.lib")
		#endif
	#endif
#endif

/* Only needed for MSVC++ unless extended to actually do something =) */
#if defined(_WIN32) && defined(_MSC_VER)

#if 0
#ifndef IL_STATIC_LIB
BOOL APIENTRY DllMain(HANDLE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
	hModule;  ul_reason_for_call;  lpReserved;
	
	if (ul_reason_for_call == DLL_PROCESS_ATTACH) {
		//iluInit();
	}

	return TRUE;
}

#endif
#endif
#endif

void ILAPIENTRY ilu2Init()
{
	// Used mostly for rotations
	//IL_PI = 4 * atan(1);		// precomputed value of pi
	//IL_DEGCONV = IL_PI / 180;	// division is slow on some computers

	ilu2SetLanguage(ILU_ENGLISH);
	return;
}


//#ifndef _WIN32_WCE
ILAPI ILimage* ILAPIENTRY ilu2LoadImage(ILconst_string FileName)
{
	ILimage* image = il2GenImage();
	if (image == 0)
		return 0;
	if (!il2LoadImage(image, FileName)) {
		il2DeleteImage(image);
		return 0;
	}
	return image;
}
//#endif//_WIN32_WCE
