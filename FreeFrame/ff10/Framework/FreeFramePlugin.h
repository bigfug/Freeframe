/*

The MIT License

Copyright (c) 2007 Alex May - www.bigfug.com

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.

*/

#ifndef __FF_PLUGIN_H__
#define __FF_PLUGIN_H__

#include "FreeFrame.h"
#include "FreeFrameInstance.h"

#ifndef SAFE_DELETE
#define SAFE_DELETE(p)  { if(p) { delete (p);     (p)=NULL; } }
#define SAFE_RELEASE(p) { if(p) { (p)->Release(); (p)=NULL; } }
#endif

typedef struct ParamConstsStructTag
{
	char		name[16];
	DWORD		type;
	ParamTypes	value;
	float		minValue, maxValue;
} ParamConstantsStruct;

class FreeFramePlugin
{
protected:
	PlugInfoStruct			 plugInfo;
	PlugExtendedInfoStruct	 plugExtInfo;
public:
	const ParamConstantsStruct	*paramConstants;
	const DWORD					 ParamCount;
public:
	FreeFramePlugin( const ParamConstantsStruct *Parameters, DWORD Count );

	PlugInfoStruct					*getInfo();
	DWORD							 getNumParameters();
	const char						*getParameterName( DWORD index );
	ParamTypes						 getParameterDefault( DWORD index );
	DWORD							 getParameterType( DWORD index );
	FreeFrameInstance				*instantiate( VideoInfoStruct *pVideoInfo );
	DWORD							 deInstantiate( FreeFrameInstance *Instance );	
	PlugExtendedInfoStruct			*getExtendedInfo();

	virtual							~FreeFramePlugin();
	virtual		DWORD				 initialise();
	virtual		DWORD				 deInitialise();
	virtual		DWORD				 getPluginCaps( DWORD index );
	virtual		FreeFrameInstance	*getInstance( VideoInfoStruct *VideoInfo );
};

class FreeFramePluginFactory
{
public:
	static	FreeFramePlugin		*getInstance();
};

#endif
