// Copyright © 2014-2016  Zhirnov Andrey. All rights reserved.

#pragma once

#include "../Defines/Init.h"

namespace GX_STL
{
namespace GXTypes
{

	typedef	wchar_t				wchar;
	typedef signed char			byte;
	//typedef signed short		short;
	//typedef signed int		int;
	typedef	signed long long	ilong;

	typedef unsigned char		ubyte;
	typedef	unsigned short		ushort;
	typedef unsigned int		uint;
	typedef unsigned long long	ulong;


// pointer & size type
#if PLATFORM_BITS == 16
	typedef unsigned short		usize;
	typedef signed short		isize;
#elif PLATFORM_BITS == 32
	typedef unsigned int		usize;
	typedef signed int			isize;
#elif PLATFORM_BITS == 64
	typedef unsigned long long	usize;
	typedef signed long long	isize;
#endif


	// Null Pointer
#	if not defined( GX_NULL_PTR )

	namespace _hidden_
	{

		struct _TNullPointer
		{
			void operator & () {}
			void operator & () const {}

			template<class T>
			operator T * () const
			{
				return (T *)0;
			}
	
			template<class C, class T>
			operator T C::* () const
			{
				return 0;
			}
		};

		const _TNullPointer	__nullptr = {};

	#	define	GX_NULL_PTR		::_hidden_::__nullptr

	}	// _hidden_

#	endif	// GX_NULL_PTR

	
#	define null	GX_NULL_PTR

	
	// For empty constructors
	struct Uninitialized {};


	STATIC_ASSERT( sizeof(void *) == sizeof(usize),	"unsigned size type is incorrect" );
	STATIC_ASSERT( sizeof(void *) == sizeof(isize),	"signed size type is incorrect" );


}	// GXTypes
}	// GX_STL
