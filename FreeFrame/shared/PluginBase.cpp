
#ifdef WIN32
#include <crtdbg.h>
#endif

#include "PluginBase.h"
#include <cassert>
#include "Plugin.h"
#include "InstanceBase.h"

Plugin &PluginBase::getInstance( void )
{
	static Plugin			PluginSingleton;

	return( PluginSingleton );
}

#if FF_VERSION >= 16
FFMixed PluginBase::entry( FFUInt32 pFunctionCode, FFMixed pParam, FFInstanceID pInstanceID )
#else
plugMainUnion PluginBase::entry( DWORD pFunctionCode, plugMainUnion pParam, LPVOID pInstanceID )
#endif
{
	Plugin				&mPlugin  = getInstance();
	InstanceBase		*Instance = reinterpret_cast<InstanceBase *>( pInstanceID );

	switch( pFunctionCode )
	{
		case FF_GETINFO:
			{
				mPlugin.getInfo( pParam );
			}
			break;

		case FF_GETEXTENDEDINFO:
			{
				mPlugin.getExtendedInfo( pParam );
			}
			break;

		case FF_GETPLUGINCAPS:
			{
				mPlugin.getPluginCaps( pParam );
			}
			break;

		case FF_INITIALISE:
			{
				mPlugin.initialise( pParam );
			}
			break;

		case FF_DEINITIALISE:
			{
				mPlugin.deinitialise( pParam );
			}
			break;

		case FF_GETINPUTSTATUS:
			{
				if( Instance != 0 )
				{
					Instance->getInputStatus( pParam );
				}
				else
				{
					pParam.ivalue = FF_FAIL;

				}
			}
			break;

		case FF_GETNUMPARAMETERS:
			{
				mPlugin.getNumParameters( pParam );
			}
			break;

		case FF_GETPARAMETERNAME:
			{
				mPlugin.getParameterName( pParam );
			}
			break;

		case FF_GETPARAMETERDEFAULT:
			{
				mPlugin.getParameterDefault( pParam );
			}
			break;

		case FF_GETPARAMETERTYPE:
			{
				mPlugin.getParameterType( pParam );
			}
			break;

		case FF_GETPARAMETERDISPLAY:
			{
				if( Instance != 0 )
				{
					Instance->getParameterDisplay( pParam );
				}
				else
				{
					pParam.ivalue = FF_FAIL;
				}
			}
			break;

		case FF_GETPARAMETER:
			{
				if( Instance != 0 )
				{
					Instance->getParameter( pParam );
				}
				else
				{
					pParam.ivalue = FF_FAIL;
				}
			}
			break;

		case FF_SETPARAMETER:
			{
				if( Instance != 0 )
				{
					Instance->setParameter( pParam );
				}
				else
				{
					pParam.ivalue = FF_FAIL;
				}
			}
			break;

#if defined( FF_VERSION_10 )
		case FF_INSTANTIATE:
			{
				mPlugin.instantiate( pParam );
			}
			break;

		case FF_DEINSTANTIATE:
			{
#if defined( FF_VERSION_10_64 )
				pParam.PointerValue = Instance;
#else
				pParam.ivalue = reinterpret_cast<DWORD>( Instance );
#endif

				mPlugin.deInstantiate( pParam );
			}
			break;

		case FF_PROCESSFRAME:
			{
				if( Instance != 0 )
				{
					Instance->processFrame( pParam );
				}
				else
				{
					pParam.ivalue = FF_FAIL;
				}
			}
			break;

		case FF_PROCESSFRAMECOPY:
			{
				if( Instance != 0 )
				{
					Instance->processFrameCopy( pParam );
				}
				else
				{
					pParam.ivalue = FF_FAIL;
				}
			}
			break;
#endif

#if FF_VERSION >= 15
		case FF_PROCESSOPENGL:
			{
				if( Instance != 0 )
				{
					Instance->processOpengl( pParam );
				}
				else
				{
					pParam.ivalue = FF_FAIL;
				}
			}
			break;

		case FF_INSTANTIATEGL:
			{
				mPlugin.instantiateGL( pParam );
			}
			break;

		case FF_DEINSTANTIATEGL:
			{
#if FF_VERSION >= 16
				pParam.PointerValue = Instance;
#else
				pParam.ivalue = reinterpret_cast<DWORD>( Instance );
#endif

				mPlugin.deInstantiateGL( pParam );
			}
			break;

		case FF_SETTIME:
			{
				if( Instance != 0 )
				{
					Instance->setTime( pParam );
				}
				else
				{
					pParam.ivalue = FF_FAIL;
				}
			}
			break;
#endif

		default:
			{
				pParam.ivalue = FF_FAIL;
			}
			break;
	}

	return( pParam );
}

PluginBase::PluginBase( void )
{
	memset( &mPluginInfo, 0, sizeof( mPluginInfo ) );

#if defined( FF_VERSION_10 )
	mPluginInfo.APIMajorVersion = 1;
	mPluginInfo.APIMinorVersion = 0;
#endif

#if defined( FF_VERSION_15 ) || defined( FF_VERSION_16 )
	mPluginInfo.APIMajorVersion = 1;
	mPluginInfo.APIMinorVersion = 500;
#endif

#if defined( FF_VERSION_20 )
	mPluginInfo.APIMajorVersion = 2;
	mPluginInfo.APIMinorVersion = 0;
#endif

	assert( strlen( mPluginUniqueId ) == 4 );

	memcpy( &mPluginInfo.PluginUniqueID, mPluginUniqueId, 4 );

	const size_t	CopyCount = min( sizeof( mPluginInfo.PluginName ), strlen( mPluginName ) );

	memcpy( &mPluginInfo.PluginName, mPluginName, CopyCount );

	mPluginInfo.PluginType = mPluginType;

	// Set-up mPluginExtendedInfo

	memset( &mPluginExtendedInfo, 0, sizeof( mPluginExtendedInfo ) );

	mPluginExtendedInfo.PluginMajorVersion = mPluginMajorVersion;
	mPluginExtendedInfo.PluginMinorVersion = mPluginMinorVersion;
	mPluginExtendedInfo.Description        = (char *)mPluginDescription;
	mPluginExtendedInfo.About              = (char *)mPluginAbout;
}

PluginBase::~PluginBase( void )
{
}

void PluginBase::getInfo( FFMixed &pResult )
{
	assert( mPluginInfo.PluginUniqueID[ 0 ] != 0x00 );
	assert( mPluginInfo.PluginUniqueID[ 1 ] != 0x00 );
	assert( mPluginInfo.PluginUniqueID[ 2 ] != 0x00 );
	assert( mPluginInfo.PluginUniqueID[ 3 ] != 0x00 );

	assert( mPluginInfo.PluginName[ 0 ] != 0x00 );

	assert( mPluginInfo.PluginType == FF_EFFECT || mPluginInfo.PluginType == FF_SOURCE );

	pResult.PISvalue = &mPluginInfo;
}

void PluginBase::getExtendedInfo( plugMainUnion &pResult )
{
#if FF_VERSION >= 16
	pResult.PointerValue = &mPluginExtendedInfo;
#else
	pResult.ivalue = reinterpret_cast<DWORD>( &mPluginExtendedInfo );
#endif
}

void PluginBase::getPluginCaps( plugMainUnion &pResult )
{
	switch( pResult.ivalue )
	{
#if defined( FF_VERSION_10 )
		case FF_CAP_16BITVIDEO:
			pResult.ivalue = m16bit;
			break;

		case FF_CAP_24BITVIDEO:
			pResult.ivalue = m24bit;
			break;

		case FF_CAP_32BITVIDEO:
			pResult.ivalue = m32bit;
			break;

		case FF_CAP_PROCESSFRAMECOPY:
			pResult.ivalue = mProcessFrameCopy;
			break;

		case FF_CAP_COPYORINPLACE:
			pResult.ivalue = mCopyOrInPlace;
			break;
#endif

#if FF_VERSION >= 15
		case FF_CAP_PROCESSOPENGL:
			pResult.ivalue = FF_SUPPORTED;
			break;

		case FF_CAP_SETTIME:
			pResult.ivalue = FF_SUPPORTED;
			break;
#endif

		case FF_CAP_MINIMUMINPUTFRAMES:
			pResult.ivalue = mMinimumInputFrames;
			break;

		case FF_CAP_MAXIMUMINPUTFRAMES:
			pResult.ivalue = mMaximumInputFrames;
			break;

		default:
			pResult.ivalue = FF_UNSUPPORTED;
			break;
	}
}

void PluginBase::initialise( plugMainUnion &pResult )
{
	pResult.ivalue = FF_SUCCESS;
}

void PluginBase::deinitialise( plugMainUnion &pResult )
{
	pResult.ivalue = FF_SUCCESS;
}

#if defined( FF_VERSION_10 )

void PluginBase::instantiate( plugMainUnion &pResult )
{
	pResult.ivalue = FF_FAIL;
}

void PluginBase::deInstantiate( plugMainUnion &pResult )
{
	pResult.ivalue = FF_FAIL;
}

#endif

#if FF_VERSION >= 15

void PluginBase::instantiateGL( plugMainUnion &pResult )
{
	pResult.ivalue = FF_FAIL;
}

void PluginBase::deInstantiateGL( plugMainUnion &pResult )
{
	pResult.ivalue = FF_FAIL;
}

#endif

void PluginBase::getNumParameters( plugMainUnion &pResult )
{
	pResult.ivalue = mParamCount;
}

void PluginBase::getParameterName( plugMainUnion &pResult )
{
	if( pResult.ivalue < mParamCount )
	{
		pResult.svalue = (char *)mParams[ pResult.ivalue ].mName;
	}
	else
	{
		pResult.ivalue = FF_FAIL;
	}
}

void PluginBase::getParameterDefault( plugMainUnion &pResult )
{
	if( pResult.ivalue < mParamCount )
	{
		const Param		&P = mParams[ pResult.ivalue ];

		if( P.mDefaultString != 0 )
		{
			pResult.svalue = (char *)P.mDefaultString;
		}
		else
		{
			pResult.fvalue = P.getDefaultFloat();
		}
	}
	else
	{
		pResult.ivalue = FF_FAIL;
	}
}

void PluginBase::getParameterType( plugMainUnion &pResult )
{
	if( pResult.ivalue < mParamCount )
	{
		if( mParams[ pResult.ivalue ].mType == FF_TYPE_INTEGER )
		{
			pResult.ivalue = FF_TYPE_STANDARD;
		}
		else if( mParams[ pResult.ivalue ].mType == FF_TYPE_ONOFF )
		{
			pResult.ivalue = FF_TYPE_BOOLEAN;
		}
		else if( mParams[ pResult.ivalue ].mType == FF_TYPE_CHOICE )
		{
			pResult.ivalue = FF_TYPE_STANDARD;
		}
		else
		{
			pResult.ivalue = mParams[ pResult.ivalue ].mType;
		}
	}
	else
	{
		pResult.ivalue = FF_FAIL;
	}
}

char *PluginBase::getParameterTypeFormat( unsigned pParamType ) const
{
	switch( pParamType )
	{
		case FF_TYPE_INTEGER:
			return( (char *)"%d" );

		case FF_TYPE_TEXT:
			return( (char *)"%s" );

		default:
			return( (char *)"%0.2f" );
	}

	return( 0 );
}

