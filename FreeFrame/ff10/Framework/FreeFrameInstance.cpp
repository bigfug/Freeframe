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

#include <stdio.h>
#include <malloc.h>
#include "FreeFramePlugin.h"
#include "FreeFrameInstance.h"

FreeFrameInstance::FreeFrameInstance( FreeFramePlugin *Plugin, VideoInfoStruct *VideoInfo )
{
	this->Plugin     = Plugin;
	this->videoInfo  = *VideoInfo;
	this->paramDynamicData = 0L;

	if( this->Plugin->ParamCount > 0 )
	{
		this->paramDynamicData = new ParamDynamicDataStructTag[ this->Plugin->ParamCount ];

		for( DWORD i = 0 ; i < this->Plugin->ParamCount ; i++ )
		{
			this->paramDynamicData[ i ].value = Plugin->paramConstants[ i ].value;
		}
	}
}

FreeFrameInstance::~FreeFrameInstance()
{
	if( Plugin->ParamCount > 0 && this->paramDynamicData != NULL )
	{
		delete [] this->paramDynamicData;
	}
}

char *FreeFrameInstance::getParameterDisplay(DWORD index)
{
	if( index < Plugin->ParamCount )
	{
		// fill the array with spaces first

		memset( paramDynamicData[ index ].displayValue, ' ', 16 );

		switch( Plugin->paramConstants[ index ].type )
		{
			case FF_TYPE_BOOLEAN:
			case FF_TYPE_EVENT:
				sprintf_s( this->paramDynamicData[ index ].displayValue, "%s", this->paramDynamicData[ index ].value.fValue > 0.0f ? "On" : "Off" );
				break;

			case FF_TYPE_RED:
			case FF_TYPE_GREEN:
			case FF_TYPE_BLUE:
				sprintf_s( this->paramDynamicData[ index ].displayValue, "%d", (int)( this->paramDynamicData[ index ].value.fValue * 255.0f ) );
				break;

			case FF_TYPE_XPOS:
				sprintf_s( this->paramDynamicData[ index ].displayValue, "%d", (int)( this->paramDynamicData[ index ].value.fValue * this->videoInfo.frameWidth ) );
				break;

			case FF_TYPE_YPOS:
				sprintf_s( this->paramDynamicData[ index ].displayValue, "%d", (int)( this->paramDynamicData[ index ].value.fValue * this->videoInfo.frameHeight ) );
				break;

			case FF_TYPE_TEXT:
				return( paramDynamicData[ index ].value.pValue );
				break;

			case FF_TYPE_STANDARD:
				sprintf_s( paramDynamicData[index].displayValue, "%.f", Plugin->paramConstants[ index ].minValue + ( paramDynamicData[ index ].value.fValue * ( Plugin->paramConstants[ index ].maxValue - Plugin->paramConstants[ index ].minValue ) ) );
				break;

			default:
				sprintf_s( paramDynamicData[index].displayValue, "%.f", paramDynamicData[index].value.fValue );
				break;
		}

		return( paramDynamicData[ index ].displayValue );
	}

	return( "" );
}

DWORD FreeFrameInstance::setParameter( SetParameterStruct* pParam )
{
	if( pParam->index < Plugin->ParamCount )
	{
		switch( Plugin->paramConstants[ pParam->index ].type )
		{
			case FF_TYPE_TEXT:
				paramDynamicData[ pParam->index ].value.pValue = pParam->value.pValue;
				break;

			default:
				paramDynamicData[ pParam->index ].value.fValue = pParam->value.fValue;
				break;
		}

		return( FF_SUCCESS );
	}

	return( FF_FAIL );
}

ParamTypes FreeFrameInstance::getParameter(DWORD index)
{
	ParamTypes		ret;

	if( index < Plugin->ParamCount )
	{
		return( paramDynamicData[ index ].value );
	}

	ret.fValue = 0.0f;

	return( ret );
}

DWORD FreeFrameInstance::processFrame( VideoFrame pFrame )
{
/*
	ProcessFrameCopyStruct		 FrameCopy;
	VideoFrame					 Input[ 1 ];

	if( this->videoInfo.bitDepth == FF_CAP_32BITVIDEO )
	{
		Input[ 0 ] = pFrame;

		FrameCopy.numInputFrames = 1;
		FrameCopy.InputFrames    = Input;
		FrameCopy.OutputFrame    = pFrame;

		return( this->processFrameCopy( &FrameCopy ) );
	}

	if( this->videoInfo.bitDepth == FF_CAP_24BITVIDEO )
	{
	}

	if( this->videoInfo.bitDepth == FF_CAP_16BITVIDEO )
	{
	}
*/
	return( FF_FAIL );
}

DWORD FreeFrameInstance::processFrameCopy( ProcessFrameCopyStruct *pFrameData )
{
	return( FF_FAIL );
}

int FreeFrameInstance::getParamInt( DWORD Index )
{
	//char		Buff[256];
	float		Range;
	int			Return;

	if( Index >= Plugin->ParamCount )
	{
		return( 0 );
	}

	switch( Plugin->paramConstants[ Index ].type )
	{
		case FF_TYPE_RED:
		case FF_TYPE_GREEN:
		case FF_TYPE_BLUE:
		case FF_TYPE_XPOS:
		case FF_TYPE_YPOS:
		case FF_TYPE_STANDARD:
			Range = Plugin->paramConstants[ Index ].maxValue - Plugin->paramConstants[ Index ].minValue;

			Return = (int)( ( this->paramDynamicData[ Index ].value.fValue * Range ) + Plugin->paramConstants[ Index ].minValue );

			//sprintf_s( Buff, "Min: %f Max: %f Range: %f Return: %d", Plugin->paramConstants[ Index ].minValue, Plugin->paramConstants[ Index ].maxValue, Range, Return );

			//MessageBox( NULL, Buff, "Debug", MB_OK );

			return( Return );
			break;
	}

	return( 0 );
}

float FreeFrameInstance::getParamFloat( DWORD Index )
{
	float		Range;

	if( Index >= Plugin->ParamCount )
	{
		return( 0 );
	}

	switch( Plugin->paramConstants[ Index ].type )
	{
		case FF_TYPE_STANDARD:
			Range = Plugin->paramConstants[ Index ].maxValue - Plugin->paramConstants[ Index ].minValue;

			return( ( this->paramDynamicData[ Index ].value.fValue * Range ) + Plugin->paramConstants[ Index ].minValue );
			break;
	}

	return( 0.0f );
}

char *FreeFrameInstance::getParamString( DWORD Index )
{
	if( Index >= Plugin->ParamCount || Plugin->paramConstants[ Index ].type != FF_TYPE_TEXT )
	{
		return( "" );
	}

	return( this->paramDynamicData[ Index ].value.pValue );
}

bool FreeFrameInstance::getParamBool( DWORD Index )
{
	if( Index >= Plugin->ParamCount )
	{
		return( false );
	}

	switch( Plugin->paramConstants[ Index ].type )
	{
		case FF_TYPE_BOOLEAN:
		case FF_TYPE_EVENT:
			return( this->paramDynamicData[ Index ].value.fValue > 0.0f ? true : false );
			break;
	}

	return( false );
}