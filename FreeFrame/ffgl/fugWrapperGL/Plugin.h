
#pragma once

#include <PluginBase.h>

#include <list>

enum
{
	NUM_PARAMS
};

class Instance;

class Plugin : public PluginBase
{
protected:
	static const char	*m24bitPlugins[];
	static const char	*mForceAlphaPlugins[];

#ifdef WIN32
	HMODULE				 mLibrary;
#endif

public:
	FF_Main_FuncPtr		 mPlugMain;
	bool				 mCap16bit;
	bool				 mCap24bit;
	bool				 mCap32bit;
	bool				 mCapProcessFrameCopy;
	bool				 mForceAlpha;
	DWORD				 mCapMinInputFrames;
	DWORD				 mCapMaxInputFrames;
	DWORD				 mCapCopyOrInPlace;
	string				 mLibName;
	list<Instance *>	 mInstances;

public:
	Plugin( void );

	virtual ~Plugin( void );

	virtual void getPluginCaps( plugMainUnion &pResult );

	virtual void initialise( plugMainUnion &pResult );
	virtual void deinitialise( plugMainUnion &pResult );

	virtual void instantiate( plugMainUnion &pResult );
	virtual void deInstantiate( plugMainUnion &pResult );

	virtual void getNumParameters( plugMainUnion &pResult );
	virtual void getParameterName( plugMainUnion &pResult );
	virtual void getParameterDefault( plugMainUnion &pResult );
	virtual void getParameterType( plugMainUnion &pResult );

	inline string getLibName( void ) const
	{
		return( mLibName );
	}
};
