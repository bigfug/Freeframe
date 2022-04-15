
#include "AudioCapture.h"

AudioCapture::AudioCapture()
{
    HRESULT						hr;
	DSCBUFFERDESC               dscbd;
	WAVEFORMATEX                wfx = { WAVE_FORMAT_PCM, 2, 44100, CAPTURE_BUFFER_SIZE, 4, 16, 0 };
	// wFormatTag, nChannels, nSamplesPerSec, mAvgBytesPerSec,
	// nBlockAlign, wBitsPerSample, cbSize

	LastRead  = 0;
	LockCount = 0;
	DSBuffer  = NULL;

	if( FAILED( hr = DirectSoundCaptureCreate( NULL, &DSCapture, NULL ) ) )
	{
		switch( hr )
		{
			case DSERR_ALLOCATED:
				MessageBox( NULL, "The request failed because resources, such as a priority level, were already in use by another caller.", "Debug", MB_OK );
				break;
			case DSERR_INVALIDPARAM:
				MessageBox( NULL, "An invalid parameter was passed to the returning function.", "Debug", MB_OK );
				break;
			case DSERR_NOAGGREGATION:
				MessageBox( NULL, "The object does not support aggregation.", "Debug", MB_OK );
				break;
			case DSERR_OUTOFMEMORY:
				MessageBox( NULL, "The DirectSound subsystem could not allocate sufficient memory to complete the caller's request.", "Debug", MB_OK );
				break;
			default:
				MessageBox( NULL, "Unknown Error", "Debug", MB_OK );
				break;
		}

		return;
	}

	dscbd.dwSize = sizeof( DSCBUFFERDESC );
	dscbd.dwFlags = 0;
	dscbd.dwBufferBytes = wfx.nAvgBytesPerSec;
	dscbd.dwReserved = 0;
	dscbd.lpwfxFormat = &wfx;
	dscbd.dwFXCount = 0;
	dscbd.lpDSCFXDesc = NULL;

	if( FAILED( hr = DSCapture->CreateCaptureBuffer( &dscbd, &DSBuffer, NULL ) ) )
	{
		MessageBox( NULL, "Failed to create capture buffer", "Debug", MB_OK );

		return;
	}
}

AudioCapture::~AudioCapture()
{
	SAFE_RELEASE( DSBuffer );
	SAFE_RELEASE( DSCapture );
}

HRESULT AudioCapture::start()
{
	if( DSBuffer == NULL )
	{
		return( -1 );
	}

	return( DSBuffer->Start( DSCBSTART_LOOPING ) );
}

HRESULT AudioCapture::stop()
{
	if( DSBuffer == NULL )
	{
		return( -1 );
	}

	return( DSBuffer->Stop() );
}

HRESULT AudioCapture::bufferLock( DWORD BufferSize )
{
	HRESULT				 hr;
	LONG				 LockOffset;
	SYSTEMTIME			 SysTime;
	LONG				 Millis;

	if( ++LockCount > 1 )
	{
		return( DS_OK );
	}

	GetSystemTime( &SysTime );

	Millis = SysTime.wMilliseconds + ( SysTime.wSecond * 1000 ) + ( SysTime.wMinute * 60000 );

	Millis -= Millis % 15;

	if( Millis > this->LastRead )
	{
		this->LastRead = Millis;

		if( FAILED( hr = DSBuffer->GetCurrentPosition( NULL, &this->ReadPos ) ) )
		{
			LockCount--;

			return( hr );
		}
	}

	BufferSize *= sizeof(short) * 2;

	LockOffset = this->ReadPos - BufferSize;

	if( LockOffset < 0 )
	{
		LockOffset += CAPTURE_BUFFER_SIZE;
	}

	LockOffset &= ~0x03;

	if( FAILED( hr = DSBuffer->Lock( LockOffset, BufferSize, (LPVOID *)&AudioBuffer1, &AudioBytes1, (LPVOID *)&AudioBuffer2, &AudioBytes2, 0L ) ) )
	{
		LockCount--;

		return( hr );
	}

	return( hr );
}

HRESULT AudioCapture::bufferUnlock()
{
	if( --LockCount > 0 )
	{
		return( DS_OK );
	}

	return( DSBuffer->Unlock( AudioBuffer1, AudioBytes1, AudioBuffer2, AudioBytes2 ) );
}

float AudioCapture::getChannelLeft( LONG Offset )
{
	DWORD		BufferOffset;

	BufferOffset = Offset * sizeof( short ) * 2;

	if( BufferOffset < AudioBytes1 )
	{
		return( AudioBuffer1[ Offset ] / 32768.0f );
	}

	if( AudioBuffer2 == NULL )
	{
		return( 0.0f );
	}

	Offset       -= ( AudioBytes1 / ( sizeof( short ) * 2 ) );
	BufferOffset -= AudioBytes1;

	if( BufferOffset < AudioBytes2 )
	{
		return( AudioBuffer2[ Offset ] / 32768.0f );
	}

	return( 0.0f );
}

float AudioCapture::getChannelRight( LONG Offset )
{
	DWORD		BufferOffset;

	BufferOffset = Offset * sizeof( short ) * 2;

	if( BufferOffset < AudioBytes1 )
	{
		return( AudioBuffer1[ Offset + 1 ] / 32768.0f );
	}

	if( AudioBuffer2 == NULL )
	{
		return( 0.0f );
	}

	Offset       -= ( AudioBytes1 / ( sizeof( short ) * 2 ) );
	BufferOffset -= AudioBytes1;

	if( BufferOffset < AudioBytes2 )
	{
		return( AudioBuffer2[ Offset + 1 ] / 32768.0f );
	}

	return( 0.0f );
}

float AudioCapture::getChannelMix( LONG Offset )
{
	//return( getChannelLeft( Offset ) + getChannelRight( Offset ) / 2.0f );

	return( getChannelLeft( Offset ) );
}
