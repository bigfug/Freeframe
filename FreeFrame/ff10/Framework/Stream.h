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

#pragma once

typedef struct FrameDataStruct
{
	BYTE		FrameNum;
	BYTE		BitDepth;
	USHORT		Line;
	USHORT		Width;
	USHORT		Height;
	UINT		DataSize;
	BYTE		Data;
}
FrameData;

#define FRAME_DATA_SIZE		(sizeof(FrameData)-sizeof(BYTE))

enum StreamProtocol
{
	STREAM_UNKNOWN,
	STREAM_UDP,
	STREAM_TCP,
	STREAM_MEM
};

#define		LOG		StreamUtils::log
//#define		LOG		((void)0)

class StreamUtils
{
protected:
	static	CRITICAL_SECTION		mLogLock;
public:
	StreamUtils( void )
	{
		InitializeCriticalSection( &mLogLock );
	}

	~StreamUtils( void )
	{
		DeleteCriticalSection( &mLogLock );
	}

	static StreamProtocol getProtocol( const char *pName )
	{
		if( strncmp( pName, "UDP", 3 ) == 0 )
		{
			return( STREAM_UDP );
		}

		if( strncmp( pName, "TCP", 3 ) == 0 )
		{
			return( STREAM_TCP );
		}

		if( strncmp( pName, "MEM", 3 ) == 0 )
		{
			return( STREAM_MEM );
		}

		return( STREAM_UNKNOWN );
	}

	static void log( const wchar_t *pFmt, ... )
#if 1
	{
		return;
	}
#else
	{
		va_list			 argptr;
		FILE			*fp;
		int				 res;
		wchar_t			 data[ 1024 ];
		SYSTEMTIME		 systime;

		va_start( argptr, pFmt );

		vswprintf( data, 1024, pFmt, argptr );

		GetSystemTime( &systime );

		EnterCriticalSection( &mLogLock );

		if( ( res = _wfopen_s( &fp, L"C:\\fugStream.log", L"a" ) ) != 0 )
		{
			return;
		}

		if( ( res = fwprintf( fp, L"%02d:%02d:%02d - %s\n", systime.wHour, systime.wMinute, systime.wSecond, data ) ) < 0 )
		{
		}

		if( ( res = fclose( fp ) ) != 0 )
		{
		}

		LeaveCriticalSection( &mLogLock );

		va_end( argptr );
	}
#endif
};
