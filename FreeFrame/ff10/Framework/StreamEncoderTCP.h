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

#include <winsock2.h>
#include <stdio.h>

#include "FreeFrame.h"
#include "FrameBuffer.h"
#include "FrameConvert.h"
#include "StreamEncoder.h"
#include "zlib.h"

enum PortState
{
	PORT_NOT_CONNECTED,
	PORT_CONNECTING,
	PORT_CONNECTED
};

class StreamEncoderTCP : public StreamEncoder
{
protected:
	SOCKET			mSocket;
	PortState		mPortStatus;
public:
	StreamEncoderTCP( const VideoInfoStruct &pSrcInfo );
	virtual ~StreamEncoderTCP( void );
protected:
	virtual void allocBuffers( void );
	virtual void deallocBuffers( void );

	virtual void portOpen( void );
	virtual void portClose( void );
	virtual bool portConnect( void );
	virtual bool portWrite( const BYTE *pBuffer );
};
