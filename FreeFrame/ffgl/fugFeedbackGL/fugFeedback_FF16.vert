#version 410 core

layout( location = 0 ) in vec4 vPosition;
layout( location = 1 ) in vec2 vUV;

out vec2 uv;

void main( void )
{
	gl_Position = vPosition;

	uv = vUV;
}
