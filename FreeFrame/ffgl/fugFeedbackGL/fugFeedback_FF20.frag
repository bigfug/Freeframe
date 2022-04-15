#version 410 core

uniform sampler2D	TexRGBA;
uniform sampler2D	TexBAK;
uniform int			Mode;
uniform float		LumMin;
uniform float		LumMax;
uniform float		Rotation;
uniform float		Zoom;
uniform float		Direction;
uniform float		Amount;
uniform float		Fade;
uniform float		HueRotate;
uniform float		ModX, ModY;

in vec2 uv0;
in vec2 uv1;
out vec4 fragColor;

vec3 RGBToHSL(vec3 color)
{
	vec3 hsl; // init to 0 to avoid warnings ? (and reverse if + remove first part)

	float fmin = min(min(color.r, color.g), color.b); //Min. value of RGB
	float fmax = max(max(color.r, color.g), color.b); //Max. value of RGB
	float delta = fmax - fmin; //Delta RGB value

	hsl.z = (fmax + fmin) / 2.0; // Luminance

	if (delta == 0.0)	 //This is a gray, no chroma...
	{
		hsl.x = 0.0;	// Hue
		hsl.y = 0.0;	// Saturation
	}
	else //Chromatic data...
	{
		if (hsl.z < 0.5)
			hsl.y = delta / (fmax + fmin); // Saturation
		else
			hsl.y = delta / (2.0 - fmax - fmin); // Saturation

		float deltaR = (((fmax - color.r) / 6.0) + (delta / 2.0)) / delta;
		float deltaG = (((fmax - color.g) / 6.0) + (delta / 2.0)) / delta;
		float deltaB = (((fmax - color.b) / 6.0) + (delta / 2.0)) / delta;

		if (color.r == fmax )
			hsl.x = deltaB - deltaG; // Hue
		else if (color.g == fmax)
			hsl.x = (1.0 / 3.0) + deltaR - deltaB; // Hue
		else if (color.b == fmax)
			hsl.x = (2.0 / 3.0) + deltaG - deltaR; // Hue

		if (hsl.x < 0.0)
			hsl.x += 1.0; // Hue
		else if (hsl.x > 1.0)
			hsl.x -= 1.0; // Hue
	}

	return hsl;

}

float HueToRGB(float f1, float f2, float hue)
{
	if (hue < 0.0)
		hue += 1.0;
	else if (hue > 1.0)
		hue -= 1.0;
	float res;
	if ((6.0 * hue) < 1.0)
		res = f1 + (f2 - f1) * 6.0 * hue;
	else if ((2.0 * hue) < 1.0)
		res = f2;
	else if ((3.0 * hue) < 2.0)
		res = f1 + (f2 - f1) * ((2.0 / 3.0) - hue) * 6.0;
	else
		res = f1;
	return res;
}

vec3 HSLToRGB(vec3 HSL)
{
	vec3 RGB;

	if (HSL.y == 0.0)
	{
		RGB = vec3(HSL.z); // Luminance
	}
	else
	{
		float f2;

		if (HSL.z < 0.5)
			f2 = HSL.z * (1.0 + HSL.y);
		else
			f2 = (HSL.z + HSL.y) - (HSL.y * HSL.z);

		float f1 = 2.0 * HSL.z - f2;

		RGB.r = HueToRGB(f1, f2, HSL.x + (1.0/3.0));
		RGB.g = HueToRGB(f1, f2, HSL.x);
		RGB.b = HueToRGB(f1, f2, HSL.x - (1.0/3.0));
	}

	return RGB;
}

void main( void )
{
	vec2	t = uv1; //gl_TexCoord[ 1 ].st;

	if( t.x > 1.0 || t.y > 1.0 )
	{
		discard;
	}

	t *= vec2( 1.0f / ModX, 1.0f / ModY );

	t -= 0.5;

	float	a  = radians( Rotation );
	float	ca = cos( a );
	float	sa = sin( a );
	float	x = t.x * ca - t.y * sa;
	float	y = t.y * ca + t.x * sa;

	float	da = radians( Direction );

	t = ( vec2( x, y ) * Zoom ) + ( vec2( -sin( da ), cos( da ) ) * Amount ) + 0.5;

	t *= vec2( ModX, ModY );

	//t = mod( t, vec2( ModX, ModY ) );

	vec4	RGBA = texture( TexRGBA, uv0 );
	vec4	BACK = texture( TexBAK, t );

	float	Luma;

	if( Mode == 0 )
	{
		Luma = RGBA.a;
	}
	else if( Mode == 1 )
	{
		Luma = 0.2126 * RGBA.r + 0.7152 * RGBA.g + 0.0722 * RGBA.b;
	}
	else if( Mode == 2 )
	{
		Luma = max( RGBA.r, max( RGBA.g, RGBA.b ) );
	}

	if( Luma < LumMin || Luma > LumMax )
	{
		Luma = 0.0;
	}

	vec3	BHSL = RGBToHSL( BACK.rgb );
	vec3	BRGB = HSLToRGB( BHSL + vec3( HueRotate, 0.0, 0.0 ) );

	fragColor = vec4( mix( BRGB * ( 1.0 - Fade ), RGBA.rgb, Luma ), max( RGBA.a * Luma, BACK.a * ( 1.0 - Fade ) ) );
}
