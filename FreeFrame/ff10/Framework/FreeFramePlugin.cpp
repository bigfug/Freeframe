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

#include <string.h>
#include <stdio.h>
#include "FreeFramePlugin.h"

FreeFramePlugin::FreeFramePlugin( const ParamConstantsStruct *Parameters, DWORD Count )
: paramConstants( Parameters ),
  ParamCount( Count )
{
}

FreeFramePlugin::~FreeFramePlugin()
{
}

PlugInfoStruct* FreeFramePlugin::getInfo() 
{
	return( &this->plugInfo );
}

DWORD FreeFramePlugin::initialise()
{
	return( FF_SUCCESS );
}

DWORD FreeFramePlugin::deInitialise()
{
	return( FF_SUCCESS );
}

DWORD FreeFramePlugin::getNumParameters()
{
	return( this->ParamCount );  
}

const char * FreeFramePlugin::getParameterName( DWORD index )
{
	if( index < this->ParamCount )
	{
		return( this->paramConstants[ index ].name );
	}

	return( "INVALID" );
}

ParamTypes FreeFramePlugin::getParameterDefault( DWORD index )
{
	ParamTypes		ret;

	if( index < this->ParamCount )
	{
		return( this->paramConstants[ index ].value );
	}

	ret.fValue = 0.0f;

	return( ret );
}

DWORD FreeFramePlugin::getParameterType( DWORD index )
{
	if( index < this->ParamCount )
	{
		return( this->paramConstants[ index ].type );
	}
		
	return( FF_TYPE_STANDARD );
}

DWORD FreeFramePlugin::getPluginCaps( DWORD index )
{
	return( FF_UNSUPPORTED );
}

FreeFrameInstance *FreeFramePlugin::getInstance( VideoInfoStruct *VideoInfo )
{
	return( new FreeFrameInstance( this, VideoInfo ) );
}

FreeFrameInstance *FreeFramePlugin::instantiate( VideoInfoStruct *pVideoInfo )
{
	FreeFrameInstance		*ffInstance;
	DWORD					 NumParams;

	if( pVideoInfo->bitDepth > FF_CAP_32BITVIDEO || pVideoInfo->bitDepth < FF_CAP_16BITVIDEO )
	{
		return( (FreeFrameInstance *)FF_FAIL );
	}

	NumParams  = this->getNumParameters();

	ffInstance = this->getInstance( pVideoInfo );

	return( ffInstance );
}

DWORD FreeFramePlugin::deInstantiate( FreeFrameInstance * Instance )
{
	SAFE_DELETE( Instance );

	return( FF_SUCCESS );
}


PlugExtendedInfoStruct *FreeFramePlugin::getExtendedInfo()
{
	return( &this->plugExtInfo );
}