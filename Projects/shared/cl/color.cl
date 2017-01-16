/*
	Color functions
*/

#include "base.cl"


force_inline real3  RGBtoHSV (real3 rgb)
{
	real	k = 0.0;
	real	r = rgb.x;
	real	g = rgb.y;
	real	b = rgb.z;
	real	t = 0.0;

	if ( g < b )
	{
		t = g;	g = b;	b = t;
		k = -1.0;
	}

	if ( r < g )
	{
		t = r;	r = g;	g = t;
		k = -2.0 / 6.0 - k;
	}

	real	chroma = r - min( g, b );

	return (real3)( fabs( k + (g - b) / 6.0 * chroma + 1e-20 ),
					chroma / ( r + 1e-20 ),
					r );
}


force_inline real3 HSVtoRGB (real3 hsv)
{
	real3 col = (real3)( fabs( hsv.x * 6.0 - 3.0 ) - 1.0,
						 2.0 - fabs( hsv.x * 6.0 - 2.0 ),
						 2.0 - fabs( hsv.x * 6.0 - 4.0 ) );

	return (( clamp( col, (real3)(0.0), (real3)(1.0) ) - 1.0 ) * hsv.y + 1.0 ) * hsv.z;
}


force_inline real4 ColorPalette (real value, const real4 *colors, int count)
{
	real	fcount	= convert_real( count-1 );
	real	a		= clamp( value * fcount, 0.0, fcount );
	real	fi		= floor( min( a, fcount - 1.0 ) );
	int		i		= convert_int( fi );

	a -= fi;
	return mix( colors[i], colors[i+1], a );
}


force_inline real4 RGBAfromInt (uint intColor)
{
	const uint4	offset = (uint4)( 24, 16, 8, 0 );
	return convert_real4( ((uint4)( intColor ) >> offset) & 0xFF ) * 0.00392;
}

force_inline real4 RGBfromInt (uint intColor)
{
	const uint3	offset = (uint3)( 16, 8, 0 );
	return (real4)( convert_real3( ((uint3)( intColor ) >> offset) & 0xFF ) * 0.00392, 1.0 );
}

force_inline uint RGBAtoInt (real4 color)
{
	const uint4	offset = (uint4)( 24, 16, 8, 0 );

	uint4 i = ( convert_uint4( round( color * 255 ) ) & 0xFF ) << offset;

	return ( i.x | i.y | i.z | i.w );
}

enum _EDefaultIntColors
{
	Color_AliceBlue				= 0xF0F8FF,
	Color_AntiqueWhite			= 0xFAEBD7,
	Color_Aqua					= 0x00FFFF,
	Color_Aquamarine			= 0x7FFFD4,
	Color_Azure					= 0xF0FFFF,
	Color_Beige					= 0xF5F5DC,
	Color_Bisque				= 0xFFE4C4,
	Color_Black					= 0x000000,
	Color_BlanchedAlmond		= 0xFFEBCD,
	Color_Blue					= 0x0000FF,
	Color_BlueViolet			= 0x8A2BE2,
	Color_Brown					= 0xA52A2A,
	Color_BurlyWood				= 0xDEB887,
	Color_CadetBlue				= 0x5F9EA0,
	Color_Chartreuse			= 0x7FFF00,
	Color_Chocolate				= 0xD2691E,
	Color_Coral					= 0xFF7F50,
	Color_CornflowerBlue		= 0x6495ED,
	Color_Cornsilk				= 0xFFF8DC,
	Color_Crimson				= 0xDC143C,
	Color_Cyan					= 0x00FFFF,
	Color_DarkBlue				= 0x00008B,
	Color_DarkCyan				= 0x008B8B,
	Color_DarkGoldenRod			= 0xB8860B,
	Color_DarkGray				= 0xA9A9A9,
	Color_DarkGreen				= 0x006400,
	Color_DarkKhaki				= 0xBDB76B,
	Color_DarkMagenta			= 0x8B008B,
	Color_DarkOliveGreen		= 0x556B2F,
	Color_DarkOrange			= 0xFF8C00,
	Color_DarkOrchid			= 0x9932CC,
	Color_DarkRed				= 0x8B0000,
	Color_DarkSalmon			= 0xE9967A,
	Color_DarkSeaGreen			= 0x8FBC8F,
	Color_DarkSlateBlue			= 0x483D8B,
	Color_DarkSlateGray			= 0x2F4F4F,
	Color_DarkTurquoise			= 0x00CED1,
	Color_DarkViolet			= 0x9400D3,
	Color_DeepPink				= 0xFF1493,
	Color_DeepSkyBlue			= 0x00BFFF,
	Color_DimGray				= 0x696969,
	Color_DodgerBlue			= 0x1E90FF,
	Color_FireBrick				= 0xB22222,
	Color_FloralWhite			= 0xFFFAF0,
	Color_ForestGreen			= 0x228B22,
	Color_Fuchsia				= 0xFF00FF,
	Color_Gainsboro				= 0xDCDCDC,
	Color_GhostWhite			= 0xF8F8FF,
	Color_Gold					= 0xFFD700,
	Color_GoldenRod				= 0xDAA520,
	Color_Gray					= 0x808080,
	Color_Green					= 0x008000,
	Color_GreenYellow			= 0xADFF2F,
	Color_HoneyDew				= 0xF0FFF0,
	Color_HotPink				= 0xFF69B4,
	Color_IndianRed				= 0xCD5C5C,
	Color_Indigo				= 0x4B0082,
	Color_Ivory					= 0xFFFFF0,
	Color_Khaki					= 0xF0E68C,
	Color_Lavender				= 0xE6E6FA,
	Color_LavenderBlush			= 0xFFF0F5,
	Color_LawnGreen				= 0x7CFC00,
	Color_LemonChiffon			= 0xFFFACD,
	Color_LightBlue				= 0xADD8E6,
	Color_LightCoral			= 0xF08080,
	Color_LightCyan				= 0xE0FFFF,
	Color_LightGoldenRodYellow	= 0xFAFAD2,
	Color_LightGray				= 0xD3D3D3,
	Color_LightGreen			= 0x90EE90,
	Color_LightPink				= 0xFFB6C1,
	Color_LightSalmon			= 0xFFA07A,
	Color_LightSeaGreen			= 0x20B2AA,
	Color_LightSkyBlue			= 0x87CEFA,
	Color_LightSlateGray		= 0x778899,
	Color_LightSteelBlue		= 0xB0C4DE,
	Color_LightYellow			= 0xFFFFE0,
	Color_Lime					= 0x00FF00,
	Color_LimeGreen				= 0x32CD32,
	Color_Linen					= 0xFAF0E6,
	Color_Magenta				= 0xFF00FF,
	Color_Maroon				= 0x800000,
	Color_MediumAquaMarine		= 0x66CDAA,
	Color_MediumBlue			= 0x0000CD,
	Color_MediumOrchid			= 0xBA55D3,
	Color_MediumPurple			= 0x9370DB,
	Color_MediumSeaGreen		= 0x3CB371,
	Color_MediumSlateBlue		= 0x7B68EE,
	Color_MediumSpringGreen		= 0x00FA9A,
	Color_MediumTurquoise		= 0x48D1CC,
	Color_MediumVioletRed		= 0xC71585,
	Color_MidnightBlue			= 0x191970,
	Color_MintCream				= 0xF5FFFA,
	Color_MistyRose				= 0xFFE4E1,
	Color_Moccasin				= 0xFFE4B5,
	Color_NavajoWhite			= 0xFFDEAD,
	Color_Navy					= 0x000080,
	Color_OldLace				= 0xFDF5E6,
	Color_Olive					= 0x808000,
	Color_OliveDrab				= 0x6B8E23,
	Color_Orange				= 0xFFA500,
	Color_OrangeRed				= 0xFF4500,
	Color_Orchid				= 0xDA70D6,
	Color_PaleGoldenRod			= 0xEEE8AA,
	Color_PaleGreen				= 0x98FB98,
	Color_PaleTurquoise			= 0xAFEEEE,
	Color_PaleVioletRed			= 0xDB7093,
	Color_PapayaWhip			= 0xFFEFD5,
	Color_PeachPuff				= 0xFFDAB9,
	Color_Peru					= 0xCD853F,
	Color_Pink					= 0xFFC0CB,
	Color_Plum					= 0xDDA0DD,
	Color_PowderBlue			= 0xB0E0E6,
	Color_Purple				= 0x800080,
	Color_Red					= 0xFF0000,
	Color_RosyBrown				= 0xBC8F8F,
	Color_RoyalBlue				= 0x4169E1,
	Color_SaddleBrown			= 0x8B4513,
	Color_Salmon				= 0xFA8072,
	Color_SandyBrown			= 0xF4A460,
	Color_SeaGreen				= 0x2E8B57,
	Color_SeaShell				= 0xFFF5EE,
	Color_Sienna				= 0xA0522D,
	Color_Silver				= 0xC0C0C0,
	Color_SkyBlue				= 0x87CEEB,
	Color_SlateBlue				= 0x6A5ACD,
	Color_SlateGray				= 0x708090,
	Color_Snow					= 0xFFFAFA,
	Color_SpringGreen			= 0x00FF7F,
	Color_SteelBlue				= 0x4682B4,
	Color_Tan					= 0xD2B48C,
	Color_Teal					= 0x008080,
	Color_Thistle				= 0xD8BFD8,
	Color_Tomato				= 0xFF6347,
	Color_Turquoise				= 0x40E0D0,
	Color_Violet				= 0xEE82EE,
	Color_Wheat					= 0xF5DEB3,
	Color_White					= 0xFFFFFF,
	Color_WhiteSmoke			= 0xF5F5F5,
	Color_Yellow				= 0xFFFF00,
	Color_YellowGreen			= 0x9ACD32,
};
