
#include <time.h>
#include <windows.h>
#include <basetsd.h>
#include <commdlg.h>
#include <mmreg.h>
#include <dxerr9.h>
#include <mmsystem.h>
#include <dsound.h>

#define SAFE_DELETE(p)  { if(p) { delete (p);     (p)=NULL; } }
#define SAFE_RELEASE(p) { if(p) { (p)->Release(); (p)=NULL; } }

#define	CAPTURE_BUFFER_SIZE		(176400)

class AudioCapture
{
private:
	LPDIRECTSOUNDCAPTURE		 DSCapture;
	LPDIRECTSOUNDCAPTUREBUFFER	 DSBuffer;
	short						*AudioBuffer1;
	short						*AudioBuffer2;
	DWORD						 AudioBytes1;
	DWORD						 AudioBytes2;
	DWORD						 ReadPos;
	LONG						 LastRead;
	int							 LockCount;

public:
	AudioCapture();
	~AudioCapture();

	HRESULT		bufferLock( DWORD BufferLen );
	HRESULT		bufferUnlock();

	HRESULT		start();
	HRESULT		stop();

	float		getChannelLeft( LONG Offset );
	float		getChannelRight( LONG Offset );
	float		getChannelMix( LONG Offset );
};
