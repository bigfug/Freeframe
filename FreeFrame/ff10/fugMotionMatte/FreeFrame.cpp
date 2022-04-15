///////////////////////////////////////////////////////////////////////////////////////////
//
// includes 
//

#include "FreeFrame.h"
#include "FreeFramePlugin.h"
#include "FreeFrameInstance.h"

FreeFramePlugin			*ffPlugin = FreeFramePluginFactory::getInstance();

#ifdef WIN32

HANDLE					 ffModule;

///////////////////////////////////////////////////////////////////////////////////////////
// Windows DLL Entry point
//
// notes: we may want to capture hModule as the instance of the host...

extern "C" BOOL APIENTRY DllMain( HANDLE hModule, DWORD fdwReason, LPVOID lpReserved )
{
	switch( fdwReason ) 
	{ 
		case DLL_PROCESS_ATTACH:
			//ffPlugin = FreeFramePluginFactory::getInstance();
			ffModule = hModule;
			break;

		case DLL_THREAD_ATTACH:
		 // Do thread-specific initialization.
			break;

		case DLL_THREAD_DETACH:
		 // Do thread-specific cleanup.
			break;

		case DLL_PROCESS_DETACH:
			//SAFE_DELETE( ffPlugin );
			break;
	}

	return( TRUE );
}

#endif // WIN32

///////////////////////////////////////////////////////////////////////////////////////
// plugMain - The one and only exposed function
// parameters: 
//	functionCode - tells the plugin which function is being called
//  pParam - 32-bit parameter or 32-bit pointer to parameter structure
//
// PLUGIN DEVELOPERS:  you shouldn't need to change this function
//
// All parameters are cast as 32-bit untyped pointers and cast to appropriate
// types here
// 
// All return values are cast to 32-bit untyped pointers here before return to 
// the host
//

#ifdef WIN32
extern "C" __declspec(dllexport) plugMainUnion __stdcall plugMain( DWORD functionCode, plugMainUnion pParam, LPVOID instanceID ) 
#elif LINUX
extern "C" {
plugMainUnion plugMain( DWORD functionCode, plugMainUnion pParam, LPVOID instanceID )
#elif TARGET_OS_MAC
extern "C" plugMainUnion plugMain( DWORD functionCode, plugMainUnion pParam, DWORD instanceID )
#endif	
{
	FreeFrameInstance	*ffInstance = static_cast<FreeFrameInstance *>( instanceID );
	plugMainUnion		 retval;
	ParamTypes			 ReturnTypes;

	if( ffPlugin == 0 )
	{
		retval.ivalue = FF_FAIL;

		return( retval );
	}

	switch( functionCode )
	{
		case FF_GETINFO:
			retval.pvalue = ffPlugin->getInfo();
			break;
		case FF_INITIALISE:
			retval.ivalue = ffPlugin->initialise();
			break;
		case FF_DEINITIALISE:
			retval.ivalue = ffPlugin->deInitialise();			// todo: pass on instance IDs etc
			break;
		case FF_GETNUMPARAMETERS:
			retval.ivalue = ffPlugin->getNumParameters();
			break;
		case FF_GETPARAMETERNAME:
			retval.svalue = (char *)ffPlugin->getParameterName( pParam.ivalue );
			break;
		case FF_GETPARAMETERDEFAULT:
			ReturnTypes = ffPlugin->getParameterDefault( pParam.ivalue );
			retval.fvalue = ReturnTypes.fValue;
			break;
		case FF_GETPARAMETERDISPLAY:
			retval.svalue = ffInstance->getParameterDisplay( pParam.ivalue );
			break;	
		case FF_SETPARAMETER:
			retval.ivalue = ffInstance->setParameter( static_cast<SetParameterStruct *>( pParam.pvalue ) );
			break;
		case FF_PROCESSFRAME:
			VideoFrame		Frame;

			Frame.Frame = pParam.pvalue;

			retval.ivalue = ffInstance->processFrame( Frame );
			break;
		case FF_GETPARAMETER:
			ReturnTypes = ffInstance->getParameter( pParam.ivalue );
			retval.fvalue = ReturnTypes.fValue;
			break;
		case FF_GETPLUGINCAPS:
			retval.ivalue = ffPlugin->getPluginCaps( pParam.ivalue );
			break;
		case FF_INSTANTIATE:
			retval.pvalue = ffPlugin->instantiate( static_cast<VideoInfoStruct *>( pParam.pvalue ) );
			break;
		case FF_DEINSTANTIATE:
			retval.ivalue = ffPlugin->deInstantiate( ffInstance );
			break;
		case FF_GETEXTENDEDINFO: 
			retval.pvalue = ffPlugin->getExtendedInfo();
			break;
		case FF_PROCESSFRAMECOPY:
			retval.ivalue = ffInstance->processFrameCopy( static_cast<ProcessFrameCopyStruct *>( pParam.pvalue ) );
			break;
		case FF_GETPARAMETERTYPE:		
			retval.ivalue = ffPlugin->getParameterType( pParam.ivalue );
			break;
		case FF_GETINPUTSTATUS:
			retval.ivalue = FF_INPUT_IN_USE;
			break;

		default:
			retval.ivalue = FF_FAIL;
			break;
	}

	return retval;
}

#ifdef LINUX
} /* extern "C" */
#endif
