#ifndef _FUG_VID_STREAM_H
#define _FUG_VID_STREAM_H

// stdint fixes for ffmpeg from http://ffmpeg.org/pipermail/ffmpeg-devel/2010-May/095488.html

#ifdef __cplusplus
#ifndef __STDC_CONSTANT_MACROS
#define __STDC_CONSTANT_MACROS
#endif
#ifdef _STDINT_H
#undef _STDINT_H
#endif
#include <stdint.h>
#endif

#include <string.h>

#ifdef __cplusplus
extern "C"
{
#endif

#define FUG_VID_16BITVIDEO		0
#define FUG_VID_24BITVIDEO		1
#define FUG_VID_32BITVIDEO		2
#define FUG_VID_8BITVIDEO		3		// Luma

#define FUG_ORIEN_TOP_LEFT		(1)
#define FUG_ORIEN_BOT_LEFT		(2)

#define FUG_BASE_PORT			(10588)

enum fugStreamProtocol
{
	FUG_STREAM_UNKNOWN,
	FUG_STREAM_UDP,
	FUG_STREAM_TCP,
	FUG_STREAM_MEM
};

enum fugStreamCompression
{
	FUG_COMPRESS_NONE,
	FUG_COMPRESS_JPEG
};

#ifndef INFINITE
#define INFINITE            0xFFFFFFFF
#endif

typedef struct fugVideoInfo
{
	uint16_t	frameWidth;			// width of frame in pixels
	uint16_t	frameHeight;		// height of frame in pixels
	uint16_t	bitDepth;			// enumerated indicator of bit depth of video
	uint16_t	orientation;		// 0 = 16 bit 5-6-5   1 = 24bit packed   2 = 32bit
} fugVideoInfo;

typedef void *HVIDSTREAM;

// utility

size_t fugGetByteSize( fugVideoInfo *pVidInfo );

// For encoder
HVIDSTREAM fugGetEncoder( fugStreamProtocol pProtocol, fugVideoInfo *pSrcInfo );
void fugSetDestAddress( HVIDSTREAM pStream, const char *pAddr, const uint16_t pPort );
void fugSetDestFormat( HVIDSTREAM pStream, const fugVideoInfo *pDstInfo, const fugStreamCompression pCompression = FUG_COMPRESS_NONE, uint16_t pCompressionLevel = 85 );
void fugEncode( HVIDSTREAM pStream, void *pSrcData );
void fugDeleteEncoder( HVIDSTREAM pStream );

// For decoder
HVIDSTREAM fugGetDecoder( fugStreamProtocol pProtocol, fugVideoInfo *pDstInfo );
void fugSetSourceAddress( HVIDSTREAM pStream, const uint16_t pPort );
uint8_t * fugLockReadBuffer( HVIDSTREAM pStream, uint32_t pTimeout = INFINITE );
void fugUnlockReadBuffer( HVIDSTREAM pStream );
bool fugHasNewFrame( HVIDSTREAM pStream );
bool fugWaitForNewFrame( HVIDSTREAM pStream, uint32_t pTimeout = INFINITE );
uint16_t fugGetRecvWidth( HVIDSTREAM pStream );
uint16_t fugGetRecvHeight( HVIDSTREAM pStream );
uint8_t fugGetRecvDepth( HVIDSTREAM pStream );
uint8_t fugGetRecvOrientation( HVIDSTREAM pStream );
uint8_t fugGetFrameNumber( HVIDSTREAM pStream );
size_t fugGetCompressedFrameSize( HVIDSTREAM pStream );
void fugDeleteDecoder( HVIDSTREAM pStream );

void fugSetAdaptiveBuffer( HVIDSTREAM pStream, bool pIsAdaptive );

#ifdef __cplusplus
}
#endif

#endif // _FUG_VID_STREAM_H
