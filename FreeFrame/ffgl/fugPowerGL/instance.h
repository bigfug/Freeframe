#ifndef INSTANCE_H
#define INSTANCE_H

#include <GL/glew.h>

#include <InstanceBase.h>
#include "plugin.h"

#include <FFT.h>

#include <portaudio.h>

class Instance : public InstanceBase
{
public:
	Instance( Plugin &pPlugin, FreeFrameApiVersion pApiVersion, plugMainUnion &pResult );

	virtual ~Instance( void );

	virtual void processOpengl( plugMainUnion &pResult );

private:
	void drawElements( GLenum pMode );

	static int paCallbackStatic( const void *inputBuffer, void *outputBuffer,
							   unsigned long framesPerBuffer,
							   const PaStreamCallbackTimeInfo* timeInfo,
							   PaStreamCallbackFlags statusFlags,
							   void *userData );

	int paCallback( const void *inputBuffer, void *outputBuffer,
					unsigned long framesPerBuffer,
					const PaStreamCallbackTimeInfo* timeInfo,
					PaStreamCallbackFlags statusFlags );

	int bands( void );

protected:
	Plugin						&mPlugin;
	PaStream					*mStream;
	int							 mChannels;
	vector<float>				 mWaveWindow;
	vector< complex<float> >	 mWaveFFT;
	vector<float>				 mWavePower;
	vector<float>				 mWaveLinear;
	vector<float>				 mWaveBands;
	vector<float>				 mWaveData;
	vector<GLfloat>				 mElements;
	DanielsonLanczos<float>		 mFFT;
};

#endif // INSTANCE_H
