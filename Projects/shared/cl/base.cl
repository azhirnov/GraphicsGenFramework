/*
	Base functions.

	Options:
		USR_COMPUTE_DEBUG	- enable debug checks
		COMPUTE_FP			- float precission.
*/

//------------------------------------------------------------------------------//
//									HEADER										//

// Emulation for C++ //
#if !defined(__OPENCL_VERSION__)

#	include "cl.h"

#	define STATIC_ASSERT( _condition_ )		static_assert( _condition_, #_condition_ )

#	define force_inline						inline

#	if !defined( COMPUTE_FP )
#		define COMPUTE_FP	32
#	endif

#endif


// Defines //
#if !defined(STATIC_ASSERT)
# if USR_COMPUTE_DEBUG
#	define __UNIQUE_ASSERT_NAME( a )		a##__LINE__
#	define STATIC_ASSERT( _condition_ )		enum { __UNIQUE_ASSERT_NAME( __static_assert_ ) = (1 / ((_condition_) ? 1 : 0)) }
# else
#	define STATIC_ASSERT( _condition_ )
# endif
#endif

#define not		!
#define or		||
#define and		&&

#define CountOf( _array_ )		(sizeof(_array_) / sizeof(_array_[0]))

#if !defined(force_inline)
#	define force_inline			__attribute__((always_inline))
#endif

#define OUT
#define INOUT


// Real Type //
#if COMPUTE_FP == 32

	typedef float			real;
	typedef float2			real2;
	typedef float3			real3;
	typedef float4			real4;
	typedef float8			real8;
	typedef float16			real16;
#	define convert_real		convert_float
#	define convert_real2	convert_float2
#	define convert_real3	convert_float3
#	define convert_real4	convert_float4
#	define convert_real8	convert_float8
#	define convert_real16	convert_float16
#	define as_real			as_float
#	define as_real2			as_float2
#	define as_real3			as_float3
#	define as_real4			as_float4
#	define as_real8			as_float8
#	define as_real16		as_float16

#elif COMPUTE_FP == 64

	typedef double			real;
	typedef double2			real2;
	typedef double3			real3;
	typedef double4			real4;
	typedef double8			real8;
	typedef double16		real16;
#	define convert_real		convert_double
#	define convert_real2	convert_double2
#	define convert_real3	convert_double3
#	define convert_real4	convert_double4
#	define convert_real8	convert_double8
#	define convert_real16	convert_double16
#	define as_real			as_double
#	define as_real2			as_double2
#	define as_real3			as_double3
#	define as_real4			as_double4
#	define as_real8			as_double8
#	define as_real16		as_double16

#else

#	error COMPUTE_FP not defined!

#endif	// COMPUTE_FP


// Byte Type //
typedef char			byte;
typedef char2			byte2;
typedef char3			byte3;
typedef char4			byte4;
typedef char8			byte8;
typedef char16			byte16;
#define convert_byte	convert_char
#define convert_byte2	convert_char2
#define convert_byte3	convert_char3
#define convert_byte4	convert_char4
#define convert_byte8	convert_char8
#define convert_byte16	convert_char16
#define as_byte			as_char
#define as_byte2		as_char2
#define as_byte3		as_char3
#define as_byte4		as_char4
#define as_byte8		as_char8
#define as_byte16		as_char16

typedef uchar			ubyte;
typedef uchar2			ubyte2;
typedef uchar3			ubyte3;
typedef uchar4			ubyte4;
typedef uchar8			ubyte8;
typedef uchar16			ubyte16;
#define convert_ubyte	convert_uchar
#define convert_ubyte2	convert_uchar2
#define convert_ubyte3	convert_uchar3
#define convert_ubyte4	convert_uchar4
#define convert_ubyte8	convert_uchar8
#define convert_ubyte16	convert_uchar16
#define as_ubyte		as_uchar
#define as_ubyte2		as_uchar2
#define as_ubyte3		as_uchar3
#define as_ubyte4		as_uchar4
#define as_ubyte8		as_uchar8
#define as_ubyte16		as_uchar16

typedef long			ilong;
typedef long2			ilong2;
typedef long3			ilong3;
typedef long4			ilong4;
typedef long8			ilong8;
typedef long16			ilong16;
#define convert_ilong	convert_long
#define convert_ilong2	convert_long2
#define convert_ilong3	convert_long3
#define convert_ilong4	convert_long4
#define convert_ilong8	convert_long8
#define convert_ilong16	convert_long16
#define as_ilong		as_long
#define as_ilong2		as_long2
#define as_ilong3		as_long3
#define as_ilong4		as_long4
#define as_ilong8		as_long8
#define as_ilong16		as_long16

#define bool			int
#define bool2			int2
#define bool3			int3
#define bool4			int4
#define convert_bool	convert_int
#define convert_bool2	convert_int2
#define convert_bool3	convert_int3
#define convert_bool4	convert_int4


// Utils //
int   GetGlobalID ();
int   GetGlobalSize ();

real  GetGlobalIdUNorm ();
real  GetGlobalIdSNorm ();

int2  GetGlobalCoord2D ();
int3  GetGlobalCoord3D ();

int2  GetGlobalSize2D ();
int3  GetGlobalSize3D ();

real2 GetGlobalSize2DScaled (real2 scale);
real3 GetGlobalSize3DScaled (real3 scale);

real2 GetGlobalCoord2DTransformed (real2 scale, real2 bias);
real3 GetGlobalCoord3DTransformed (real3 scale, real3 bias);

int2  GetGlobalCoord2DTransformedI (real2 scale, real2 bias);
int3  GetGlobalCoord3DTransformedI (real3 scale, real3 bias);

real2 GetGlobalCoord2DTransformedUNorm (real2 scale, real2 bias);
real3 GetGlobalCoord3DTransformedUNorm (real3 scale, real3 bias);

real2 GetGlobalCoord2DTransformedSNorm (real2 scale, real2 bias);
real3 GetGlobalCoord3DTransformedSNorm (real3 scale, real3 bias);

real2 GetGlobalCoord2DTransformedUNormCorrected (real scale, real2 bias);
real2 GetGlobalCoord2DTransformedSNormCorrected (real scale, real2 bias);


//------------------------------------------------------------------------------//
//									SOURCE										//

force_inline int GetGlobalID ()
{
	return	get_global_id(0) +
			get_global_size(0) * get_global_id(1) +
			get_global_size(0) * get_global_size(1) * get_global_id(2);
}

force_inline int GetGlobalSize ()
{
	return get_global_size(0) * get_global_size(1) * get_global_size(2);
}

force_inline real GetGlobalIdUNorm ()
{
	return convert_real( GetGlobalID() ) / convert_real( GetGlobalSize()-1 );
}

force_inline real GetGlobalIdSNorm ()
{
	return GetGlobalIdUNorm() * 2.0 - 1.0;
}

force_inline int2 GetGlobalCoord2D ()
{
	return (int2)( get_global_id(0), get_global_id(1) );
}

force_inline int3 GetGlobalCoord3D ()
{
	return (int3)( get_global_id(0), get_global_id(1), get_global_id(2) );
}

force_inline int2 GetGlobalSize2D ()
{
	return (int2)( get_global_size(0), get_global_size(1) );
}

force_inline int3 GetGlobalSize3D ()
{
	return (int3)( get_global_size(0), get_global_size(1), get_global_size(2) );
}

force_inline real2 GetGlobalCoord2DTransformed (real2 scale, real2 bias)
{
	return convert_real2( GetGlobalCoord2D() ) * scale + bias;
}

force_inline real3 GetGlobalCoord3DTransformed (real3 scale, real3 bias)
{
	return convert_real3( GetGlobalCoord3D() ) * scale + bias;
}

force_inline int2  GetGlobalCoord2DTransformedI (real2 scale, real2 bias)
{
	return convert_int2( round( GetGlobalCoord2DTransformed( scale, bias ) ) );
}

force_inline int3  GetGlobalCoord3DTransformedI (real3 scale, real3 bias)
{
	return convert_int3( round( GetGlobalCoord3DTransformed( scale, bias ) ) );
}

force_inline real2 GetGlobalSize2DScaled (real2 scale)
{
	return convert_real2( GetGlobalSize2D() ) * scale;
}

force_inline real3  GetGlobalSize3DScaled (real3 scale)
{
	return convert_real3( GetGlobalSize3D() ) * scale;
}

force_inline real2 GetGlobalCoord2DTransformedUNorm (real2 scale, real2 bias)
{
	return ( convert_real2( GetGlobalCoord2D() ) / convert_real2( GetGlobalSize2D() ) ) * scale + bias;
}

force_inline real3 GetGlobalCoord3DTransformedUNorm (real3 scale, real3 bias)
{
	return ( convert_real3( GetGlobalCoord3D() ) / convert_real3( GetGlobalSize3D() ) ) * scale + bias;
}

force_inline real2 GetGlobalCoord2DTransformedSNorm (real2 scale, real2 bias)
{
	real2	size = convert_real2( GetGlobalSize2D() ) * 0.5;
	return ( (convert_real2( GetGlobalCoord2D() ) - size) / size ) * scale + bias;
}

force_inline real3 GetGlobalCoord3DTransformedSNorm (real3 scale, real3 bias)
{
	real3	size = convert_real3( GetGlobalSize3D() ) * 0.5;
	return ( (convert_real3( GetGlobalCoord3D() ) - size) / size ) * scale + bias;
}

force_inline real2 GetGlobalCoord2DTransformedSNormCorrected (real scale, real2 bias)
{
	real2	size	= convert_real2( GetGlobalSize2D() );
	real2	aspect	= size.x > size.y ?	(real2)( 1.0, size.y / size.x ) :
										(real2)( size.x / size.y, 1.0 );
	return GetGlobalCoord2DTransformedSNorm( (real2)(scale, scale), bias ) * aspect;
}

force_inline real2 GetGlobalCoord2DTransformedUNormCorrected (real scale, real2 bias)
{
	real2	size	= convert_real2( GetGlobalSize2D() );
	real2	aspect	= size.x > size.y ?	(real2)( 1.0, size.y / size.x ) :
										(real2)( size.x / size.y, 1.0 );
	return GetGlobalCoord2DTransformedUNorm( (real2)(scale, scale), bias ) * aspect;
}
