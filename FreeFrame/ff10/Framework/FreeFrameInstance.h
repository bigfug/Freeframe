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

#ifndef		__FF_INSTANCE__
#define		__FF_INSTANCE__

#include	"FreeFrame.h"

class FreeFramePlugin;

typedef struct SetParameterStructTag
{
	DWORD			index;
	ParamTypes		value;
}
SetParameterStruct;

typedef struct ParamDynamicDataStructTag
{
	char			displayValue[ 16 ];
	ParamTypes		value;
}
ParamDynamicDataStruct;

typedef struct VideoPixel24bitTag
{
#ifdef WIN32
	BYTE blue;
	BYTE green;
	BYTE red;
#else
	BYTE red;
	BYTE green;
	BYTE blue;
#endif
} VideoPixel24bit;

typedef union VideoPixel16bitTag
{
	short int	value;

	struct
	{
#ifdef WIN32
		short int	blue  : 5;
		short int	green : 6;
		short int	red   : 5;
#else
		short int	red   : 5;
		short int	green : 6;
		short int	blue  : 5;
#endif
	};
} VideoPixel16bit;

typedef unsigned char VideoPixel8bit;

typedef union VideoPixel32bitTag
{
	UINT	value;

	struct
	{
#ifdef WIN32
		BYTE blue;
		BYTE green;
		BYTE red;
		BYTE alpha;
#else
		BYTE red;
		BYTE green;
		BYTE blue;
		BYTE alpha;
#endif
	};
} VideoPixel32bit;

typedef struct VideoPixelHSLTag
{
	float	mHue;
	float	mSat;
	float	mLum;
} VideoPixelHSL;

typedef union VideoFrameTag
{
	LPVOID				 Frame;
	VideoPixel16bit		*Vid16;
	VideoPixel24bit		*Vid24;
	VideoPixel32bit		*Vid32;
} VideoFrame;

typedef struct ProcessFrameCopyStructTag
{
	DWORD			  numInputFrames;
	VideoFrame		 *InputFrames;
	VideoFrame		  OutputFrame;
} ProcessFrameCopyStruct;


class FreeFrameInstance
{
protected:
	FreeFramePlugin				*Plugin;
	ParamDynamicDataStruct		*paramDynamicData;
	VideoInfoStruct				 videoInfo;

public:
	FreeFrameInstance( FreeFramePlugin *Plugin, VideoInfoStruct *VideoInfo );
	
	DWORD				 setParameter(SetParameterStruct* pParam);		
	ParamTypes			 getParameter(DWORD index);					
	char				*getParameterDisplay(DWORD index);
	int					 getParamInt( DWORD Index );
	float				 getParamFloat( DWORD Index );
	char				*getParamString( DWORD Index );
	bool				 getParamBool( DWORD Index );

	virtual				~FreeFrameInstance();
	virtual		DWORD	 processFrame( VideoFrame pFrame );
	virtual		DWORD	 processFrameCopy( ProcessFrameCopyStruct* pFrameData );
};

#endif