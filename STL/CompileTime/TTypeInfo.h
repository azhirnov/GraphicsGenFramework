// Copyright © 2014-2016  Zhirnov Andrey. All rights reserved.

#pragma once

#include "../Types/TypeId.h"
#include "../BaseTypes/Float.h"
#include "../BaseTypes/Fixed.h"
#include "../BaseTypes/Half.h"
#include "../BaseTypes/UFloat.h"
#include "TypeTraits.h"
#include "TemplateUtils.h"
#include "TemplateMath.h"
#include "PlainOldDataType.h"
#include "TypeQualifier.h"
#include "NearType.h"

namespace GX_STL
{
namespace CompileTime
{

	namespace _ctime_hidden_
	{
		// Limits
#	pragma warning (push)
#	pragma warning (disable: 4723)	// potential divide by 0
		
#	undef UINT8_MAX
#	undef UINT16_MAX
#	undef UINT32_MAX
#	undef UINT64_MAX
#	undef INT8_MIN
#	undef INT16_MIN
#	undef INT32_MIN
#	undef INT64_MIN
#	undef INT8_MAX
#	undef INT16_MAX
#	undef INT32_MAX
#	undef INT64_MAX

#	ifdef GX_MATH_REAL_LIMITS

		constexpr	ubyte	UINT8_MAX			= ubyte ( -1 );
		constexpr	ushort	UINT16_MAX			= ushort( -1 );
		constexpr	uint	UINT32_MAX			= uint( -1 );
		constexpr	ulong	UINT64_MAX			= ulong( -1 );

		constexpr	byte	INT8_MIN			= byte ( 1 << (CompileTime::SizeOf<byte>::bits-1) );
		constexpr	short	INT16_MIN			= short( 1 << (CompileTime::SizeOf<short>::bits-1) );
		constexpr	int		INT32_MIN			= int(1) << (CompileTime::SizeOf<int>::bits-1);
		constexpr	ilong	INT64_MIN			= ilong(1) << (CompileTime::SizeOf<ilong>::bits-1);

		constexpr	byte	INT8_MAX			= UINT8_MAX  & ~INT8_MIN;
		constexpr	short	INT16_MAX			= UINT16_MAX & ~INT16_MIN;
		constexpr	int		INT32_MAX			= UINT32_MAX & ~INT32_MIN;
		constexpr	ilong	INT64_MAX			= UINT64_MAX & ~INT64_MIN;

		constexpr	float	FLOAT_MAX			= static_cast< float const &>( 0x7F7FFFFF );
		constexpr	float	FLOAT_MIN			= static_cast< float const &>( 0x00800000 );

		constexpr	double	DOUBLE_MAX			= static_cast< double const &>( 0x7FEFFFFFFFFFFFFFull );
		constexpr	double	DOUBLE_MIN			= static_cast< double const &>( 0x0010000000000000ull );

		constexpr	uint	FLOAT_MAX_POW		= 38;
		constexpr	uint	DOUBLE_MAX_POW		= 308;

		constexpr	float	FLOAT_INF			= 1.0f / static_cast< float const  &>( uint(0) );
		constexpr	double	DOUBLE_INF			= 1.0  / static_cast< double const &>( ulong(0) );

		constexpr	float	FLOAT_NAN			= static_cast< float const  &>( uint(0) ) / static_cast< float const  &>( uint(0) );
		constexpr	double	DOUBLE_NAN			= static_cast< double const &>( ulong(0) ) / static_cast< double const &>( ulong(0) );

#	else

		constexpr	ubyte	UINT8_MAX			= 0xFF;
		constexpr	ushort	UINT16_MAX			= 0xFFFF;
		constexpr	uint	UINT32_MAX			= 0xFFFFFFFF;
		constexpr	ulong	UINT64_MAX			= 0xFFFFFFFFFFFFFFFFull;

		constexpr	byte	INT8_MIN			= byte (0x80);
		constexpr	short	INT16_MIN			= short(0x8000);
		constexpr	int		INT32_MIN			= 0x80000000;
		constexpr	ilong	INT64_MIN			= 0x8000000000000000ull;

		constexpr	byte	INT8_MAX			= UINT8_MAX  & ~INT8_MIN;
		constexpr	short	INT16_MAX			= UINT16_MAX & ~INT16_MIN;
		constexpr	int		INT32_MAX			= UINT32_MAX & ~INT32_MIN;
		constexpr	ilong	INT64_MAX			= UINT64_MAX & ~INT64_MIN;

		constexpr	float	FLOAT_MAX			= static_cast< float const &>( 0x7F7FFFFF );
		constexpr	float	FLOAT_MIN			= static_cast< float const &>( 0x00800000 );

		constexpr	double	DOUBLE_MAX			= static_cast< double const &>( 0x7FEFFFFFFFFFFFFFull );
		constexpr	double	DOUBLE_MIN			= static_cast< double const &>( 0x0010000000000000ull );
	
		constexpr	uint	FLOAT_MAX_POW		= 38;
		constexpr	uint	DOUBLE_MAX_POW		= 308;
		
		constexpr	float	FLOAT_INF			= static_cast< float const  &>( 0x7F800000 );
		constexpr	double	DOUBLE_INF			= static_cast< double const &>( 0x7FF0000000000000ull );

		constexpr	float	FLOAT_NAN			= static_cast< float const  &>( 0xFFC00000 );
		constexpr	double	DOUBLE_NAN			= static_cast< double const &>( 0xFFF8000000000000ull );

#	endif	// GX_MATH_REAL_LIMITS

		constexpr	float	FLOAT_EPSILON		= 1.0e-6f;
		constexpr	double	DOUBLE_EPSILON		= 1.0e-8;
		
#	pragma warning (pop)

		enum ETypeDescriptor
		{
			UNKNOWN		= 0,
			VOID		= 1 << 1,
			FLOAT		= 1 << 2,
			INTEGER		= 1 << 3,
			FIXED		= 1 << 4,
			BOOL		= 1 << 5,

			SIGNED		= 1 << 6,
			UNSIGNED	= 1 << 7,

			_MASK		= (1 << 8) - 1,

			COMPOUND	= 1 << 8,	// has inner type
			VECTOR		= 1 << 9,	// vector type
			MATRIX		= 1 << 10,	// matrix type
			SOFTWARE	= 1 << 11,	// software type (it is slower than hardware types)
		};

	}	// _ctime_hidden_


	
	template <typename T>	struct TypeInfo;



	//
	// Type Descriptor
	//

	namespace TypeDescriptor
	{

		// Get Inner Type
		template <typename T>
		struct InnerType
		{
			template <typename U, bool I>
			struct _GetInner;

			template <typename U>
			struct _GetInner<U,false>
			{
				typedef typename TypeInfo< U >::inner_type	type;
			};

			template <typename U>
			struct _GetInner<U,true>
			{
				typedef typename _GetInner< typename TypeInfo< U >::inner_type,
											(TypeInfo< U >::FLAGS & _ctime_hidden_::COMPOUND) != 0 >::type	type;
			};

			typedef typename _GetInner< T, true >::type		type;
		};


		// get type info
		template <typename T>
		using GetTypeInfo = TypeInfo< typename TypeTraits::RemoveConstVolatile<T> >;
		

		// Is<Type>
		template <typename T>
		struct IsFloat {
			static const bool	value = ( int(TypeInfo< T >::FLAGS) & int(_ctime_hidden_::FLOAT) ) != 0;
		};

		template <typename T>
		struct IsInteger {
			static const bool	value = ( int(TypeInfo< T >::FLAGS) & int(_ctime_hidden_::INTEGER) ) != 0;
		};

		template <typename T>
		struct IsFixed {
			static const bool	value = ( int(TypeInfo< T >::FLAGS) & int(_ctime_hidden_::FIXED) ) != 0;
		};

		template <typename T>
		struct IsSigned {
			static const bool	value = ( int(TypeInfo< T >::FLAGS) & int(_ctime_hidden_::SIGNED) ) != 0;
		};

		template <typename T>
		struct IsUnsigned {
			static const bool	value = ( int(TypeInfo< T >::FLAGS) & int(_ctime_hidden_::UNSIGNED) ) != 0;
		};

		template <typename T>
		struct IsSoftware {
			static const bool	value = ( int(TypeInfo< T >::FLAGS) & int(_ctime_hidden_::SOFTWARE) ) != 0;
		};

		template <typename T>
		struct IsVoid {
			static const bool	value = false;
		};

		template <>
		struct IsVoid<void> {
			static const bool	value = true;
		};


		// Strong comparison
		template <typename T1, typename T2>
		struct IsSameTypes {
			static const bool	value = false;
		};

		template <typename T>
		struct IsSameTypes <T,T> {
			static const bool	value = true;
		};


		// Weak comparison
		template <typename T1, typename T2>
		struct IsSameTypesWithoutQualifiers {
			static const bool	value = IsSameTypes< 
											typename TypeTraits::RemoveConstVolatile<T1>, 
											typename TypeTraits::RemoveConstVolatile<T2> >::value;
		};
	

		template <template <typename ...> class Left, template <typename ...> class Right>
		struct IsSameTemplates			{ static const bool  value = false; };

		template <template <typename ...> class T>
		struct IsSameTemplates< T, T >	{ static const bool  value = true; };


		template <typename ToType, typename FromType>
		static inline bool CanCastTo (FromType *p)
		{
			if ( p == null )
				return true;

			DESCTOP_ONLY( return dynamic_cast< ToType *>( p ) != null );
			MOBILE_ONLY( return true );
		}

	}	// TypeDescriptor


#	define __GX_CHECK_SIMILAR_TYPES() \
		private: \
			static const bool _sameSize = ( \
				CompileTime::MustBeSameSize< type, Similar_Unsigned >(), \
				CompileTime::MustBeSameSize< type, Similar_Signed >(), \
				CompileTime::MustBeSameSize< type, Similar_Float >(), \
				CompileTime::MustBeSameSize< type, Similar_UInt >(), \
				CompileTime::MustBeSameSize< type, Similar_Int >(), true );


#	define GX_TYPE_INFO_WITH_INNER_TYPE( _type_, _innerType_, _typeName_ ) \
		template <> \
		struct TypeInfo< _type_ > \
		{ \
			typedef _type_			type; \
			typedef _innerType_		inner_type; \
			\
			typedef TypeInfo< inner_type >::Similar_Unsigned	Similar_Unsigned; \
			typedef TypeInfo< inner_type >::Similar_Signed		Similar_Signed; \
			typedef TypeInfo< inner_type >::Similar_Float		Similar_Float; \
			typedef TypeInfo< inner_type >::Similar_UInt		Similar_UInt; \
			typedef TypeInfo< inner_type >::Similar_Int			Similar_Int; \
			\
			enum { \
				FLAGS	= (int)TypeInfo< inner_type >::FLAGS | int(_ctime_hidden_::COMPOUND), \
				UID		= __COUNTER__, \
			}; \
			\
			static			 type	Max()		{ return type( TypeInfo< inner_type >::Max() ); } \
			static			 type	Min()		{ return type( TypeInfo< inner_type >::Min() ); } \
			\
			static			 type	Epsilon()	{ return type( TypeInfo< inner_type >::Epsilon() ); } \
			static constexpr uint	SignBit()	{ return TypeInfo< inner_type >::SignBit(); } \
			\
			static const char *		Name()		{ return _typeName_; } \
			\
			__GX_CHECK_SIMILAR_TYPES(); \
		}


#	define GX_TEMPLATE_TYPE_INFO_WITH_INNER_TYPE( _type_, _typeName_ ) \
		template <typename T> \
		struct ::GX_STL::CompileTime::TypeInfo< _type_<T> > \
		{ \
			typedef _type_<T>		type; \
			typedef T				inner_type; \
			\
			typedef typename TypeInfo< inner_type >::Similar_Unsigned	Similar_Unsigned; \
			typedef typename TypeInfo< inner_type >::Similar_Signed		Similar_Signed; \
			typedef typename TypeInfo< inner_type >::Similar_Float		Similar_Float; \
			typedef typename TypeInfo< inner_type >::Similar_UInt		Similar_UInt; \
			typedef typename TypeInfo< inner_type >::Similar_Int		Similar_Int; \
			\
			enum { \
				FLAGS	= (int)TypeInfo< inner_type >::FLAGS | int(_ctime_hidden_::COMPOUND), \
				UID		= __COUNTER__, \
			}; \
			\
			static			 type	Max()		{ return type( TypeInfo< inner_type >::Max() ); } \
			static			 type	Min()		{ return type( TypeInfo< inner_type >::Min() ); } \
			\
			static			 type	Epsilon()	{ return type( TypeInfo< inner_type >::Epsilon() ); } \
			static constexpr uint	SignBit()	{ return TypeInfo< inner_type >::SignBit(); } \
			\
			static const char *		Name()		{ return _typeName_; } \
			\
			__GX_CHECK_SIMILAR_TYPES(); \
		}
	


	//
	// Type Info (example)
	//
	
	template <typename T>
	struct TypeInfo
	{
		/*	
		typedef T		type;
		typedef T		inner_type;

		// similar types //
		typedef void	Similar_Unsigned;
		typedef void	Similar_Signed;
		typedef void	Similar_Float;
		typedef void	Similar_UInt;
		typedef void	Similar_Int;
		*/
		// type descriptor //
		enum {
			FLAGS	= _ctime_hidden_::UNKNOWN,	// type descriptor flags
			UID		= -1,						// unique identifer
		};
		/*
		static constexpr type	Max()		{ return type(1); }
		static constexpr type	Min()		{ return type(0); }
		static constexpr type	Inf()		{ return type(0); }
		static constexpr type	NaN()		{ return type(0); }

		static constexpr type	Epsilon()	{ return type(0); }
		static constexpr uint	SignBit()	{ return 0; }

		static const char *		Name()		{ return ""; }

		STATIC_WARNING( "unknown type" );
		*/
	};
	

	template <typename T>	struct TypeInfo< const T > : TypeInfo<T> {};
	template <typename T>	struct TypeInfo< volatile T > : TypeInfo<T> {};


	template <>
	struct TypeInfo<void>
	{
		typedef void		type;
		typedef type		inner_type;

		// similar types //
		typedef void		Similar_Unsigned;
		typedef void		Similar_Signed;
		typedef void		Similar_Float;
		typedef void		Similar_UInt;
		typedef void		Similar_Int;

		// type descriptor //
		enum {
			FLAGS	= _ctime_hidden_::VOID,
			UID		= __COUNTER__,
		};

		static void				Max()		{ return void(); }
		static void				Min()		{ return void(); }
		
		static void				Epsilon()	{ return void(); }
		static constexpr uint	SignBit()	{ return 0; }
		
		static const char *		Name()		{ return "void"; }
	};


	template <>
	struct TypeInfo<bool>
	{
		typedef bool		type;
		typedef type		inner_type;

		// similar types //
		typedef ubyte		Similar_Unsigned;
		typedef byte		Similar_Signed;
		typedef float8u_t	Similar_Float;
		typedef ubyte		Similar_UInt;
		typedef byte		Similar_Int;

		// type descriptor //
		enum {
			FLAGS	= (int)_ctime_hidden_::BOOL | (int)_ctime_hidden_::UNSIGNED,
			UID		= __COUNTER__,
		};

		static constexpr type			Max()		{ return true; }
		static constexpr type			Min()		{ return false; }
		
		static constexpr type			Epsilon()	{ return type(0); }
		static constexpr uint			SignBit()	{ return 0; }
		
		static const char *				Name()		{ return "bool"; }

		__GX_CHECK_SIMILAR_TYPES();
	};


	template <>
	struct TypeInfo<byte>
	{
		typedef byte		type;
		typedef type		inner_type;

		// similar types //
		typedef ubyte		Similar_Unsigned;
		typedef byte		Similar_Signed;
		typedef float8u_t	Similar_Float;
		typedef ubyte		Similar_UInt;
		typedef byte		Similar_Int;

		// type descriptor //
		enum {
			FLAGS	= (int)_ctime_hidden_::INTEGER | (int)_ctime_hidden_::SIGNED,
			UID		= __COUNTER__,
		};

		static constexpr type			Max()		{ return _ctime_hidden_::INT8_MAX; }
		static constexpr type			Min()		{ return _ctime_hidden_::INT8_MIN; }
		
		static constexpr type			Epsilon()	{ return type(0); }
		static constexpr uint			SignBit()	{ return CompileTime::SizeOf<type>::bits-1; }
		
		static const char *				Name()		{ return "byte"; }

		__GX_CHECK_SIMILAR_TYPES();
	};


	template <>
	struct TypeInfo<ubyte>
	{
		typedef ubyte		type;
		typedef type		inner_type;

		// similar types //
		typedef ubyte		Similar_Unsigned;
		typedef byte		Similar_Signed;
		typedef float8u_t	Similar_Float;
		typedef ubyte		Similar_UInt;
		typedef byte		Similar_Int;

		// type descriptor //
		enum {
			FLAGS	= (int)_ctime_hidden_::INTEGER | (int)_ctime_hidden_::UNSIGNED,
			UID		= __COUNTER__,
		};

		static constexpr type			Max()		{ return _ctime_hidden_::UINT8_MAX; }
		static constexpr type			Min()		{ return type(0); }
		
		static constexpr type			Epsilon()	{ return type(0); }
		static constexpr uint			SignBit()	{ return uint(-1); }
		
		static const char *				Name()		{ return "ubyte"; }

		__GX_CHECK_SIMILAR_TYPES();
	};


	template <>
	struct TypeInfo<char>
	{
		typedef char		type;
		typedef type		inner_type;

		// similar types //
		typedef ubyte		Similar_Unsigned;
		typedef byte		Similar_Signed;
		typedef float8u_t	Similar_Float;
		typedef ubyte		Similar_UInt;
		typedef byte		Similar_Int;

		// type descriptor //
		enum {
			FLAGS	= (int)_ctime_hidden_::INTEGER | (int)_ctime_hidden_::SIGNED,
			UID		= __COUNTER__,
		};

		static constexpr type			Max()		{ return _ctime_hidden_::INT8_MAX; }
		static constexpr type			Min()		{ return _ctime_hidden_::INT8_MIN; }
		
		static constexpr type			Epsilon()	{ return type(0); }
		static constexpr uint			SignBit()	{ return CompileTime::SizeOf<type>::bits-1; }
		
		static const char *				Name()		{ return "char"; }

		__GX_CHECK_SIMILAR_TYPES();
	};


	template <>
	struct TypeInfo<short>
	{
		typedef short		type;
		typedef type		inner_type;

		// similar types //
		typedef ushort		Similar_Unsigned;
		typedef short		Similar_Signed;
		typedef half		Similar_Float;
		typedef ushort		Similar_UInt;
		typedef short		Similar_Int;

		// type descriptor //
		enum {
			FLAGS	= (int)_ctime_hidden_::INTEGER | (int)_ctime_hidden_::SIGNED,
			UID		= __COUNTER__,
		};

		static constexpr type			Max()		{ return _ctime_hidden_::INT16_MAX; }
		static constexpr type			Min()		{ return _ctime_hidden_::INT16_MIN; }
		
		static constexpr type			Epsilon()	{ return type(0); }
		static constexpr uint			SignBit()	{ return CompileTime::SizeOf<type>::bits-1; }
		
		static const char *				Name()		{ return "short"; }

		__GX_CHECK_SIMILAR_TYPES();
	};


	template <>
	struct TypeInfo<ushort>
	{
		typedef ushort		type;
		typedef type		inner_type;

		// similar types //
		typedef ushort		Similar_Unsigned;
		typedef short		Similar_Signed;
		typedef half		Similar_Float;
		typedef ushort		Similar_UInt;
		typedef short		Similar_Int;

		// type descriptor //
		enum {
			FLAGS	= (int)_ctime_hidden_::INTEGER | (int)_ctime_hidden_::UNSIGNED,
			UID		= __COUNTER__,
		};

		static constexpr type			Max()		{ return _ctime_hidden_::UINT16_MAX; }
		static constexpr type			Min()		{ return type(0); }
		
		static constexpr type			Epsilon()	{ return type(0); }
		static constexpr uint			SignBit()	{ return uint(-1); }
		
		static const char *				Name()		{ return "ushort"; }

		__GX_CHECK_SIMILAR_TYPES();
	};


	template <> struct TypeInfo<wchar> : TypeInfo<short> {};


	template <>
	struct TypeInfo<int>
	{
		typedef int			type;
		typedef type		inner_type;

		// similar types //
		typedef uint		Similar_Unsigned;
		typedef int			Similar_Signed;
		typedef float		Similar_Float;
		typedef uint		Similar_UInt;
		typedef int			Similar_Int;

		// type descriptor //
		enum {
			FLAGS	= (int)_ctime_hidden_::INTEGER | (int)_ctime_hidden_::SIGNED,
			UID		= __COUNTER__,
		};

		static constexpr type			Max()		{ return _ctime_hidden_::INT32_MAX; }
		static constexpr type			Min()		{ return _ctime_hidden_::INT32_MIN; }
		
		static constexpr type			Epsilon()	{ return type(0); }
		static constexpr uint			SignBit()	{ return CompileTime::SizeOf<type>::bits-1; }
		
		static const char *				Name()		{ return "int"; }

		__GX_CHECK_SIMILAR_TYPES();
	};


	template <>
	struct TypeInfo<uint>
	{
		typedef uint		type;
		typedef type		inner_type;

		// similar types //
		typedef uint		Similar_Unsigned;
		typedef int			Similar_Signed;
		typedef float		Similar_Float;
		typedef uint		Similar_UInt;
		typedef int			Similar_Int;

		// type descriptor //
		enum {
			FLAGS	= (int)_ctime_hidden_::INTEGER | (int)_ctime_hidden_::UNSIGNED,
			UID		= __COUNTER__,
		};

		static constexpr type			Max()		{ return _ctime_hidden_::UINT32_MAX; }
		static constexpr type			Min()		{ return type(0); }
		
		static constexpr type			Epsilon()	{ return type(0); }
		static constexpr uint			SignBit()	{ return uint(-1); }
		
		static const char *				Name()		{ return "uint"; }

		__GX_CHECK_SIMILAR_TYPES();
	};


	template <>
	struct TypeInfo<ilong>
	{
		typedef ilong		type;
		typedef type		inner_type;

		// similar types //
		typedef ulong		Similar_Unsigned;
		typedef ilong		Similar_Signed;
		typedef double		Similar_Float;
		typedef ulong		Similar_UInt;
		typedef ilong		Similar_Int;

		// type descriptor //
		enum {
			FLAGS	= (int)_ctime_hidden_::INTEGER | (int)_ctime_hidden_::SIGNED,
			UID		= __COUNTER__,
		};

		static constexpr type			Max()		{ return _ctime_hidden_::INT64_MAX; }
		static constexpr type			Min()		{ return _ctime_hidden_::INT64_MIN; }
		
		static constexpr type			Epsilon()	{ return type(0); }
		static constexpr uint			SignBit()	{ return CompileTime::SizeOf<type>::bits-1; }
		
		static const char *				Name()		{ return "ilong"; }

		__GX_CHECK_SIMILAR_TYPES();
	};


	template <>
	struct TypeInfo<ulong>
	{
		typedef ulong		type;
		typedef type		inner_type;

		// similar types //
		typedef ulong		Similar_Unsigned;
		typedef ilong		Similar_Signed;
		typedef double		Similar_Float;
		typedef ulong		Similar_UInt;
		typedef ilong		Similar_Int;

		// type descriptor //
		enum {
			FLAGS	= (int)_ctime_hidden_::INTEGER | (int)_ctime_hidden_::UNSIGNED,
			UID		= __COUNTER__,
		};

		static constexpr type			Max()		{ return _ctime_hidden_::UINT64_MAX; }
		static constexpr type			Min()		{ return 0; }
		
		static constexpr type			Epsilon()	{ return type(0); }
		static constexpr uint			SignBit()	{ return uint(-1); }
		
		static const char *				Name()		{ return "ulong"; }

		__GX_CHECK_SIMILAR_TYPES();
	};


	template <>
	struct TypeInfo<float8u_t>
	{
		typedef float8u_t	type;
		typedef type		inner_type;

		// similar types //
		typedef float8u_t	Similar_Unsigned;
		typedef float8u_t	Similar_Signed;
		typedef float8u_t	Similar_Float;
		typedef ubyte		Similar_UInt;
		typedef byte		Similar_Int;

		// type descriptor //
		enum {
			FLAGS	= (int)_ctime_hidden_::FLOAT | (int)_ctime_hidden_::UNSIGNED | (int)_ctime_hidden_::SOFTWARE,
			UID		= __COUNTER__,
		};

		static			 type			Max()		{ return (type) _ctime_hidden_::FLOAT_MAX; }
		static			 type			Min()		{ return (type) _ctime_hidden_::FLOAT_MIN; }
		static			 type			Inf()		{ return type(0); }	// TODO
		static			 type			NaN()		{ return type(0); }	// TODO
		
		static			 type			Epsilon()	{ return (type) _ctime_hidden_::FLOAT_EPSILON; }
		static constexpr uint			SignBit()	{ return uint(-1); }
		
		static const char *				Name()		{ return "float8u"; }

		__GX_CHECK_SIMILAR_TYPES();
	};


	template <>
	struct TypeInfo<half>
	{
		typedef half		type;
		typedef type		inner_type;

		// similar types //
		typedef half		Similar_Unsigned;
		typedef half		Similar_Signed;
		typedef half		Similar_Float;
		typedef ushort		Similar_UInt;
		typedef short		Similar_Int;

		// type descriptor //
		enum {
			FLAGS	= (int)_ctime_hidden_::FLOAT | (int)_ctime_hidden_::SIGNED | (int)_ctime_hidden_::SOFTWARE,
			UID		= __COUNTER__,
		};

		static			 type			Max()		{ return (type) _ctime_hidden_::FLOAT_MAX; }
		static			 type			Min()		{ return (type) _ctime_hidden_::FLOAT_MIN; }
		static			 type			Inf()		{ return type(0); }	// TODO
		static			 type			NaN()		{ return type(0); }	// TODO
		
		static			 type			Epsilon()	{ return (type) _ctime_hidden_::FLOAT_EPSILON; }
		static constexpr uint			SignBit()	{ return CompileTime::SizeOf<type>::bits-1; }
		
		static const char *				Name()		{ return "half"; }

		__GX_CHECK_SIMILAR_TYPES();
	};


	template <>
	struct TypeInfo<float>
	{
		typedef float		type;
		typedef type		inner_type;

		// similar types //
		typedef float		Similar_Unsigned;
		typedef float		Similar_Signed;
		typedef float		Similar_Float;
		typedef uint		Similar_UInt;
		typedef int			Similar_Int;

		// type descriptor //
		enum {
			FLAGS	= (int)_ctime_hidden_::FLOAT | (int)_ctime_hidden_::SIGNED,
			UID		= __COUNTER__,
		};

		static constexpr type			Max()		{ return _ctime_hidden_::FLOAT_MAX; }
		static constexpr type			Min()		{ return _ctime_hidden_::FLOAT_MIN; }
		static constexpr type			Inf()		{ return _ctime_hidden_::FLOAT_INF; }
		static constexpr type			NaN()		{ return _ctime_hidden_::FLOAT_NAN; }
		
		static constexpr type			Epsilon()	{ return _ctime_hidden_::FLOAT_EPSILON; }
		static constexpr uint			SignBit()	{ return CompileTime::SizeOf<type>::bits-1; }
		
		static const char *				Name()		{ return "float"; }

		__GX_CHECK_SIMILAR_TYPES();
	};


	template <>
	struct TypeInfo<double>
	{
		typedef double		type;
		typedef type		inner_type;

		// similar types //
		typedef double		Similar_Unsigned;
		typedef double		Similar_Signed;
		typedef double		Similar_Float;
		typedef ulong		Similar_UInt;
		typedef ilong		Similar_Int;

		// type descriptor //
		enum {
			FLAGS	= (int)_ctime_hidden_::FLOAT | (int)_ctime_hidden_::SIGNED,
			UID		= __COUNTER__,
		};

		static constexpr type			Max()		{ return _ctime_hidden_::DOUBLE_MAX; }
		static constexpr type			Min()		{ return _ctime_hidden_::DOUBLE_MIN; }
		static constexpr type			Inf()		{ return _ctime_hidden_::DOUBLE_INF; }
		static constexpr type			NaN()		{ return _ctime_hidden_::DOUBLE_NAN; }
		
		static constexpr type			Epsilon()	{ return _ctime_hidden_::DOUBLE_EPSILON; }
		static constexpr uint			SignBit()	{ return CompileTime::SizeOf<type>::bits-1; }
		
		static const char *				Name()		{ return "double"; }

		__GX_CHECK_SIMILAR_TYPES();
	};


	template <>
	struct TypeInfo<long double>
	{
		typedef long double		type;
		typedef type			inner_type;

		// similar types //
		typedef long double		Similar_Unsigned;
		typedef long double		Similar_Signed;
		typedef long double		Similar_Float;
		typedef ulong			Similar_UInt;
		typedef ilong			Similar_Int;

		// type descriptor //
		enum {
			FLAGS	= (int)_ctime_hidden_::FLOAT | (int)_ctime_hidden_::SIGNED,
			UID		= __COUNTER__,
		};

		static constexpr type			Max()		{ return _ctime_hidden_::DOUBLE_MAX; }
		static constexpr type			Min()		{ return _ctime_hidden_::DOUBLE_MIN; }
		static constexpr type			Inf()		{ return _ctime_hidden_::DOUBLE_INF; }
		static constexpr type			NaN()		{ return _ctime_hidden_::DOUBLE_NAN; }
		
		static constexpr type			Epsilon()	{ return _ctime_hidden_::DOUBLE_EPSILON; }
		static constexpr uint			SignBit()	{ return CompileTime::SizeOf<type>::bits-1; }
		
		static const char *				Name()		{ return "long double"; }

		__GX_CHECK_SIMILAR_TYPES();
	};


	template <>
	struct TypeInfo<Fixed16_t>
	{
		typedef Fixed16_t		type;
		typedef type			inner_type;

		// similar types //
		typedef half			Similar_Unsigned;
		typedef half			Similar_Signed;
		typedef half			Similar_Float;
		typedef ushort			Similar_UInt;
		typedef short			Similar_Int;

		// type descriptor //
		enum {
			FLAGS	= (int)_ctime_hidden_::FIXED | (int)_ctime_hidden_::SIGNED | (int)_ctime_hidden_::SOFTWARE,
			UID		= __COUNTER__,
		};

		static			 type			Max()		{ return type( TypeInfo< type::half_int_t >::Max(), TypeInfo< type::half_uint_t >::Max() ); }
		static			 type			Min()		{ return type( TypeInfo< type::half_int_t >::Min(), TypeInfo< type::half_uint_t >::Max() ); }
		
		static			 type			Epsilon()	{ return type( 0, 1 ); }
		static constexpr uint			SignBit()	{ return CompileTime::SizeOf<type>::bits-1; }
		
		static const char *				Name()		{ return "Fixed16"; }

		__GX_CHECK_SIMILAR_TYPES();
	};


	template <>
	struct TypeInfo<Fixed32_t>
	{
		typedef Fixed32_t		type;
		typedef type			inner_type;

		// similar types //
		typedef Fixed32_t		Similar_Unsigned;
		typedef Fixed32_t		Similar_Signed;
		typedef float			Similar_Float;
		typedef uint			Similar_UInt;
		typedef int				Similar_Int;

		// type descriptor //
		enum {
			FLAGS	= (int)_ctime_hidden_::FIXED | (int)_ctime_hidden_::SIGNED | (int)_ctime_hidden_::SOFTWARE,
			UID		= __COUNTER__,
		};

		static			 type			Max()		{ return type( TypeInfo< type::half_int_t >::Max(), TypeInfo< type::half_uint_t >::Max() ); }
		static			 type			Min()		{ return type( TypeInfo< type::half_int_t >::Min(), TypeInfo< type::half_uint_t >::Max() ); }
		
		static			 type			Epsilon()	{ return type( 0, 1 ); }
		static constexpr uint			SignBit()	{ return CompileTime::SizeOf<type>::bits-1; }
		
		static const char *				Name()		{ return "Fixed32"; }

		__GX_CHECK_SIMILAR_TYPES();
	};


	template <>
	struct TypeInfo<Fixed64_t>
	{
		typedef Fixed64_t		type;
		typedef type			inner_type;

		// similar types //
		typedef Fixed64_t		Similar_Unsigned;
		typedef Fixed64_t		Similar_Signed;
		typedef double			Similar_Float;
		typedef ulong			Similar_UInt;
		typedef ilong			Similar_Int;

		// type descriptor //
		enum {
			FLAGS	= (int)_ctime_hidden_::FIXED | (int)_ctime_hidden_::SIGNED | (int)_ctime_hidden_::SOFTWARE,
			UID		= __COUNTER__,
		};

		static			 type			Max()		{ return type( TypeInfo< type::half_int_t >::Max(), TypeInfo< type::half_uint_t >::Max() ); }
		static			 type			Min()		{ return type( TypeInfo< type::half_int_t >::Min(), TypeInfo< type::half_uint_t >::Max() ); }
		
		static			 type			Epsilon()	{ return type( 0, 1 ); }
		static constexpr uint			SignBit()	{ return CompileTime::SizeOf<type>::bits-1; }
		
		static const char *				Name()		{ return "Fixed64"; }

		__GX_CHECK_SIMILAR_TYPES();
	};


	GX_TYPE_INFO_WITH_INNER_TYPE( Float_t,	Float_t::Float_t,	"Float" );
	GX_TYPE_INFO_WITH_INNER_TYPE( Double_t,	Double_t::Float_t,	"Double" );


}	// CompileTime

namespace GXTypes
{

	//
	// Min/Max Value
	//

	template <typename T>
	constexpr T MinValue (const T&)
	{
		return CompileTime::TypeDescriptor::template GetTypeInfo<T>::Min();
	}

	template <typename T>
	constexpr T MaxValue (const T&)
	{
		return CompileTime::TypeDescriptor::template GetTypeInfo<T>::Max();
	}



	//
	// Infinity, NaN
	//
	
	template <typename T>
	inline T  Infinity ()
	{
		return CompileTime::TypeDescriptor::template GetTypeInfo<T>::Inf();
	}

	template <typename T>
	inline T  NaN ()
	{
		return CompileTime::TypeDescriptor::template GetTypeInfo<T>::NaN();
	}
	
	template <typename T>
	inline bool IsNaN (const T& x)
	{
		return x != x;
	}

	template <typename T>
	inline bool IsInfinity (const T& x)
	{
		return not IsFinite( x );
	}

	template <typename T>
	inline bool IsFinite (const T& x)
	{
		return ( x != Infinity<T>() and x != -Infinity<T>() );
	}
	


	//
	// Check Value
	//
	/*
	template <typename T>
	inline bool CheckValue(const T& val)
	{
		return true;
	}

	template <>
	inline bool CheckValue<float>(const float& val)
	{
		return ( GXMath::Abs(val) <= _ctime_hidden_::FLOAT_MAX or
				 GXMath::Abs(val) >= _ctime_hidden_::FLOAT_MIN );
	}

	template <>
	inline bool CheckValue<double>(const double& val)
	{
		return ( GXMath::Abs(val) <= _ctime_hidden_::DOUBLE_MAX or
				 GXMath::Abs(val) >= _ctime_hidden_::DOUBLE_MIN );
	}
	*/

}	// GXTypes
}	// GX_STL
