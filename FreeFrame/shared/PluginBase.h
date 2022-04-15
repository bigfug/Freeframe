
#pragma once

#if defined( FF_VERSION_10 )
#include <FreeFrame.h>
#else
#include <FFGL.h>
#endif

#if FF_VERSION >= 16 || ( defined( FF_VERSION_10 ) && defined( IS_64BIT ) )
typedef FFMixed plugMainUnion;
#define ivalue UIntValue
#define svalue PointerValue
#define fvalue FloatValue
#define PISvalue PointerValue
#define VISvalue PointerValue
#endif

#include <algorithm>
#include <vector>
#include <string>

using namespace std;

#define FF_TYPE_INTEGER			(1000)
#define FF_TYPE_ONOFF			(1001)
#define FF_TYPE_CHOICE			(1002)

typedef enum FreeFrameApiVersion
{
	VERSION_10,
	VERSION_15
} FreeFrameApiVersion;

typedef struct Param
{
	unsigned		  mIndex;
	unsigned		  mType;
	const char		 *mName;
	const char		 *mDefaultString;
	float			  mDefaultFloat;
	float			  mMin;
	float			  mMax;
	const char		**mStrings;

	Param( unsigned pIndex, unsigned pType, const char *pName, const char *pDefaultString, float pDefaultFloat, float pMin, float pMax )
		: mIndex( pIndex ), mType( pType ), mName( pName ), mDefaultString( pDefaultString ), mDefaultFloat( pDefaultFloat ), mMin( pMin ), mMax( pMax ), mStrings( 0 )
	{

	}

	Param( unsigned pIndex, const char *pName, float pDefaultFloat, float pMin = 0.0f, float pMax = 1.0f )
		: mIndex( pIndex ), mType( FF_TYPE_STANDARD ), mName( pName ), mDefaultString( 0 ), mDefaultFloat( pDefaultFloat ), mMin( pMin ), mMax( pMax ), mStrings( 0 )
	{

	}

	Param( unsigned pIndex, const char *pName, int pDefaultInt, int pMin = 0, int pMax = 1 )
		: mIndex( pIndex ), mType( FF_TYPE_INTEGER ), mName( pName ), mDefaultString( 0 ), mDefaultFloat( float( pDefaultInt ) ), mMin( float( pMin ) ), mMax( float( pMax ) ), mStrings( 0 )
	{

	}

	Param( unsigned pIndex, const char *pName, const char *pDefaultString )
		: mIndex( pIndex ), mType( FF_TYPE_TEXT ), mName( pName ), mDefaultString( pDefaultString ), mDefaultFloat( 0.0f ), mMin( 0.0f ), mMax( 0.0f ), mStrings( 0 )
	{

	}

	Param( unsigned pIndex, const char *pName, bool pDefaultValue )
		: mIndex( pIndex ), mType( FF_TYPE_BOOLEAN ), mName( pName ), mDefaultString( 0 ), mDefaultFloat( pDefaultValue ? 1.0f : 0.0f ), mMin( 0.0f ), mMax( 1.0f ), mStrings( 0 )
	{

	}

	Param( unsigned pIndex, const char *pName, int pDefaultChoice, const char *pStrings[], int pChoiceCount )
		: mIndex( pIndex ), mType( FF_TYPE_CHOICE ), mName( pName ), mDefaultString( 0 ), mDefaultFloat( float( pDefaultChoice ) ), mMin( 0.0f ), mMax( float( pChoiceCount ) ), mStrings( pStrings )
	{
	}

	static Param onoff( unsigned pIndex, const char *pName, bool pOn )
	{
		Param		P( pIndex, FF_TYPE_ONOFF, pName, 0, pOn ? 1.0f : 0.0f, 0.0f, 1.0f );

		return( P );
	}

	float getDefaultFloat( void ) const
	{
		const float		Default = std::min<float>( max<float>( mDefaultFloat, mMin ), mMax );
		const float		Range   = mMax - mMin;

		if( Range == 0.0f )
		{
			return( 0.0f );
		}

		return( ( Default - mMin ) / Range );
	}

} Param;

class Plugin;

class PluginBase
{
protected:
	PluginInfoStruct				 mPluginInfo;
	PluginExtendedInfoStruct		 mPluginExtendedInfo;

	static const char				*mPluginUniqueId;
	static const char				*mPluginName;
	static const char				*mPluginDescription;
	static const char				*mPluginAbout;

	static const unsigned			 mPluginMajorVersion;
	static const unsigned			 mPluginMinorVersion;

	static const unsigned			 mPluginType;

	static const Param				 mParams[];
	static const unsigned			 mParamCount;
	static const unsigned			 mMinimumInputFrames;
	static const unsigned			 mMaximumInputFrames;

#if defined( FF_VERSION_10 )
	static const unsigned			 m16bit;
	static const unsigned			 m24bit;
	static const unsigned			 m32bit;
	static const unsigned			 mProcessFrameCopy;
	static const unsigned			 mCopyOrInPlace;
#endif

	PluginBase( void );

	virtual ~PluginBase( void );

	virtual void getInfo( FFMixed &pResult );
	virtual void getExtendedInfo( plugMainUnion &pResult );

	virtual void getPluginCaps( plugMainUnion &pResult );

	virtual void initialise( plugMainUnion &pResult );
	virtual void deinitialise( plugMainUnion &pResult );

#if defined( FF_VERSION_10 )
	virtual void instantiate( plugMainUnion &pResult );
	virtual void deInstantiate( plugMainUnion &pResult );
#endif

#if FF_VERSION >= 15
	virtual void instantiateGL( plugMainUnion &pResult );
	virtual void deInstantiateGL( plugMainUnion &pResult );
#endif

	virtual void getNumParameters( plugMainUnion &pResult );
	virtual void getParameterName( plugMainUnion &pResult );
	virtual void getParameterDefault( plugMainUnion &pResult );
	virtual void getParameterType( plugMainUnion &pResult );

public:
#if FF_VERSION >= 16 || defined( FF_VERSION_10_64 )
	static FFMixed entry( FFUInt32 functionCode, FFMixed inputValue, FFInstanceID instanceID );
#else
	static plugMainUnion entry( DWORD pFunctionCode, plugMainUnion pParam, LPVOID pInstanceID );
#endif

	static Plugin &getInstance( void );

	virtual const Param *getParameters( void ) const
	{
		return( mParams );
	}

	virtual size_t getParameterCount( void ) const
	{
		return( mParamCount );
	}

	virtual char *getParameterTypeFormat( unsigned pParamType ) const;
};
