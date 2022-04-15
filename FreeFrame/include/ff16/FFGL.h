////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// FFGL.h
//
// FreeFrame is an open-source cross-platform real-time video effects plugin system.
// It provides a framework for developing video effects plugins and hosts on Windows, 
// Linux and Mac OSX. 
// 
// Copyright (c) 2006 www.freeframe.org
// All rights reserved. 
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// FFGL.h by Trey Harrison
// www.harrisondigitalmedia.com
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Redistribution and use in source and binary forms, with or without modification, 
//	are permitted provided that the following conditions are met:
//
//  * Redistributions of source code must retain the above copyright
//    notice, this list of conditions and the following disclaimer.
//  * Redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in
//    the documentation and/or other materials provided with the
//    distribution.
//  * Neither the name of FreeFrame nor the names of its
//    contributors may be used to endorse or promote products derived
//    from this software without specific prior written permission.
//
//
//	THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND 
//	ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED 
//	WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. 
//	IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, 
//	INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, 
//	BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, 
//	DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY 
//	OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE 
//	OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED 
//	OF THE POSSIBILITY OF SUCH DAMAGE. 
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __FFGL_H__
#define __FFGL_H__

//////////////////////////////////////////////////////////////////////////////////////
// Includes
/////////////////////////////////////////////////////////////////////////////////////

#ifndef __gl_h_
#include <glad/glad.h>
#endif

/////////////////////////////////////////////////////////////////////////////
//FreeFrameGL defines numerically extend FreeFrame defines (see FreeFrame.h)
/////////////////////////////////////////////////////////////////////////////
#include "FreeFrame.h"

// new function codes for FFGL
#define FF_PROCESSOPENGL       17
#define FF_INSTANTIATEGL       18
#define FF_DEINSTANTIATEGL     19
#define FF_SETTIME             20

// new plugin capabilities for FFGL
#define FF_CAP_PROCESSOPENGL    4
#define FF_CAP_SETTIME          5

//FFGLViewportStruct (for InstantiateGL)
typedef struct FFGLViewportStructTag
{
  GLuint x,y,width,height;
} FFGLViewportStruct;

//FFGLTextureStruct (for ProcessOpenGLStruct)
typedef struct FFGLTextureStructTag
{
  FFUInt32 Width, Height;
  FFUInt32 HardwareWidth, HardwareHeight;
  GLuint Handle; //the actual texture handle, from glGenTextures()
} FFGLTextureStruct;

// ProcessOpenGLStruct
typedef struct ProcessOpenGLStructTag {
  FFUInt32 numInputTextures;
  FFGLTextureStruct **inputTextures;
  
  //if the host calls ProcessOpenGL with a framebuffer object actively bound
  //(as is the case when the host is capturing the plugins output to an offscreen texture)
  //the host must provide the GL handle to its EXT_framebuffer_object
  //so that the plugin can restore that binding if the plugin
  //makes use of its own FBO's for intermediate rendering
  GLuint HostFBO; 
} ProcessOpenGLStruct;


#endif
