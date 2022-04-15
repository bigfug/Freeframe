
#include "PluginBase.h"

#if !defined(Q_UNUSED)
#define Q_UNUSED(arg) (void)arg;
#endif

#ifdef WIN32

FILE _iob[] = { *stdin, *stdout, *stderr };
extern "C" FILE * __cdecl __iob_func(void) { return _iob; }

#include <crtdbg.h>

HANDLE		DLLmodule;
WCHAR		DLLname[ 512 ];

BOOL APIENTRY DllMain( HANDLE hModule, DWORD fdwReason, LPVOID lpReserved )
{
	Q_UNUSED( lpReserved )

#if 0
	if( hModule != 0 )
	{
		DLLmodule = hModule;

		GetModuleFileName( (HMODULE)DLLmodule, DLLname, sizeof( DLLname ) );
	}
#endif

	switch( fdwReason )
	{
		case DLL_PROCESS_ATTACH:
			break;

		case DLL_THREAD_ATTACH:
			break;

		case DLL_THREAD_DETACH:
			break;

		case DLL_PROCESS_DETACH:
			break;
	}

	return( TRUE );
}

#endif

#ifdef WIN32
extern "C"
{
#if defined( FF_VERSION_16 )
	__declspec(dllexport) FFMixed __stdcall plugMain( FFUInt32 functionCode, FFMixed inputValue, FFInstanceID instanceID )
#else
	//	__declspec(dllexport) plugMainUnion __stdcall plugMain( DWORD functionCode, plugMainUnion pParam, LPVOID instanceID )
#endif
	{
		return( PluginBase::entry( functionCode, inputValue, instanceID ) );
	}
}
#elif LINUX
extern "C"
{
	plugMainUnion plugMain( DWORD functionCode, plugMainUnion pParam, LPVOID instanceID )
	{
		return( plugEntry( functionCode, pParam, instanceID );
	}
}
#elif __linux__
extern "C"
{
	plugMainUnion plugMain( DWORD functionCode, plugMainUnion pParam, LPVOID instanceID )
	{
		return( plugEntry( functionCode, pParam, instanceID );
	}
}
#elif TARGET_OS_MAC
extern "C"
{
#if FF_VERSION >= 16 || defined( FF_VERSION_10_64 )
	FFMixed plugMain( FFUInt32 functionCode, FFMixed pParam, FFInstanceID instanceID )
#else
	plugMainUnion plugMain( DWORD functionCode, plugMainUnion pParam, LPVOID instanceID )
#endif
	{
		return( PluginBase::entry( functionCode, pParam, (void *)instanceID ) );
	}
}
#endif
