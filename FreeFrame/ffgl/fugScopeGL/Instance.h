
#pragma once

#include <GL/glew.h>

#include <InstanceBase.h>
#include "Plugin.h"

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

protected:
	Plugin					&mPlugin;
	PaStream				*mStream;
	int						 mChannels;
	vector<float>			 mWaveForm;
	vector<float>			 mWavSrc;
	vector<GLfloat>			 mElements;
};
