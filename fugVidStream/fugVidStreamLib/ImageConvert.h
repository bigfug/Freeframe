#pragma once

#include "fugVidStream.h"
#include <vector>

struct SwsContext;

class ImageConvert
{
protected:
//	SwsContext				*mContext;
	const fugVideoInfo		 mInput;
	const fugVideoInfo		 mOutput;
	int						 mInputStride;
	int						 mOutputStride;
	std::vector<uint32_t>	 mConvertionInput;
	std::vector<uint32_t>	 mConvertionOutput;

public:
	ImageConvert( const fugVideoInfo &pInput, const fugVideoInfo &pOutput );

	~ImageConvert( void );

	void convert( const void *pInput, void *pOutput );

	size_t getOutputByteSize( void ) const;
};
