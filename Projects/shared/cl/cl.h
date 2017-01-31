// Copyright © 2014-2017  Zhirnov Andrey. All rights reserved.
/*
	Header to compile OpenCL program with C++ compiler.
*/

#pragma once

// check //
#if defined(__OPENCL_VERSION__)
#	error this header not supported in real opencl compiler!
#endif


// helpers //
#define __CL_EXPORT__	// TODO
#define __ASSERT(x)		// TODO


// CL //
#define __CL_CPP_EMULATION__				1

#define __OPENCL_VERSION__					110
#define __OPENCL_C_VERSION__				__OPENCL_VERSION__
#define CL_VERSION_1_0						100
#define CL_VERSION_1_1						110
#define CL_VERSION_1_2						120
#define CL_VERSION_2_0						200
#define __kernel_exec( X, typen )

#define __ENDIAN_LITTLE__					1
#define __IMAGE_SUPPORT__					1
#define __FAST_RELAXED_MATH__				0
#define CL_DEVICE_MAX_GLOBAL_VARIABLE_SIZE	(64*1024)
#define CL_DEVICE_ADDRESS_BITS				32
#define cl_khr_fp64							1
#define cl_khr_3d_image_writes				1
/*
#define not		!
#define or		||
#define and		&&
*/

namespace _cl_
{

	//
	// Helpers
	//

	inline void memset (void *dst, int value, int size)
	{
		char * p = (char *) dst;
		for (int i = 0; i < size; ++i) {
			p[i] = (char) value;
		}
	}

	template <typename T>
	inline void ZeroMem (T &mem)
	{
		memset( &mem, 0, sizeof(mem) );
	}


	template <typename A, typename B>
	struct TypeConversionCheckImpl
	{
		TypeConversionCheckImpl ()
		{
			const A tmp = B();
		}
	};
	
	template <typename A>
	struct TypeConversionCheckImpl < A, void >
	{};
	
	template <	typename T,
				typename T0 = void, typename T1 = void, typename T2 = void, typename T3 = void,
				typename T4 = void, typename T5 = void, typename T6 = void, typename T7 = void,
				typename T8 = void, typename T9 = void, typename TA = void, typename TB = void,
				typename TC = void, typename TD = void, typename TE = void, typename TF = void >
	struct TypeConversionCheck
	{
		TypeConversionCheck ()
		{
			TypeConversionCheckImpl< T, T0 >();
			TypeConversionCheckImpl< T, T1 >();
			TypeConversionCheckImpl< T, T2 >();
			TypeConversionCheckImpl< T, T3 >();
			TypeConversionCheckImpl< T, T4 >();
			TypeConversionCheckImpl< T, T5 >();
			TypeConversionCheckImpl< T, T6 >();
			TypeConversionCheckImpl< T, T7 >();
			TypeConversionCheckImpl< T, T8 >();
			TypeConversionCheckImpl< T, T9 >();
			TypeConversionCheckImpl< T, TA >();
			TypeConversionCheckImpl< T, TB >();
			TypeConversionCheckImpl< T, TC >();
			TypeConversionCheckImpl< T, TD >();
			TypeConversionCheckImpl< T, TE >();
			TypeConversionCheckImpl< T, TF >();
		}
	};


	//
	// Vectors
	//

#	define VFOR( _i_ )			for (int _i_ = 0; _i_ < COUNT; ++_i_)

#	define __VEC_SWIZZLE_TYPES__ \
	private: \
		typedef VecSwizzle< TVec2<T>, T >	sw2_t; \
		typedef VecSwizzle< TVec3<T>, T >	sw3_t; \
		typedef VecSwizzle< TVec4<T>, T >	sw4_t; \
	
#	define __VEC2_SWIZZLE__ \
		T		s0, s1; \
		sw2_t	xx, yy, xy, yx; \
		sw2_t	s00, s11, s01, s10; \
		sw3_t	xxx, xxy, xyx, xyy, yxx, yyx, yyy; \
		sw3_t	s000, s001, s010, s011, s100, s110, s111; \
		sw4_t	xxxx, yyyy; \
		sw4_t	s0000, s1111; \

#	define __VEC3_SWIZZLE__ \
		__VEC2_SWIZZLE__ \
		T		s2; \
		sw2_t	xz, zx, yz, zy, zz; \
		sw2_t	s02, s20, s12, s21, s22; \
		sw3_t	xyz; \
		sw3_t	s012; \
		sw4_t	xyzz; \
		sw4_t	s0122; \

#	define __VEC4_SWIZZLE__ \
		__VEC3_SWIZZLE__ \
		T		s3; \
		sw2_t	xw, wx, yw, wy, zw, wz, ww; \
		sw2_t	s03, s30, s13, s31, s23, s32, s33; \
		sw3_t	www; \
		sw3_t	s333; \
		sw4_t	wwww; \
		sw4_t	s3333;

#	define __VEC_OPERATORS__ \
		Self   operator +  () const						{ return *this; } \
		Self   operator -  () const						{ return *this; } \
		Self   operator ~  () const						{ return *this; } \
		Self   operator !  () const						{ return *this; } \
		\
		Self & operator -- ()							{ return *this; } \
		Self & operator ++ ()							{ return *this; } \
		Self   operator -- (int) const					{ return *this; } \
		Self   operator ++ (int) const					{ return *this; } \
		\
		Self & operator =  (const Self &right)			{ return *this; } \
		Self & operator += (const Self &right)			{ return *this; } \
		Self & operator -= (const Self &right)			{ return *this; } \
		Self & operator *= (const Self &right)			{ return *this; } \
		Self & operator /= (const Self &right)			{ return *this; } \
		Self & operator %= (const Self &right)			{ return *this; } \
		Self & operator |= (const Self &right)			{ return *this; } \
		Self & operator &= (const Self &right)			{ return *this; } \
		Self & operator ^= (const Self &right)			{ return *this; } \
		Self & operator >>=(const Self &right)			{ return *this; } \
		Self & operator <<=(const Self &right)			{ return *this; } \
		\
		Self & operator =  (const T &right)				{ return *this; } \
		Self & operator += (const T &right)				{ return *this; } \
		Self & operator -= (const T &right)				{ return *this; } \
		Self & operator *= (const T &right)				{ return *this; } \
		Self & operator /= (const T &right)				{ return *this; } \
		Self & operator %= (const T &right)				{ return *this; } \
		Self & operator |= (const T &right)				{ return *this; } \
		Self & operator &= (const T &right)				{ return *this; } \
		Self & operator ^= (const T &right)				{ return *this; } \
		Self & operator >>=(const T &right)				{ return *this; } \
		Self & operator <<=(const T &right)				{ return *this; } \
		\
		Self   operator +  (const Self &right) const	{ return Self(); } \
		Self   operator -  (const Self &right) const	{ return Self(); } \
		Self   operator *  (const Self &right) const	{ return Self(); } \
		Self   operator /  (const Self &right) const	{ return Self(); } \
		Self   operator %  (const Self &right) const	{ return Self(); } \
		Self   operator |  (const Self &right) const	{ return Self(); } \
		Self   operator &  (const Self &right) const	{ return Self(); } \
		Self   operator ^  (const Self &right) const	{ return Self(); } \
		Self   operator >> (const Self &right) const	{ return Self(); } \
		Self   operator << (const Self &right) const	{ return Self(); } \
		\
		Self   operator +  (const T &right) const		{ return Self(); } \
		Self   operator -  (const T &right) const		{ return Self(); } \
		Self   operator *  (const T &right) const		{ return Self(); } \
		Self   operator /  (const T &right) const		{ return Self(); } \
		Self   operator %  (const T &right) const		{ return Self(); } \
		Self   operator |  (const T &right) const		{ return Self(); } \
		Self   operator &  (const T &right) const		{ return Self(); } \
		Self   operator ^  (const T &right) const		{ return Self(); } \
		Self   operator >> (const T &right) const		{ return Self(); } \
		Self   operator << (const T &right) const		{ return Self(); } \
		\
		bool   operator == (const Self &right) const	{ return false; } \
		bool   operator != (const Self &right) const	{ return true; } \
		bool   operator >  (const Self &right) const	{ return false; } \
		bool   operator <  (const Self &right) const	{ return false; } \
		bool   operator >= (const Self &right) const	{ return false; } \
		bool   operator <= (const Self &right) const	{ return false; } \
		\
		T &			operator [] (int i)					{ return reinterpret_cast< T *>( this )[i]; } \
		T const &	operator [] (int i) const			{ return reinterpret_cast< T const *>( this )[i]; }

	
	template <typename T>	struct TVec2;
	template <typename T>	struct TVec3;
	template <typename T>	struct TVec4;
	template <typename T>	struct TVec8;
	template <typename T>	struct TVec16;
	template <typename V, typename T> struct VecSwizzle;
	

	template <typename V, typename T>
	struct VecSwizzle
	{
	public:
		typedef V	Self;

		operator V () const		{ return _Get(); }
		operator V & ()			{ return _Get(); }

		__VEC_OPERATORS__

	private:
		static V & _Get ()
		{
			static V	v;
			return v;
		}
	};
	

	template <typename A>
	struct TypeConversionCheckImpl < A, VecSwizzle< TVec2<A>, A > >
	{};
	
	template <typename A>
	struct TypeConversionCheckImpl < A, VecSwizzle< TVec3<A>, A > >
	{};
	
	template <typename A>
	struct TypeConversionCheckImpl < A, VecSwizzle< TVec4<A>, A > >
	{};

	template <typename A>
	struct TypeConversionCheckImpl < A, VecSwizzle< TVec8<A>, A > >
	{};

	template <typename A>
	struct TypeConversionCheckImpl < A, VecSwizzle< TVec16<A>, A > >
	{};


	template <typename T>
	struct TVec2
	{
	public:
		typedef TVec2<T>	Self;
		typedef bool		_is_vector_t;
		typedef T			value_t;
		enum { COUNT = 2 };

		__VEC_SWIZZLE_TYPES__

	public:
		T	x, y;

		union {
			__VEC2_SWIZZLE__
		};

	public:
		TVec2 ()							{ ZeroMem( *this ); }

		template <typename T0>
		TVec2 (T0)
		{
			ZeroMem( *this );
			TypeConversionCheck< T, T0 >();
		}

		template <typename T0, typename T1>
		explicit
		TVec2 (T0, T1)
		{
			ZeroMem( *this );
			TypeConversionCheck< T, T0, T1 >();
		}

		TVec2 (const Self &)				{ ZeroMem( *this ); }

		__VEC_OPERATORS__
	};

	
	template <typename T>
	struct TVec3
	{
	public:
		typedef TVec3<T>	Self;
		typedef bool		_is_vector_t;
		typedef T			value_t;
		enum { COUNT = 3 };

		__VEC_SWIZZLE_TYPES__

	public:
		T	x, y, z,
			__forAlign;		// sizeof(TVec3) == sizeof(TVec4)	as specification says
		
		union {
			__VEC3_SWIZZLE__
		};

	public:
		TVec3 ()							{ ZeroMem( *this ); }

		template <typename T0>
		TVec3 (T0)
		{
			ZeroMem( *this );
			TypeConversionCheck< T, T0 >();
		}
		
		template <typename T0, typename T1>
		explicit
		TVec3 (T0, T1)
		{
			ZeroMem( *this );
			TypeConversionCheck< T, T0, T1 >();
		}
		
		template <typename T0, typename T1, typename T2>
		explicit
		TVec3 (T0, T1, T2)
		{
			ZeroMem( *this );
			TypeConversionCheck< T, T0, T1, T2 >();
		}
		
		TVec3 (const Self &)				{ ZeroMem( *this ); }

		__VEC_OPERATORS__
	};
	

	template <typename T>
	struct TVec4
	{
	public:
		typedef TVec4<T>	Self;
		typedef bool		_is_vector_t;
		typedef T			value_t;
		enum { COUNT = 4 };

		__VEC_SWIZZLE_TYPES__

	public:
		T	x, y, z, w;
		
		union {
			__VEC4_SWIZZLE__
		};

	public:
		TVec4 ()							{ ZeroMem( *this ); }
		
		template <typename T0>
		TVec4 (T0)
		{
			ZeroMem( *this );
			TypeConversionCheck< T, T0 >();
		}
		
		template <typename T0, typename T1>
		explicit
		TVec4 (T0, T1)
		{
			ZeroMem( *this );
			TypeConversionCheck< T, T0, T1 >();
		}
		
		template <typename T0, typename T1, typename T2>
		explicit
		TVec4 (T0, T1, T2)
		{
			ZeroMem( *this );
			TypeConversionCheck< T, T0, T1, T2 >();
		}
		
		template <typename T0, typename T1, typename T2, typename T3>
		explicit
		TVec4 (T0, T1, T2, T3)
		{
			ZeroMem( *this );
			TypeConversionCheck< T, T0, T1, T2, T3 >();
		}

		TVec4 (const Self &)				{ ZeroMem( *this ); }

		__VEC_OPERATORS__
	};
	

	template <typename T>
	struct TVec8
	{
	public:
		typedef TVec8<T>	Self;
		typedef bool		_is_vector_t;
		typedef T			value_t;
		enum { COUNT = 8 };

	public:
		T	a[COUNT];

	public:
		TVec8 ()								{ ZeroMem( *this ); }
		
		template <typename T0>
		TVec8 (T0)
		{
			ZeroMem( *this );
			TypeConversionCheck< T, T0 >();
		}
		
		template <typename T0, typename T1, typename T2, typename T3,
				  typename T4, typename T5, typename T6, typename T7>
		explicit
		TVec8 (T0, T1, T2, T3, T4, T5, T6, T7)
		{
			ZeroMem( *this );
			TypeConversionCheck< T, T0, T1, T2, T3, T4, T5, T6, T7 >();
		}

		TVec8 (const Self &)					{ ZeroMem( *this ); }

		__VEC_OPERATORS__
	};
	

	template <typename T>
	struct TVec16
	{
	public:
		typedef TVec16<T>	Self;
		typedef bool		_is_vector_t;
		typedef T			value_t;
		enum { COUNT = 16 };

	public:
		T	a[COUNT];

	public:
		TVec16 ()													{ ZeroMem( *this ); }
		
		template <typename T0>
		TVec16 (T0)
		{
			ZeroMem( *this );
			TypeConversionCheck< T, T0 >();
		}
		
		template <typename T0, typename T1, typename T2, typename T3,
				  typename T4, typename T5, typename T6, typename T7,
				  typename T8, typename T9, typename TA, typename TB,
				  typename TC, typename TD, typename TE, typename TF>
		explicit
		TVec16 (T0, T1, T2, T3, T4, T5, T6, T7,
				T8, T9, TA, TB, TC, TD, TE, TF)
		{
			ZeroMem( *this );
			TypeConversionCheck< T, T0, T1, T2, T3, T4, T5, T6, T7, T8, T9, TA, TB, TC, TD, TE, TF >();
		}

		TVec16 (const Self &)										{ ZeroMem( *this ); }

		__VEC_OPERATORS__
	};

#	undef VFOR


}	// _cl_


//-----------------------------------------------------------------------------
// Base Types //

	// long
#	undef long
	typedef signed long long	longint;
	typedef unsigned long long	ulongint;
#	define long					longint
#	define long2				longint2
#	define long3				longint3
#	define long4				longint4
#	define long8				longint8
#	define long16				longint16
#	define convert_long			convert_longint
#	define convert_long2		convert_longint
#	define convert_long3		convert_longint
#	define convert_long4		convert_longint
#	define convert_long8		convert_longint
#	define convert_long16		convert_longint
#	define convert_long_sat		convert_longint
#	define convert_long2_sat	convert_longint
#	define convert_long3_sat	convert_longint
#	define convert_long4_sat	convert_longint
#	define convert_long8_sat	convert_longint
#	define convert_long16_sat	convert_longint
#	define convert_long_rte		convert_longint
#	define convert_long2_rte	convert_longint
#	define convert_long3_rte	convert_longint
#	define convert_long4_rte	convert_longint
#	define convert_long8_rte	convert_longint
#	define convert_long16_rte	convert_longint
#	define convert_long_rtz		convert_longint
#	define convert_long2_rtz	convert_longint
#	define convert_long3_rtz	convert_longint
#	define convert_long4_rtz	convert_longint
#	define convert_long8_rtz	convert_longint
#	define convert_long16_rtz	convert_longint
#	define convert_long_rtp		convert_longint
#	define convert_long2_rtp	convert_longint
#	define convert_long3_rtp	convert_longint
#	define convert_long4_rtp	convert_longint
#	define convert_long8_rtp	convert_longint
#	define convert_long16_rtp	convert_longint
#	define convert_long_rtn		convert_longint
#	define convert_long2_rtn	convert_longint
#	define convert_long3_rtn	convert_longint
#	define convert_long4_rtn	convert_longint
#	define convert_long8_rtn	convert_longint
#	define convert_long16_rtn	convert_longint
#	define as_long				as_longint
#	define as_long2				as_longint2
#	define as_long3				as_longint3
#	define as_long4				as_longint4
#	define as_long8				as_longint8
#	define as_long16			as_longint16

#	define __VEC_TYPES( _type_ ) \
	typedef	_cl_::TVec2<  _type_ >		_type_##2; \
	typedef	_cl_::TVec3<  _type_ >		_type_##3; \
	typedef	_cl_::TVec4<  _type_ >		_type_##4; \
	typedef	_cl_::TVec8<  _type_ >		_type_##8; \
	typedef	_cl_::TVec16< _type_ >		_type_##16;

	
	// typedef ...						bool;
	
	// typedef ...						char;
	__VEC_TYPES( char )

	typedef unsigned char				uchar;
	__VEC_TYPES( uchar )
	
	// typedef ...						short;
	__VEC_TYPES( short )
	
	typedef unsigned short				ushort;
	__VEC_TYPES( ushort )

	typedef unsigned int				uint;
	__VEC_TYPES( uint )

	// typedef ...						int;
	__VEC_TYPES( int )
	
	typedef unsigned int				uint;
	__VEC_TYPES( uint )

	// typedef ...						long
	__VEC_TYPES( long )

	typedef ulongint					ulong;
	__VEC_TYPES( ulong )

	// typedef ...						float;
	__VEC_TYPES( float )

	// typedef ...						double;
	__VEC_TYPES( double )
	
#	undef __VEC_TYPES


#	if CL_DEVICE_ADDRESS_BITS == 32
	typedef unsigned int				size_t;
	typedef unsigned int				ptrdiff_t;
	typedef signed int					intptr_t;
	typedef unsigned int				uintptr_t;
#	elif CL_DEVICE_ADDRESS_BITS == 64
	typedef uint64						size_t;
	typedef uint64						ptrdiff_t;
	typedef int64						intptr_t;
	typedef uint64						uintptr_t;
#	else
#	error invalid value in CL_DEVICE_ADDRESS_BITS
#	endif

	static_assert( sizeof(intptr_t) == sizeof(void *),  "" );
	static_assert( sizeof(uintptr_t) == sizeof(void *), "" );


	// Sampler
	enum sampler_t
	{
		CLK_NORMALIZED_COORDS_TRUE,
		CLK_NORMALIZED_COORDS_FALSE,
		CLK_ADDRESS_MIRRORED_REPEAT,
		CLK_ADDRESS_REPEAT,
		CLK_ADDRESS_CLAMP_TO_EDGE,
		CLK_ADDRESS_CLAMP,
		CLK_ADDRESS_NONE,
		CLK_FILTER_NEAREST,
		CLK_FILTER_LINEAR,
	};

	inline sampler_t operator | (sampler_t left, sampler_t right) {
		return sampler_t( int(left) | int(right) );
	}


	// Image
	typedef struct __image2d_t *	image2d_t;
	typedef struct __image3d_t *	image3d_t;


	// Event
	typedef struct __event_t *		event_t;


	// Image Format
	enum cl_channel_order
	{
		CLK_R,
		CLK_Rx,
		CLK_A,
		CLK_RG,
		CLK_RGx,
		CLK_RA,
		CLK_RGBA,
		
		// if channel data type = CL_UNORM_SHORT_565, CL_UNORM_SHORT_555 or CL_UNORM_INT101010.
		CLK_RGB,
		CLK_RGBx,
		
		// if channel data type = CL_UNORM_INT8, CL_SNORM_INT8, CL_SIGNED_INT8 or CL_UNSIGNED_INT8.
		CLK_ARGB,
		CLK_BGRA,

		// if channel data type = CL_UNORM_INT8, CL_UNORM_INT16, CL_SNORM_INT8, CL_SNORM_INT16, CL_HALF_FLOAT, or CL_FLOAT.
		CLK_INTENSITY,
		CLK_LUMINANCE,
	};

	enum cl_channel_type
	{
		CLK_SNORM_INT8,
		CLK_SNORM_INT16,
		CLK_UNORM_INT8,
		CLK_UNORM_INT16,
		CLK_UNORM_SHORT_565,
		CLK_UNORM_SHORT_555,
		CLK_UNORM_INT_101010,
		CLK_SIGNED_INT8,
		CLK_SIGNED_INT16,
		CLK_SIGNED_INT32,
		CLK_UNSIGNED_INT8,
		CLK_UNSIGNED_INT16,
		CLK_UNSIGNED_INT32,
		CLK_HALF_FLOAT,
		CLK_FLOAT,
	};

	typedef struct _cl_image_format {
		cl_channel_order image_channel_order;
		cl_channel_type image_channel_data_type;
	} cl_image_format;


	// Other
	enum cl_mem_fence_flags
	{
		CLK_LOCAL_MEM_FENCE,
		CLK_GLOBAL_MEM_FENCE
	};


//-----------------------------------------------------------------------------
// Qualificators //

	// reuse const
	// reuse volatile
#	define restrict			__restrict

#	define __constant		const
#	define constant			__constant

#	define __local			
#	define local			__local

#	define __global			
#	define global			__global

#	define	__private
#	define private			__private

#	define __kernel
#	define kernel			__kernel
	
#	define __read_only		
#	define read_only		__read_only

#	define __write_only
#	define write_only		__write_only

#	define __attribute__( _x_ )


	
//-----------------------------------------------------------------------------
// Helpers Functions //

namespace _cl_
{
	template <typename T0, typename T1>
	struct IsSameTypes {
		enum { value = 0 };
	};

	template <typename T>
	struct IsSameTypes <T,T> {
		enum { value = 1 };
	};

	template <typename T>
	struct IsScalar {
		enum { value =	IsSameTypes< T, bool >::value |
						IsSameTypes< T, char >::value | IsSameTypes< T, uchar >::value |
						IsSameTypes< T, short >::value | IsSameTypes< T, ushort >::value |
						IsSameTypes< T, int >::value | IsSameTypes< T, uint >::value |
						IsSameTypes< T, long >::value | IsSameTypes< T, ulong >::value |
						IsSameTypes< T, float >::value | IsSameTypes< T, double >::value };
	};

	template <typename T>
	struct IsInteger {
		enum { value =	IsSameTypes< T, bool >::value |
						IsSameTypes< T, char >::value | IsSameTypes< T, uchar >::value |
						IsSameTypes< T, short >::value | IsSameTypes< T, ushort >::value |
						IsSameTypes< T, int >::value | IsSameTypes< T, uint >::value |
						IsSameTypes< T, long >::value | IsSameTypes< T, ulong >::value };
	};

	template <typename T>
	struct IsFloat {
		enum { value = IsSameTypes< T, float >::value | IsSameTypes< T, double >::value };
	};

	template <typename T>
	struct IsVector {
		typedef T			type;
		typedef T			element_t;
		enum {
			value = 0,
			count = 1,
		};
	};

	template <typename T>
	struct IsVector< TVec2<T> > {
		typedef TVec2<T>	type;
		typedef T			element_t;
		enum {
			value = 1,
			count = type::COUNT,
		};
	};

	template <typename T>
	struct IsVector< TVec3<T> > {
		typedef TVec3<T>	type;
		typedef T			element_t;
		enum {
			value = 1,
			count = type::COUNT,
		};
	};

	template <typename T>
	struct IsVector< TVec4<T> > {
		typedef TVec4<T>	type;
		typedef T			element_t;
		enum {
			value = 1,
			count = type::COUNT,
		};
	};

	template <typename T>
	struct IsVector< TVec8<T> > {
		typedef TVec8<T>	type;
		typedef T			element_t;
		enum {
			value = 1,
			count = type::COUNT,
		};
	};

	template <typename T>
	struct IsVector< TVec16<T> > {
		typedef TVec16<T>	type;
		typedef T			element_t;
		enum {
			value = 1,
			count = type::COUNT,
		};
	};

	template <typename T>
	struct ToBaseType {
		typedef typename IsVector<T>::element_t		type;
	};

	template <typename T>	struct ToInteger {};
	template <>				struct ToInteger< char >		{ typedef uchar		unsigned_t;		typedef char	signed_t; };
	template <>				struct ToInteger< uchar >		{ typedef uchar		unsigned_t;		typedef char	signed_t; };
	template <>				struct ToInteger< short >		{ typedef ushort	unsigned_t;		typedef short	signed_t; };
	template <>				struct ToInteger< ushort >		{ typedef ushort	unsigned_t;		typedef short	signed_t; };
	template <>				struct ToInteger< int >			{ typedef uint		unsigned_t;		typedef int		signed_t; };
	template <>				struct ToInteger< uint >		{ typedef uint		unsigned_t;		typedef int		signed_t; };
	template <>				struct ToInteger< long >		{ typedef ulong		unsigned_t;		typedef long	signed_t; };
	template <>				struct ToInteger< ulong >		{ typedef ulong		unsigned_t;		typedef long	signed_t; };
	template <>				struct ToInteger< float >		{ typedef uint		unsigned_t;		typedef int		signed_t; };
	template <>				struct ToInteger< double >		{ typedef ulong		unsigned_t;		typedef long	signed_t; };

	template <typename T>
	struct ToInteger< TVec2<T> >	{
		typedef TVec2< typename ToInteger<T>::unsigned_t >	unsigned_t;
		typedef TVec2< typename ToInteger<T>::signed_t >	signed_t;
	};

	template <typename T>
	struct ToInteger< TVec3<T> >	{
		typedef TVec3< typename ToInteger<T>::unsigned_t >	unsigned_t;
		typedef TVec3< typename ToInteger<T>::signed_t >	signed_t;
	};

	template <typename T>
	struct ToInteger< TVec4<T> >	{
		typedef TVec4< typename ToInteger<T>::unsigned_t >	unsigned_t;
		typedef TVec4< typename ToInteger<T>::signed_t >	signed_t;
	};

	template <typename T>
	struct ToInteger< TVec8<T> >	{
		typedef TVec8< typename ToInteger<T>::unsigned_t >	unsigned_t;
		typedef TVec8< typename ToInteger<T>::signed_t >	signed_t;
	};

	template <typename T>
	struct ToInteger< TVec16<T> >	{
		typedef TVec16< typename ToInteger<T>::unsigned_t >	unsigned_t;
		typedef TVec16< typename ToInteger<T>::signed_t >	signed_t;
	};

	template <typename T, int I>	struct ToVec		{};
	template <typename T>			struct ToVec<T,1>	{ typedef T			type;  static_assert(IsScalar<T>::value != 0, "type must be scalar"); };
	template <typename T>			struct ToVec<T,2>	{ typedef TVec2<T>	type; };
	template <typename T>			struct ToVec<T,3>	{ typedef TVec3<T>	type; };
	template <typename T>			struct ToVec<T,4>	{ typedef TVec4<T>	type; };
	template <typename T>			struct ToVec<T,8>	{ typedef TVec8<T>	type; };
	template <typename T>			struct ToVec<T,16>	{ typedef TVec16<T>	type; };

	template <typename T>			struct VecSize		{ enum { value = IsVector<T>::count }; };

	template <typename From, typename To>	struct ToVecFrom	{ typedef typename ToVec< To, VecSize< From >::value >::type  type; };


	template <typename gentype>
	inline void Check_IsScalarOrVector ()
	{
		static_assert( (IsScalar<gentype>::value | IsVector<gentype>::value) != 0, "type must be scalar or vector" );
	}

	template <typename gentype>
	inline void Check_IsAnyScalar ()
	{
		static_assert( IsScalar<gentype>::value != 0, "type must be scalar" );
	}

	template <typename gentype>
	inline void Check_IsAnyVector ()
	{
		static_assert( IsVector<gentype>::value != 0, "type must be vector" );
	}

	template <typename gentype>
	inline void Check_IsAnyFloat ()
	{
		typedef ToBaseType< gentype >::type		base_t;
		static_assert( IsFloat< base_t >::value != 0, "type must be float" );
	}

	template <typename gentype>
	inline void Check_IsSingleFloat ()
	{
		typedef ToBaseType< gentype >::type		base_t;
		static_assert( IsSameTypes< base_t, float >::value != 0, "type must be single float" );
	}

	template <typename gentype>
	inline void Check_IsDoubleFloat ()
	{
		typedef ToBaseType< gentype >::type		base_t;
		static_assert( IsSameTypes< base_t, double >::value != 0, "type must be double float" );
	}

	template <typename gentype>
	inline void Check_IsAnyFloatScalarOrVector ()
	{
		Check_IsAnyFloat< gentype >();
		Check_IsScalarOrVector< gentype >();
	}

	template <typename gentype>
	inline void Check_IsAnyInteger ()
	{
		typedef ToBaseType< gentype >::type		base_t;
		static_assert( IsInteger< base_t >::value != 0, "type must be integer" );
	}

	template <typename gentype>
	inline void Check_IsAnyIntegerScalarOrVector ()
	{
		Check_IsAnyInteger< gentype >();
		Check_IsScalarOrVector< gentype >();
	}

	template <typename T0, typename T1>
	inline void Check_IsAnyIntegerOrAnyFloat ()
	{
		typedef ToBaseType< T0 >::type		base0_t;
		typedef ToBaseType< T1 >::type		base1_t;
		enum { is_integer_or_float =	( IsInteger< base0_t >::value & IsInteger< base1_t >::value ) |
										( IsFloat< base0_t >::value & IsFloat< base1_t >::value ) };
		static_assert( is_integer_or_float != 0, "all types must be integers or floats" );
	}

	template <typename T0, typename T1>
	inline void Check_IsAnyIntegerOrAnyFloatScalarOrVector ()
	{
		Check_IsAnyIntegerOrAnyFloat< T0, T1 >();
		Check_IsScalarOrVector< T0 >();
		Check_IsScalarOrVector< T1 >();
		
		// countof(T0) == countof(T1) or countof(T1) == 1
		static_assert( (VecSize<T0>::value == VecSize<T1>::value) or VecSize<T1>::value == 1, "invalid type" );
	}

}	// _cl_


//-----------------------------------------------------------------------------
// Functions //


	// <type><size>  convert_<type><size> (<type1><size>)
	// <type><size>  convert_<type><size>_<round> (<type2><size>)
#	define __CONVERT_TO_TYPE( _type1_, _type2_, _size_, _postfix_ ) \
	inline _type1_##_size_ convert_##_type1_##_size_##_postfix_ (_type2_##_size_)	{ return _type1_##_size_(); }
#	define __CONVERT_TO_TYPE1( _type_, _size_, _postfix_ ) \
	__CONVERT_TO_TYPE( _type_, char, _size_, _postfix_ ) \
	__CONVERT_TO_TYPE( _type_, uchar, _size_, _postfix_ ) \
	__CONVERT_TO_TYPE( _type_, short, _size_, _postfix_ ) \
	__CONVERT_TO_TYPE( _type_, ushort, _size_, _postfix_ ) \
	__CONVERT_TO_TYPE( _type_, int, _size_, _postfix_ ) \
	__CONVERT_TO_TYPE( _type_, uint, _size_, _postfix_ ) \
	__CONVERT_TO_TYPE( _type_, long, _size_, _postfix_ ) \
	__CONVERT_TO_TYPE( _type_, ulong, _size_, _postfix_ ) \
	__CONVERT_TO_TYPE( _type_, float, _size_, _postfix_ ) \
	__CONVERT_TO_TYPE( _type_, double, _size_, _postfix_ )
#	define __CONVERT_TO_TYPE2( _type_, _size_ ) \
	__CONVERT_TO_TYPE1( _type_, _size_, ) \
	__CONVERT_TO_TYPE1( _type_, _size_, _sat ) \
	__CONVERT_TO_TYPE1( _type_, _size_, _rte ) \
	__CONVERT_TO_TYPE1( _type_, _size_, _rtz ) \
	__CONVERT_TO_TYPE1( _type_, _size_, _rtp ) \
	__CONVERT_TO_TYPE1( _type_, _size_, _rtn )
#	define __CONVERT_TO_TYPE3( _type_ ) \
	__CONVERT_TO_TYPE2( _type_, ) \
	__CONVERT_TO_TYPE2( _type_, 2 ) \
	__CONVERT_TO_TYPE2( _type_, 3 ) \
	__CONVERT_TO_TYPE2( _type_, 4 ) \
	__CONVERT_TO_TYPE2( _type_, 8 ) \
	__CONVERT_TO_TYPE2( _type_, 16 )
	__CONVERT_TO_TYPE3( char )
	__CONVERT_TO_TYPE3( uchar )
	__CONVERT_TO_TYPE3( short )
	__CONVERT_TO_TYPE3( ushort )
	__CONVERT_TO_TYPE3( int )
	__CONVERT_TO_TYPE3( uint )
	__CONVERT_TO_TYPE3( long )
	__CONVERT_TO_TYPE3( ulong )
	__CONVERT_TO_TYPE3( float )
	__CONVERT_TO_TYPE3( double )
#	undef __CONVERT_TO_TYPE3
#	undef __CONVERT_TO_TYPE2
#	undef __CONVERT_TO_TYPE1
#	undef __CONVERT_TO_TYPE
	

	// <type><size>  as_<type><size> (<type1><size1>)
#	define __AS_TYPE( _type1_, _type2_ ) \
	inline _type1_ as_##_type1_ (_type2_)  { /*static_assert( sizeof(_type1_) == sizeof(_type2_), "types size mismatch" );*/  return _type1_(); }
#	define __AS_TYPE1( _type1_, _type2_ ) \
	__AS_TYPE( _type1_,		_type2_ ) \
	__AS_TYPE( _type1_##2,	_type2_##2 ) \
	__AS_TYPE( _type1_##3,	_type2_##3 ) \
	__AS_TYPE( _type1_##3,	_type2_##4 ) \
	__AS_TYPE( _type1_##4,	_type2_##3 ) \
	__AS_TYPE( _type1_##4,	_type2_##4 ) \
	__AS_TYPE( _type1_##8,	_type2_##8 ) \
	__AS_TYPE( _type1_##16,	_type2_##16 )
#	define __AS_TYPE1x2( _type1_, _type2_ ) \
	__AS_TYPE( _type1_,		_type2_##2 ) \
	__AS_TYPE( _type1_##2,	_type2_##4 ) \
	__AS_TYPE( _type1_##4,	_type2_##8 ) \
	__AS_TYPE( _type1_##8,	_type2_##16 ) \
	__AS_TYPE( _type2_##2,	_type1_ ) \
	__AS_TYPE( _type2_##4,	_type1_##2 ) \
	__AS_TYPE( _type2_##8,	_type1_##4 ) \
	__AS_TYPE( _type2_##16,	_type1_##8 )
#	define __AS_TYPE1x4( _type1_, _type2_ ) \
	__AS_TYPE( _type1_,		_type2_##4 ) \
	__AS_TYPE( _type1_##2,	_type2_##8 ) \
	__AS_TYPE( _type1_##4,	_type2_##16 ) \
	__AS_TYPE( _type2_##4,	_type1_ ) \
	__AS_TYPE( _type2_##8,	_type1_##2 ) \
	__AS_TYPE( _type2_##16,	_type1_##4 )
#	define __AS_TYPE2( _type1_, _type2_ ) \
	__AS_TYPE1( _type1_, _type2_ ) \
	__AS_TYPE1( _type2_, _type1_ ) \
	__AS_TYPE1x2( _type1_, _type2_ ) \
	__AS_TYPE1x4( _type1_, _type2_ )
	__AS_TYPE2( char,	uchar )
	__AS_TYPE2( short,	ushort )
	__AS_TYPE2( int,	uint )
	__AS_TYPE2( float,	int )
	__AS_TYPE2( float,	uint )
	__AS_TYPE2( long,	ulong )
	__AS_TYPE2( double,	long )
	__AS_TYPE2( double,	ulong )
#	undef __AS_TYPE2
#	undef __AS_TYPE1
#	undef __AS_TYPE


	// event_t async_work_group_copy (__local gentype *dst, const __global gentype *src, size_t  num_gentypes, event_t event)
	// event_t async_work_group_copy (__global gentype *dst, const __local gentype *src, size_t num_gentypes, event_t event)
	
	template <typename gentype> inline
	event_t async_work_group_copy (gentype *dst, const gentype *src, size_t  num_gentypes, event_t event)
	{
		_cl_::Check_IsScalarOrVector< gentype >();
		return event_t();
	}

	// event_t async_work_group_strided_copy (__local gentype *dst, const __global gentype *src, size_t num_gentypes, size_t src_stride, event_t event)
	// event_t async_work_group_strided_copy (__global gentype *dst, const __local gentype *src, size_t num_gentypes, size_t dst_stride, event_t event)
	
	template <typename gentype> inline
	event_t async_work_group_strided_copy (gentype *dst, const gentype *src, size_t num_gentypes, size_t dst_stride, event_t event)
	{
		_cl_::Check_IsScalarOrVector< gentype >();
		return event_t();
	}
	
	inline
	void wait_group_events (int num_events, event_t *event_list)
	{}

	template <typename gentype> inline
	void prefetch (const __global gentype *p, size_t num_gentypes)
	{
		_cl_::Check_IsScalarOrVector< gentype >();
	}

	// gentype clamp (gentype x, gentype minval, gentype maxval)
	// gentype clamp (gentype x, float minval, float maxval)
	// gentype clamp (gentype x, double minval, double maxval)
	template <typename gentype, typename gentype2> inline
	gentype clamp (gentype x, gentype2 minval, gentype2 maxval)
	{
		_cl_::Check_IsAnyIntegerOrAnyFloatScalarOrVector< gentype, gentype2 >();
		return x;
	}
	
	template <typename gentype> inline
	gentype degrees (gentype radians)
	{
		_cl_::Check_IsAnyFloatScalarOrVector< gentype >();
		return radians;
	}

	// gentype max (gentype x, gentype y)
	// gentype max (gentype x, float y)
	// gentype max (gentype x, double y)
	template <typename gentype, typename gentype2> inline
	gentype max (gentype x, gentype2 y)
	{
		_cl_::Check_IsAnyIntegerOrAnyFloatScalarOrVector< gentype, gentype2 >();
		return x;
	}

	// gentype min (gentype x, gentype y)
	// gentype min (gentype x, float y)
	// gentype min (gentype x, double y)
	template <typename gentype, typename gentype2> inline
	gentype min (gentype x, gentype2 y)
	{
		_cl_::Check_IsAnyIntegerOrAnyFloatScalarOrVector< gentype, gentype2 >();
		return x;
	}

	// gentype mix (gentype x, gentype y, gentype a)
	// gentype mix (gentype x, gentype y, float a)
	// gentype mix (gentype x, gentype y, double a)
	template <typename gentype, typename gentype2> inline
	gentype mix (gentype x, gentype y, gentype2 a)
	{
		_cl_::Check_IsAnyFloatScalarOrVector< gentype >();
		_cl_::Check_IsAnyFloatScalarOrVector< gentype2 >();
		return x;
	}
	
	template <typename gentype> inline
	gentype radians (gentype degrees)
	{
		_cl_::Check_IsAnyFloatScalarOrVector< gentype >();
		return radians;
	}
	
	template <typename gentype> inline
	gentype sign (gentype x)
	{
		_cl_::Check_IsAnyFloatScalarOrVector< gentype >();
		return radians;
	}

	// gentype smoothstep (gentype edge0, gentype edge1, gentype x)
	// gentype smoothstep (float edge0, float edge1, gentype x)
	// gentype smoothstep (double edge0, double edge1, gentype x)
	template <typename gentype, typename gentype2> inline
	gentype smoothstep (gentype edge0, gentype2 edge1, gentype x)
	{
		_cl_::Check_IsAnyFloatScalarOrVector< gentype >();
		_cl_::Check_IsAnyFloatScalarOrVector< gentype2 >();
		return x;
	}

	// gentype step (gentype edge, gentype x)
	// gentype step (float edge, gentype x)
	// gentype step (double edge, gentype x)
	template <typename gentype, typename gentype2> inline
	gentype step (gentype2 edge, gentype x)
	{
		_cl_::Check_IsAnyFloatScalarOrVector< gentype >();
		_cl_::Check_IsAnyFloatScalarOrVector< gentype2 >();
		return x;
	}

	inline
	void mem_fence (cl_mem_fence_flags flags)
	{}

	inline
	void read_mem_fence (cl_mem_fence_flags flags)
	{}

	inline
	void write_mem_fence (cl_mem_fence_flags flags)
	{}

	inline float4  cross (float4 p0, float4 p1)		{ return p0; }
	inline float3  cross (float3 p0, float3 p1)		{ return p0; }
	inline double4 cross (double4 p0, double4 p1)	{ return p0; }
	inline double3 cross (double3 p0, double3 p1)	{ return p0; }
	
	// float dot (floatn p0, floatn p1)
	// double dot (gentype p0, gentype p1)
	template <typename gentype> inline
	typename _cl_::ToBaseType< gentype >::type dot (const gentype p0, const gentype p1)
	{
		_cl_::Check_IsAnyFloatScalarOrVector< gentype >();
		return typename _cl_::ToBaseType< gentype >::type();
	}

	// float distance (floatn p0, floatn p1)
	// double distance (gentype p0, gentype p1)
	template <typename gentype> inline
	typename _cl_::ToBaseType< gentype >::type distance (gentype p0, gentype p1)
	{
		_cl_::Check_IsAnyFloatScalarOrVector< gentype >();
		return typename _cl_::ToBaseType< gentype >::type();
	}

	// float length (floatn p)
	// double length (gentype p)
	template <typename gentype> inline
	typename _cl_::ToBaseType< gentype >::type length (gentype p)
	{
		_cl_::Check_IsAnyFloatScalarOrVector< gentype >();
		return typename _cl_::ToBaseType< gentype >::type();
	}
	
	template <typename gentype> inline
	gentype normalize (gentype p)
	{
		_cl_::Check_IsAnyFloatScalarOrVector< gentype >();
		return p;
	}

	// float fast_distance (floatn p0, floatn p1)
	template <typename gentype> inline
	typename _cl_::ToBaseType< gentype >::type fast_distance (gentype p0, gentype p1)
	{
		_cl_::Check_IsScalarOrVector< gentype >();
		_cl_::Check_IsSingleFloat< gentype >();
		return p;
	}

	// float fast_length (floatn p)
	template <typename gentype> inline
	typename _cl_::ToBaseType< gentype >::type fast_length (gentype p)
	{
		_cl_::Check_IsScalarOrVector< gentype >();
		_cl_::Check_IsSingleFloat< gentype >();
		return p;
	}

	// floatn fast_normalize (floatn p)
	template <typename gentype> inline
	gentype fast_length (gentype p)
	{
		_cl_::Check_IsScalarOrVector< gentype >();
		_cl_::Check_IsSingleFloat< gentype >();
		return p;
	}

	inline float4 read_imagef (image2d_t image, sampler_t sampler, int2 coord)		{ return float4(); }
	inline float4 read_imagef (image2d_t image, sampler_t sampler, float2 coord)	{ return float4(); }

	inline float4 read_imagef (image3d_t image, sampler_t sampler, int4 coord)		{ return float4(); }
	inline float4 read_imagef (image3d_t image, sampler_t sampler, float4 coord)	{ return float4(); }

	inline int4 read_imagei (image2d_t image, sampler_t sampler, int2 coord)		{ return int4(); }
	inline int4 read_imagei (image2d_t image, sampler_t sampler, float2 coord)		{ return int4(); }
	
	inline uint4 read_imageui (image2d_t image, sampler_t sampler, int2 coord)		{ return uint4(); }
	inline uint4 read_imageui (image2d_t image, sampler_t sampler, float2 coord)	{ return uint4(); }

	inline int4 read_imagei (image3d_t image, sampler_t sampler, int4 coord)		{ return int4(); }
	inline int4 read_imagei (image3d_t image, sampler_t sampler, float4 coord)		{ return int4(); }

	inline uint4 read_imageui (image3d_t image, sampler_t sampler, int4 coord)		{ return uint4(); }
	inline uint4 read_imageui (image3d_t image, sampler_t sampler, float4 coord)	{ return uint4(); }
	
	inline void write_imagef (image2d_t image, int2 coord, float4 color)			{}
	inline void write_imagei (image2d_t image, int2 coord, int4 color)				{}
	inline void write_imageui (image2d_t image, int2 coord, uint4 color)			{}
	
	inline void write_imagef (image3d_t image, int4 coord, float4 color)			{}
	inline void write_imagei (image3d_t image, int4 coord, int4 color)				{}
	inline void write_imageui (image3d_t image, int4 coord, uint4 color)			{}

	inline int get_image_width (image2d_t image)									{ return int(); }
	inline int get_image_width (image3d_t image)									{ return int(); }

	inline int get_image_height (image2d_t image)									{ return int(); }
	inline int get_image_height (image3d_t image)									{ return int(); }
	
	inline int get_image_depth (image2d_t image)									{ return int(); }
	inline int get_image_depth (image3d_t image)									{ return int(); }
	
	inline cl_channel_type get_image_channel_data_type (image2d_t image)			{ return cl_channel_type(); }
	inline cl_channel_type get_image_channel_data_type (image3d_t image)			{ return cl_channel_type(); }
	
	inline cl_channel_order get_image_channel_order (image2d_t image)				{ return cl_channel_order(); }
	inline cl_channel_order get_image_channel_order (image3d_t image)				{ return cl_channel_order(); }
	
	inline int2 get_image_dim (image2d_t image)										{ return int2(); }
	inline int4 get_image_dim (image3d_t image)										{ return int4(); }
	
	// ugentype abs (gentype x)
	template <typename gentype> inline
	typename _cl_::ToInteger< gentype >::unsigned_t  abs (gentype x)
	{
		_cl_::Check_IsAnyIntegerScalarOrVector< gentype >();
		return typename _cl_::ToInteger< gentype >::unsigned_t();
	}
	
	// ugentype abs_diff (gentype x, gentype y)
	template <typename gentype> inline
	typename _cl_::ToInteger< gentype >::unsigned_t  abs_diff (gentype x, gentype y)
	{
		_cl_::Check_IsAnyIntegerScalarOrVector< gentype >();
		return typename _cl_::ToInteger< gentype >::unsigned_t();
	}
	
	template <typename gentype> inline
	gentype add_sat (gentype x, gentype y)
	{
		_cl_::Check_IsAnyIntegerScalarOrVector< gentype >();
		return x;
	}

	// gentype clamp (gentype x, gentype minval, gentype maxval)
	// gentype clamp (gentype x, sgentype minval, sgentype maxval)
	
	template <typename gentype> inline
	gentype clz (gentype x)
	{
		_cl_::Check_IsAnyIntegerScalarOrVector< gentype >();
		return x;
	}
	
	template <typename gentype> inline
	gentype hadd (gentype x, gentype y)
	{
		_cl_::Check_IsAnyIntegerScalarOrVector< gentype >();
		return x;
	}
	
	template <typename gentype> inline
	gentype rhadd (gentype x, gentype y)
	{
		_cl_::Check_IsAnyIntegerScalarOrVector< gentype >();
		return x;
	}
	
	template <typename gentype> inline
	gentype mad24 (gentype x, gentype y, gentype z)
	{
		_cl_::Check_IsAnyIntegerScalarOrVector< gentype >();
		return x;
	}
	
	template <typename gentype> inline
	gentype mad_hi (gentype a, gentype b, gentype c)
	{
		_cl_::Check_IsAnyIntegerScalarOrVector< gentype >();
		return x;
	}
	
	template <typename gentype> inline
	gentype mad_sat (gentype a, gentype b, gentype c)
	{
		_cl_::Check_IsAnyIntegerScalarOrVector< gentype >();
		return a;
	}

	// gentype max (gentype x, gentype y)
	// gentype max (gentype x, sgentype y)
	
	// gentype min (gentype x, gentype y)
	// gentype min (gentype x, sgentype y)
	
	template <typename gentype> inline
	gentype mul24 (gentype x, gentype y)
	{
		_cl_::Check_IsAnyIntegerScalarOrVector< gentype >();
		return x;
	}
	
	template <typename gentype> inline
	gentype mul_hi (gentype x, gentype y)
	{
		_cl_::Check_IsAnyIntegerScalarOrVector< gentype >();
		return x;
	}
	
	template <typename gentype> inline
	gentype rotate (gentype v, gentype i)
	{
		_cl_::Check_IsAnyIntegerScalarOrVector< gentype >();
		return v;
	}
	
	template <typename gentype> inline
	gentype sub_sat (gentype x, gentype y)
	{
		_cl_::Check_IsAnyIntegerScalarOrVector< gentype >();
		return x;
	}

	// short upsample (char hi, uchar lo)
	// ushort upsample (uchar hi, uchar lo)
	// shortn upsample (charn hi, ucharn lo)
	// ushortn upsample (ucharn hi, ucharn lo)
	// int upsample (short hi, ushort lo)
	// uint upsample (ushort hi, ushort lo)
	// intn upsample (shortn hi, ushortn lo)
	// uintn upsample (ushortn hi, ushortn lo)
	// long upsample (int hi, uint lo)
	// ulong upsample (uint hi, uint lo)
	// longn upsample (intn hi, uintn lo)
	// ulongn upsample (uintn hi, uintn lo)
#	define __UPSAMPLE( _type1_, _utype1_, _type2_ ) \
	inline _type2_ upsample (_type1_ hi, _utype1_ lo)		{ _type2_ t;  return t; }
#	define __UPSAMPLE1( _type1_, _utype1_, _type2_ ) \
	__UPSAMPLE( _type1_,     _utype1_,     _type2_ ) \
	__UPSAMPLE( _type1_##2,  _utype1_##2,  _type2_##2 ) \
	__UPSAMPLE( _type1_##3,  _utype1_##3,  _type2_##3 ) \
	__UPSAMPLE( _type1_##4,  _utype1_##4,  _type2_##4 ) \
	__UPSAMPLE( _type1_##8,  _utype1_##8,  _type2_##8 ) \
	__UPSAMPLE( _type1_##16, _utype1_##16, _type2_##16 )
#	define __UPSAMPLE2( _type1_, _type2_ ) \
	__UPSAMPLE1( _type1_, u##_type1_, _type2_ ) \
	__UPSAMPLE1( u##_type1_, u##_type1_, u##_type2_ )
	__UPSAMPLE2( short, char )
	__UPSAMPLE2( int, short )
	__UPSAMPLE2( long, int )
#	undef __UPSAMPLE2
#	undef __UPSAMPLE1
#	undef __UPSAMPLE


	
//-----------------------------------------------------------------------------
// Math Functions //

	// Constants
	const float		MAXFLOAT		= 1.0f;
	const float		HUGE_VALF		= 1.0f;
	const double	HUGE_VAL		= 1.0;
	const float		INFINITY		= 1.0f;
	const float		NAN				= 1.0f;

	const float		M_E_F			= 1.0f;
	const float		M_LOG2E_F		= 1.0f;
	const float		M_LOG10E_F		= 1.0f;
	const float		M_LN2_F			= 1.0f;
	const float		M_LN10_F		= 1.0f;
	const float		M_PI_F			= 1.0f;
	const float		M_PI_2_F		= 1.0f;
	const float		M_PI_4_F		= 1.0f;
	const float		M_1_PI_F		= 1.0f;
	const float		M_2_PI_F		= 1.0f;
	const float		M_2_SQRTPI_F	= 1.0f;
	const float		M_SQRT2_F		= 1.0f;
	const float		M_SQRT1_2_F		= 1.0f;

	const double	M_E				= 1.0;
	const double	M_LOG2E			= 1.0;
	const double	M_LOG10E		= 1.0;
	const double	M_LN2			= 1.0;
	const double	M_LN10			= 1.0;
	const double	M_PI			= 1.0;
	const double	M_PI_2			= 1.0;
	const double	M_PI_4			= 1.0;
	const double	M_1_PI			= 1.0;
	const double	M_2_PI			= 1.0;
	const double	M_2_SQRTPI		= 1.0;
	const double	M_SQRT2			= 1.0;
	const double	M_SQRT1_2		= 1.0;

	#define FLT_DIG 				6
	#define FLT_MANT_DIG 			24
	#define FLT_MAX_10_EXP 			+38
	#define FLT_MAX_EXP 			+128
	#define FLT_MIN_10_EXP 			-37
	#define FLT_MIN_EXP 			-125
	#define FLT_RADIX 				2
	#define FLT_MAX 				0x1.fffffep127f
	#define FLT_MIN 				0x1.0p-126f
	#define FLT_EPSILON 			0x1.0p-23f
	#define CHAR_BIT 				8
	#define CHAR_MAX 				SCHAR_MAX
	#define CHAR_MIN 				SCHAR_MIN
	#define INT_MAX 				2147483647
	#define INT_MIN 				(-2147483647- 1)
	#define LONG_MAX 				0x7fffffffffffffffL
	#define LONG_MIN 				(-0x7fffffffffffffffL- 1)
	#define SCHAR_MAX 				127
	#define SCHAR_MIN 				(-127 - 1)
	#define SHRT_MAX 				32767
	#define SHRT_MIN 				(-32767- 1)
	#define UCHAR_MAX 				255
	#define USHRT_MAX 				65535
	#define UINT_MAX 				0xffffffff
	#define ULONG_MAX 				0xffffffffffffffffULL

	#define DBL_DIG 				15
	#define DBL_MANT_DIG 			53
	#define DBL_MAX_10_EXP 			+308
	#define DBL_MAX_EXP 			+1024
	#define DBL_MIN_10_EXP			-307
	#define DBL_MIN_EXP 			-1021
	#define DBL_MAX 				0x1.fffffffffffffp1023
	#define DBL_MIN 				0x1.0p-1022
	#define DBL_EPSILON 			0x1.0p-52

	
	template <typename gentype> inline
	gentype acos (gentype x)
	{
		_cl_::Check_IsAnyFloatScalarOrVector< gentype >();
		return x;
	}

	template <typename gentype> inline
	gentype acosh (gentype x)
	{
		_cl_::Check_IsAnyFloatScalarOrVector< gentype >();
		return x;
	}

	template <typename gentype> inline
	gentype acospi (gentype x)
	{
		_cl_::Check_IsAnyFloatScalarOrVector< gentype >();
		return x;
	}
	
	template <typename gentype> inline
	gentype asin (gentype x)
	{
		_cl_::Check_IsAnyFloatScalarOrVector< gentype >();
		return x;
	}
	
	template <typename gentype> inline
	gentype asinh (gentype x)
	{
		_cl_::Check_IsAnyFloatScalarOrVector< gentype >();
		return x;
	}
	
	template <typename gentype> inline
	gentype asinpi (gentype x)
	{
		_cl_::Check_IsAnyFloatScalarOrVector< gentype >();
		return x;
	}
	
	template <typename gentype> inline
	gentype atan (gentype y_over_x)
	{
		_cl_::Check_IsAnyFloatScalarOrVector< gentype >();
		return y_over_x;
	}
	
	template <typename gentype> inline
	gentype atan2 (gentype y, gentype x)
	{
		_cl_::Check_IsAnyFloatScalarOrVector< gentype >();
		return x;
	}
	
	template <typename gentype> inline
	gentype atanh (gentype x)
	{
		_cl_::Check_IsAnyFloatScalarOrVector< gentype >();
		return x;
	}
	
	template <typename gentype> inline
	gentype atanpi (gentype x)
	{
		_cl_::Check_IsAnyFloatScalarOrVector< gentype >();
		return x;
	}
	
	template <typename gentype> inline
	gentype atan2pi (gentype y, gentype x)
	{
		_cl_::Check_IsAnyFloatScalarOrVector< gentype >();
		return x;
	}
	
	template <typename gentype> inline
	gentype cbrt (gentype x)
	{
		_cl_::Check_IsAnyFloatScalarOrVector< gentype >();
		return x;
	}
	
	template <typename gentype> inline
	gentype ceil (gentype x)
	{
		_cl_::Check_IsAnyFloatScalarOrVector< gentype >();
		return x;
	}
	
	template <typename gentype> inline
	gentype copysign (gentype x, gentype y)
	{
		_cl_::Check_IsAnyFloatScalarOrVector< gentype >();
		return x;
	}
	
	template <typename gentype> inline
	gentype cos (gentype x)
	{
		_cl_::Check_IsAnyFloatScalarOrVector< gentype >();
		return x;
	}
	
	template <typename gentype> inline
	gentype cosh (gentype x)
	{
		_cl_::Check_IsAnyFloatScalarOrVector< gentype >();
		return x;
	}
	
	template <typename gentype> inline
	gentype cospi (gentype x)
	{
		_cl_::Check_IsAnyFloatScalarOrVector< gentype >();
		return x;
	}
	
	template <typename gentype> inline
	gentype half_cos (gentype x)
	{
		_cl_::Check_IsAnyFloatScalarOrVector< gentype >();
		return x;
	}
	
	template <typename gentype> inline
	gentype native_cos (gentype x)
	{
		_cl_::Check_IsAnyFloatScalarOrVector< gentype >();
		return x;
	}
	
	template <typename gentype> inline
	gentype half_divide (gentype x, gentype y)
	{
		_cl_::Check_IsAnyFloatScalarOrVector< gentype >();
		return x;
	}
	
	template <typename gentype> inline
	gentype native_divide (gentype x, gentype y)
	{
		_cl_::Check_IsAnyFloatScalarOrVector< gentype >();
		return x;
	}
	
	template <typename gentype> inline
	gentype erf (gentype x)
	{
		_cl_::Check_IsAnyFloatScalarOrVector< gentype >();
		return x;
	}
	
	template <typename gentype> inline
	gentype erfc (gentype x)
	{
		_cl_::Check_IsAnyFloatScalarOrVector< gentype >();
		return x;
	}
	
	template <typename gentype> inline
	gentype exp (gentype x)
	{
		_cl_::Check_IsAnyFloatScalarOrVector< gentype >();
		return x;
	}
	
	template <typename gentype> inline
	gentype exp2 (gentype x)
	{
		_cl_::Check_IsAnyFloatScalarOrVector< gentype >();
		return x;
	}
	
	template <typename gentype> inline
	gentype exp10 (gentype x)
	{
		_cl_::Check_IsAnyFloatScalarOrVector< gentype >();
		return x;
	}
	
	template <typename gentype> inline
	gentype expm1 (gentype x)
	{
		_cl_::Check_IsAnyFloatScalarOrVector< gentype >();
		return x;
	}
	
	template <typename gentype> inline
	gentype half_exp (gentype x)
	{
		_cl_::Check_IsAnyFloatScalarOrVector< gentype >();
		return x;
	}
	
	template <typename gentype> inline
	gentype half_exp2 (gentype x)
	{
		_cl_::Check_IsAnyFloatScalarOrVector< gentype >();
		return x;
	}
	
	template <typename gentype> inline
	gentype half_exp10 (gentype x)
	{
		_cl_::Check_IsAnyFloatScalarOrVector< gentype >();
		return x;
	}
	
	template <typename gentype> inline
	gentype native_exp (gentype x)
	{
		_cl_::Check_IsAnyFloatScalarOrVector< gentype >();
		return x;
	}
	
	template <typename gentype> inline
	gentype native_exp2 (gentype x)
	{
		_cl_::Check_IsAnyFloatScalarOrVector< gentype >();
		return x;
	}
	
	template <typename gentype> inline
	gentype native_exp10 (gentype x)
	{
		_cl_::Check_IsAnyFloatScalarOrVector< gentype >();
		return x;
	}
	
	template <typename gentype> inline
	gentype fabs (gentype x)
	{
		_cl_::Check_IsAnyFloatScalarOrVector< gentype >();
		return x;
	}
	
	template <typename gentype> inline
	gentype fdim (gentype x, gentype y)
	{
		_cl_::Check_IsAnyFloatScalarOrVector< gentype >();
		return x;
	}
	
	template <typename gentype> inline
	gentype floor (gentype x)
	{
		_cl_::Check_IsAnyFloatScalarOrVector< gentype >();
		return x;
	}
	
	template <typename gentype> inline
	gentype fma (gentype a, gentype b, gentype c)
	{
		_cl_::Check_IsAnyFloatScalarOrVector< gentype >();
		return a;
	}

	// gentype fmax (gentype x, gentype y)
	// gentype fmax (gentype x, float y)
	// gentype fmax (gentype x, double y)
	template <typename gentype, typename gentype2> inline
	gentype fmax (gentype x, gentype2 y)
	{
		_cl_::Check_IsAnyFloatScalarOrVector< gentype >();
		_cl_::Check_IsAnyFloatScalarOrVector< gentype2 >();
		return x;
	}

	// gentype fmin (gentype x, gentype y)
	// gentype fmin (gentype x, float y)
	// gentype fmin (gentype x, double y)
	template <typename gentype, typename gentype2> inline
	gentype fmin (gentype x, gentype2 y)
	{
		_cl_::Check_IsAnyFloatScalarOrVector< gentype >();
		_cl_::Check_IsAnyFloatScalarOrVector< gentype2 >();
		return x;
	}
	
	template <typename gentype> inline
	gentype fmod (gentype x, gentype y)
	{
		_cl_::Check_IsAnyFloatScalarOrVector< gentype >();
		return x;
	}

	// gentype fract (gentype x, __global gentype *itpr)
	// gentype fract (gentype x, __local gentype *itpr)
	// gentype fract (gentype x, __private gentype *itpr)
	template <typename gentype> inline
	gentype fract (gentype x, gentype *itpr)
	{
		_cl_::Check_IsAnyFloatScalarOrVector< gentype >();
		return x;
	}

	// gentype frexp (gentype x, __global intn *exp)
	// gentype frexp (gentype x, __local intn *exp)
	// gentype frexp (gentype x, __private intn *exp)
	template <typename gentype> inline
	gentype frexp (gentype x, typename _cl_::ToVecFrom< gentype, int >::type *exp)
	{
		_cl_::Check_IsAnyFloatScalarOrVector< gentype >();
		return x;
	}
	
	template <typename gentype> inline
	gentype hypot (gentype x, gentype y)
	{
		_cl_::Check_IsAnyFloatScalarOrVector< gentype >();
		return x;
	}
	
	template <typename gentype> inline
	typename _cl_::ToVecFrom< gentype, int >::type  ilogb (gentype x)
	{
		_cl_::Check_IsAnyFloatScalarOrVector< gentype >();
		return typename _cl_::ToVecFrom< gentype, int >::type();
	}
	
	template <typename gentype> inline
	gentype ldexp (gentype x, typename _cl_::ToVecFrom< gentype, int >::type n)
	{
		_cl_::Check_IsAnyFloatScalarOrVector< gentype >();
		return x;
	}
	
	template <typename gentype> inline
	gentype ldexp (gentype x, int n)
	{
		_cl_::Check_IsAnyFloatScalarOrVector< gentype >();
		return x;
	}
	
	template <typename gentype> inline
	gentype lgamma (gentype x)
	{
		_cl_::Check_IsAnyFloatScalarOrVector< gentype >();
		return x;
	}
	
	//gentype lgamma_r (gentype x, __global intn *signp)
	//gentype lgamma_r (gentype x, __local intn *signp)
	//gentype lgamma_r (gentype x, __private intn *signp)
	template <typename gentype> inline
	gentype lgamma_r (gentype x, typename _cl_::ToVecFrom< gentype, int >::type *signp)
	{
		_cl_::Check_IsAnyFloatScalarOrVector< gentype >();
		return x;
	}
	
	template <typename gentype> inline
	gentype log (gentype x)
	{
		_cl_::Check_IsAnyFloatScalarOrVector< gentype >();
		return x;
	}
	
	template <typename gentype> inline
	gentype log2 (gentype x)
	{
		_cl_::Check_IsAnyFloatScalarOrVector< gentype >();
		return x;
	}
	
	template <typename gentype> inline
	gentype log10 (gentype x)
	{
		_cl_::Check_IsAnyFloatScalarOrVector< gentype >();
		return x;
	}
	
	template <typename gentype> inline
	gentype log1p (gentype x)
	{
		_cl_::Check_IsAnyFloatScalarOrVector< gentype >();
		return x;
	}
	
	template <typename gentype> inline
	gentype logb (gentype x)
	{
		_cl_::Check_IsAnyFloatScalarOrVector< gentype >();
		return x;
	}
	
	template <typename gentype> inline
	gentype half_log (gentype x)
	{
		_cl_::Check_IsAnyFloatScalarOrVector< gentype >();
		return x;
	}
	
	template <typename gentype> inline
	gentype half_log2 (gentype x)
	{
		_cl_::Check_IsAnyFloatScalarOrVector< gentype >();
		return x;
	}
	
	template <typename gentype> inline
	gentype half_log10 (gentype x)
	{
		_cl_::Check_IsAnyFloatScalarOrVector< gentype >();
		return x;
	}
	
	template <typename gentype> inline
	gentype native_log (gentype x)
	{
		_cl_::Check_IsAnyFloatScalarOrVector< gentype >();
		return x;
	}
	
	template <typename gentype> inline
	gentype native_log2 (gentype x)
	{
		_cl_::Check_IsAnyFloatScalarOrVector< gentype >();
		return x;
	}
	
	template <typename gentype> inline
	gentype native_log10 (gentype x)
	{
		_cl_::Check_IsAnyFloatScalarOrVector< gentype >();
		return x;
	}
	
	template <typename gentype> inline
	gentype mad (gentype a, gentype b, gentype c)
	{
		_cl_::Check_IsAnyFloatScalarOrVector< gentype >();
		return a;
	}
	
	template <typename gentype> inline
	gentype maxmag (gentype x, gentype y)
	{
		_cl_::Check_IsAnyFloatScalarOrVector< gentype >();
		return x;
	}
	
	template <typename gentype> inline
	gentype minmag (gentype x, gentype y)
	{
		_cl_::Check_IsAnyFloatScalarOrVector< gentype >();
		return x;
	}

	// gentype modf (gentype x, __global gentype *iptr)
	// gentype modf (gentype x, __local gentype *iptr)
	// gentype modf (gentype x, __private gentype *iptr)
	template <typename gentype> inline
	gentype modf (gentype x, gentype *iptr)
	{
		_cl_::Check_IsAnyFloatScalarOrVector< gentype >();
		return x;
	}

	// floatn nan (uintn nancode)
	// doublen nan (ulongn nancode)
	inline float nan (uint nancode)				{ return float(); }
	inline float2 nan (uint2 nancode)			{ return float2(); }
	inline float3 nan (uint3 nancode)			{ return float3(); }
	inline float4 nan (uint4 nancode)			{ return float4(); }
	inline float8 nan (uint8 nancode)			{ return float8(); }
	inline float16 nan (uint16 nancode)			{ return float16(); }

	inline double nan (ulong nancode)			{ return double(); }
	inline double2 nan (ulong2 nancode)			{ return double2(); }
	inline double3 nan (ulong3 nancode)			{ return double3(); }
	inline double4 nan (ulong4 nancode)			{ return double4(); }
	inline double8 nan (ulong8 nancode)			{ return double8(); }
	inline double16 nan (ulong16 nancode)		{ return double16(); }
	
	template <typename gentype> inline
	gentype nextafter (gentype x, gentype y)
	{
		_cl_::Check_IsAnyFloatScalarOrVector< gentype >();
		return x;
	}
	
	template <typename gentype> inline
	gentype pow (gentype x, gentype y)
	{
		_cl_::Check_IsAnyFloatScalarOrVector< gentype >();
		return x;
	}
	
	template <typename gentype> inline
	gentype pown (gentype x, typename _cl_::ToVecFrom< gentype, int >::type y)
	{
		_cl_::Check_IsAnyFloatScalarOrVector< gentype >();
		return x;
	}
	
	template <typename gentype> inline
	gentype powr (gentype x, gentype y)
	{
		_cl_::Check_IsAnyFloatScalarOrVector< gentype >();
		return x;
	}
	
	template <typename gentype> inline
	gentype half_powr (gentype x, gentype y)
	{
		_cl_::Check_IsAnyFloatScalarOrVector< gentype >();
		return x;
	}
	
	template <typename gentype> inline
	gentype native_powr (gentype x, gentype y)
	{
		_cl_::Check_IsAnyFloatScalarOrVector< gentype >();
		return x;
	}
	
	template <typename gentype> inline
	gentype half_recip (gentype x)
	{
		_cl_::Check_IsAnyFloatScalarOrVector< gentype >();
		return x;
	}
	
	template <typename gentype> inline
	gentype native_recip (gentype x)
	{
		_cl_::Check_IsAnyFloatScalarOrVector< gentype >();
		return x;
	}
	
	template <typename gentype> inline
	gentype remainder (gentype x, gentype y)
	{
		_cl_::Check_IsAnyFloatScalarOrVector< gentype >();
		return x;
	}

	// gentype remquo (gentype x, gentype y, __global intn *quo)
	// gentype remquo (gentype x, gentype y, __local intn *quo)
	// gentype remquo (gentype x, gentype y, __private intn *quo)
	template <typename gentype> inline
	gentype remquo (gentype x, gentype y, typename _cl_::ToVecFrom< gentype, int >::type *quo)
	{
		_cl_::Check_IsAnyFloatScalarOrVector< gentype >();
		return x;
	}
	
	template <typename gentype> inline
	gentype rint (gentype x)
	{
		_cl_::Check_IsAnyFloatScalarOrVector< gentype >();
		return x;
	}
	
	template <typename gentype> inline
	gentype round (gentype x)
	{
		_cl_::Check_IsAnyFloatScalarOrVector< gentype >();
		return x;
	}

	template <typename gentype> inline
	gentype rootn (gentype x, typename _cl_::ToVecFrom< gentype, int >::type y)
	{
		_cl_::Check_IsAnyFloatScalarOrVector< gentype >();
		return x;
	}
	
	template <typename gentype> inline
	gentype sqrt (gentype x)
	{
		_cl_::Check_IsAnyFloatScalarOrVector< gentype >();
		return x;
	}
	
	template <typename gentype> inline
	gentype half_sqrt (gentype x)
	{
		_cl_::Check_IsAnyFloatScalarOrVector< gentype >();
		return x;
	}
	
	template <typename gentype> inline
	gentype native_sqrt (gentype x)
	{
		_cl_::Check_IsAnyFloatScalarOrVector< gentype >();
		return x;
	}
	
	template <typename gentype> inline
	gentype rsqrt (gentype x)
	{
		_cl_::Check_IsAnyFloatScalarOrVector< gentype >();
		return x;
	}
	
	template <typename gentype> inline
	gentype half_rsqrt (gentype x)
	{
		_cl_::Check_IsAnyFloatScalarOrVector< gentype >();
		return x;
	}
	
	template <typename gentype> inline
	gentype native_rsqrt (gentype x)
	{
		_cl_::Check_IsAnyFloatScalarOrVector< gentype >();
		return x;
	}
	
	template <typename gentype> inline
	gentype sin (gentype x)
	{
		_cl_::Check_IsAnyFloatScalarOrVector< gentype >();
		return x;
	}

	// gentype sincos (gentype x, __global gentype *cosval)
	// gentype sincos (gentype x, __local gentype *cosval)
	// gentype sincos (gentype x, __private gentype *cosval)
	template <typename gentype> inline
	gentype sincos (gentype x, gentype *cosval)
	{
		_cl_::Check_IsAnyFloatScalarOrVector< gentype >();
		return x;
	}
	
	template <typename gentype> inline
	gentype sinh (gentype x)
	{
		_cl_::Check_IsAnyFloatScalarOrVector< gentype >();
		return x;
	}
	
	template <typename gentype> inline
	gentype sinpi (gentype x)
	{
		_cl_::Check_IsAnyFloatScalarOrVector< gentype >();
		return x;
	}
	
	template <typename gentype> inline
	gentype half_sin (gentype x)
	{
		_cl_::Check_IsAnyFloatScalarOrVector< gentype >();
		return x;
	}
	
	template <typename gentype> inline
	gentype native_sin (gentype x)
	{
		_cl_::Check_IsAnyFloatScalarOrVector< gentype >();
		return x;
	}
	
	template <typename gentype> inline
	gentype tan (gentype x)
	{
		_cl_::Check_IsAnyFloatScalarOrVector< gentype >();
		return x;
	}
	
	template <typename gentype> inline
	gentype tanh (gentype x)
	{
		_cl_::Check_IsAnyFloatScalarOrVector< gentype >();
		return x;
	}
	
	template <typename gentype> inline
	gentype tanpi (gentype x)
	{
		_cl_::Check_IsAnyFloatScalarOrVector< gentype >();
		return x;
	}
	
	template <typename gentype> inline
	gentype half_tan (gentype x)
	{
		_cl_::Check_IsAnyFloatScalarOrVector< gentype >();
		return x;
	}
	
	template <typename gentype> inline
	gentype native_tan (gentype x)
	{
		_cl_::Check_IsAnyFloatScalarOrVector< gentype >();
		return x;
	}
	
	template <typename gentype> inline
	gentype tgamma (gentype x)
	{
		_cl_::Check_IsAnyFloatScalarOrVector< gentype >();
		return x;
	}
	
	template <typename gentype> inline
	gentype trunc (gentype x)
	{
		_cl_::Check_IsAnyFloatScalarOrVector< gentype >();
		return x;
	}
	
	// gentypen shuffle (gentypem x, ugentypen mask)
	template <typename gentype, int I0> inline
	gentype shuffle (typename _cl_::ToVec< gentype, I0 >::type x, uint mask)
	{
		_cl_::Check_IsScalar< gentype >();
		return gentype();
	}
	
	// gentypen shuffle (gentypem x, ugentypen mask)
	template <typename gentype, int I0, int I1> inline
	typename _cl_::ToVec< gentype, I1 >::type
		shuffle (typename _cl_::ToVec< gentype, I0 >::type x,
				 typename _cl_::ToVec< uint, I1 >::type mask)
	{
		_cl_::Check_IsScalar< gentype >();
		return typename _cl_::ToVec< gentype, I1 >::type();
	}
	
	// gentypen shuffle2 (gentypem x, gentypem y, ugentypen mask)
	template <typename gentype, int I0> inline
	gentype shuffle2 (typename _cl_::ToVec< gentype, I0 >::type x,
					  typename _cl_::ToVec< gentype, I0 >::type y,
					  uint mask)
	{
		_cl_::Check_IsScalar< gentype >();
		return gentype();
	}
	
	// gentypen shuffle2 (gentypem x, gentypem y, ugentypen mask)
	template <typename gentype, int I0, int I1> inline
	typename _cl_::ToVec< gentype, I1 >::type
		shuffle2 (typename _cl_::ToVec< gentype, I0 >::type x,
				  typename _cl_::ToVec< gentype, I0 >::type y,
				  typename _cl_::ToVec< uint, I1 >::type mask)
	{
		_cl_::Check_IsScalar< gentype >();
		return typename _cl_::ToVec< gentype, I1 >::type();
	}
	
	template <typename gentype> inline
	int vec_step (gentype a)
	{
		_cl_::Check_IsScalarOrVector< gentype >();
		return _cl_::VecSize< gentype >::value;
	}

	inline int vec_step (char3 a)			{ return int(4); }
	inline int vec_step (uchar3 a)			{ return int(4); }
	inline int vec_step (short3 a)			{ return int(4); }
	inline int vec_step (ushort3 a)			{ return int(4); }
	//inline int vec_step (half3 a)			{ return int(4); }
	inline int vec_step (int3 a)			{ return int(4); }
	inline int vec_step (uint3 a)			{ return int(4); }
	inline int vec_step (long3 a)			{ return int(4); }
	inline int vec_step (ulong3 a)			{ return int(4); }
	inline int vec_step (float3 a)			{ return int(4); }
	inline int vec_step (double3 a)			{ return int(4); }
	//inline int vec_step (type x)			{ return int(); }


	// int isequal (float x, float y)
	// intn isequal (floatn x, floatn y)
	// int isequal (double x, double y)
	// longn isequal (doublen x, doublen y)
	
	// int isnotequal (float x, float y)
	// intn isnotequal (floatn x, floatn y)
	// int isnotequal (double x, double y)
	// longn isnotequal (doublen x, doublen y) 

	// int isgreater (float x, float y)
	// intn isgreater (floatn x, floatn y)
	// int isgreater (double x, double y)
	// longn isgreater (doublen x, doublen y)

	// int isgreaterequal (float x, float y)
	// intn isgreaterequal (floatn x, floatn y)
	// int isgreaterequal (double x, double y)
	// longn isgreaterequal (doublen x, doublen y) 
	
	// int isless (float x, float y)
	// intn isless (floatn x, floatn y)
	// int isless (double x, double y)
	// longn isless (doublen x, doublen y)

	// int islessequal (float x, float y)
	// intn islessequal (floatn x, floatn y)
	// int islessequal (double x, double y)
	// longn islessequal (doublen x, doublen y) 
	
	// int islessgreater (float x, float y)
	// intn islessgreater (floatn x, floatn y)
	// int islessgreater (double x, double y)
	// longn islessgreater (doublen x, doublen y) 
	
	// int isfinite (float x)
	// intn isfinite (floatn x)
	// int isfinite (double x)
	// longn isfinite (doublen x)

	// int isinf (float x)
	// intn isinf (floatn x)
	// int isinf (double x)
	// longn isinf (doublen x)
	
	// int isnan (float x)
	// intn isnan (floatn x)
	// int isnan (double x)
	// longn isnan (doublen x) 
	
	// int isnormal (float x)
	// intn isnormal (floatn x)
	// int isnormal (double x)
	// longn isnormal (doublen x)
	
	// int isordered (float x, float y)
	// intn isordered (floatn x, floatn y)
	// int isordered (double x, double y)
	// longn isordered (doublen x, doublen y) 
	
	// int isunordered (float x, float y)
	// intn isunordered (floatn x, floatn y)
	// int isunordered (double x, double y)
	// longn isunordered (doublen x, doublen y)
	
	// int signbit (float x)
	// intn signbit (floatn x)
	// int signbit (double x)
	// longn signbit (doublen x) 

#	define __ISEQUAL( _itype_, _ftype_ ) \
	inline _itype_ isequal (_ftype_ x, _ftype_ y)			{ return _itype_(); } \
	inline _itype_ isnotequal (_ftype_ x, _ftype_ y)		{ return _itype_(); } \
	inline _itype_ isgreater (_ftype_ x, _ftype_ y)			{ return _itype_(); } \
	inline _itype_ isgreaterequal (_ftype_ x, _ftype_ y)	{ return _itype_(); } \
	inline _itype_ isless (_ftype_ x, _ftype_ y)			{ return _itype_(); } \
	inline _itype_ islessequal (_ftype_ x, _ftype_ y)		{ return _itype_(); } \
	inline _itype_ islessgreater (_ftype_ x, _ftype_ y)		{ return _itype_(); } \
	inline _itype_ isfinite (_ftype_ x)						{ return _itype_(); } \
	inline _itype_ isinf (_ftype_ x)						{ return _itype_(); } \
	inline _itype_ isnan (_ftype_ x)						{ return _itype_(); } \
	inline _itype_ isnormal (_ftype_ x)						{ return _itype_(); } \
	inline _itype_ isordered (_ftype_ x)					{ return _itype_(); } \
	inline _itype_ isunordered (_ftype_ x, _ftype_ y)		{ return _itype_(); } \
	inline _itype_ signbit (_ftype_ x)						{ return _itype_(); }
#	define __ISEQUAL1( _itype_, _ftype_ ) \
	__ISEQUAL( _itype_, _ftype_ ) \
	__ISEQUAL( _itype_##2, _ftype_##2 ) \
	__ISEQUAL( _itype_##3, _ftype_##3 ) \
	__ISEQUAL( _itype_##4, _ftype_##4 ) \
	__ISEQUAL( _itype_##8, _ftype_##8 ) \
	__ISEQUAL( _itype_##16, _ftype_##16 )
	__ISEQUAL1( int, float )
	__ISEQUAL1( long, double )
#	undef __ISEQUAL1
#	undef __ISEQUAL

	template <typename igentype> inline
	int any (igentype x)
	{
		_cl_::Check_IsAnyIntegerScalarOrVector< igentype >();
		return int();
	}
	
	template <typename igentype> inline
	int all (igentype x)
	{
		_cl_::Check_IsAnyIntegerScalarOrVector< igentype >();
		return int();
	}

	// gentype bitselect (gentype a, gentype b, gentype c)
	// doublen bitselect (doublen a, doublen b, doublen c) 
	template <typename gentype> inline
	gentype bitselect (gentype a, gentype b, gentype c)
	{
		_cl_::Check_IsScalarOrVector< gentype >();
		return a;
	}

	// gentype select (gentype a, gentype b, igentype c)
	// doublen select (doublen a, doublen b, longn c)
	template <typename gentype> inline
	gentype select (gentype a, gentype b, typename _cl_::ToInteger<gentype>::signed_t c)
	{
		_cl_::Check_IsScalarOrVector< gentype >();
		return a;
	}
	
	// gentype select (gentype a, gentype b, ugentype c)
	// doublen select (doublen a, doublen b, ulongn c)
	template <typename gentype> inline
	gentype select (gentype a, gentype b, typename _cl_::ToInteger<gentype>::unsigned_t c)
	{
		_cl_::Check_IsScalarOrVector< gentype >();
		return a;
	}

	inline
	void barrier (cl_mem_fence_flags flags)
	{}

	// gentypen vloadn (size_t offset, const __global gentype *p)
	// gentypen vloadn (size_t offset, const __local gentype *p)
	// gentypen vloadn (size_t offset, const __constant gentype *p)
	// gentypen vloadn (size_t offset, const __private gentype *p)
	// void vstoren (gentypen data , size_t offset , const __global gentype *p )
	// void vstoren (gentypen data, size_t offset, const __local gentype *p)
	// void vstoren (gentypen data, size_t offset, const __private gentype *p)
#	define __VLOAD( _type_, _size_ ) \
	inline _type_##_size_ vload##_size_ (size_t offset, const _type_ *p)				{ return _type_##_size_(); } \
	inline void vstore##_size_ (_type_##_size_ data, size_t offset, const _type_ *p)	{}
#	define __VLOAD1( _type_ ) \
	__VLOAD( _type_, 2 ) \
	__VLOAD( _type_, 3 ) \
	__VLOAD( _type_, 4 ) \
	__VLOAD( _type_, 8 ) \
	__VLOAD( _type_, 16 )
	__VLOAD1( char )
	__VLOAD1( uchar )
	__VLOAD1( short )
	__VLOAD1( ushort )
	__VLOAD1( int )
	__VLOAD1( uint )
	__VLOAD1( long )
	__VLOAD1( ulong )
	__VLOAD1( float )
	__VLOAD1( double )
#	undef __VLOAD1
#	undef __VLOAD


	inline size_t  get_global_id (uint dimindx)			{ return size_t(); }
	inline size_t  get_global_size (uint dimindx)		{ return size_t(); }
	inline size_t  get_global_offset (uint dimindx)		{ return size_t(); }
	inline size_t  get_group_id (uint dimindx)			{ return size_t(); }
	inline size_t  get_local_id (uint dimindx)			{ return size_t(); }
	inline size_t  get_local_size (uint dimindx)		{ return size_t(); }
	inline size_t  get_num_groups (uint dimindx)		{ return size_t(); }
	inline uint    get_work_dim ()						{ return uint(); }
	

