/*
	Types converter
*/

#define BYTE_TO_SNORM( _x_ )	(convert_real( _x_ ) / 128.0)
#define UBYTE_TO_UNORM( _x_ )	(convert_real( _x_ ) / 255.0)

#define BYTE_TO_SNORM2( _x_ )	(convert_real2( _x_ ) / 128.0)
#define UBYTE_TO_UNORM2( _x_ )	(convert_real2( _x_ ) / 255.0)

#define SNORM_TO_BYTE( _x_ )	(convert_byte( clamp( (_x_) * 128.0, -128.0, 127.0) ))
#define UNORM_TO_UBYTE( _x_ )	(convert_ubyte( clamp( (_x_) * 255.0, 0.0, 255.0 ) ))

#define SNORM_TO_BYTE2( _x_ )	(convert_byte2( clamp( (_x_) * 128.0, -128.0, 127.0) ))
#define UNORM_TO_UBYTE2( _x_ )	(convert_ubyte2( clamp( (_x_) * 255.0, 0.0, 255.0 ) ))


// UNormToSNorm
#define UNORM_TO_SNORM( _x_ )	((_x_) * 2.0 - 1.0)		//  0..1  to -1..+1
#define SNORM_TO_UNORM( _x_ )	((_x_) * 0.5 + 0.5)		// -1..+1 to  0..1
