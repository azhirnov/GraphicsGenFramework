// Copyright © 2014-2016  Zhirnov Andrey. All rights reserved.
//
// see http://en.cppreference.com/w/cpp/compiler_support
//

#ifdef  COMPILER_MSVC

#pragma once


// Version //

#define COMPILER_VERSION		_MSC_VER

/*
	MSVC++ 15.0 _MSC_VER == 2000 (Visual Studio 2017)
	MSVC++ 14.0 _MSC_VER == 1900 (Visual Studio 2015)
	MSVC++ 12.0 _MSC_VER == 1800 (Visual Studio 2013)
	MSVC++ 11.0 _MSC_VER == 1700 (Visual Studio 2012)
	MSVC++ 10.0 _MSC_VER == 1600 (Visual Studio 2010)
	MSVC++ 9.0  _MSC_VER == 1500 (Visual Studio 2008)
	MSVC++ 8.0  _MSC_VER == 1400 (Visual Studio 2005)
	MSVC++ 7.1  _MSC_VER == 1310 (Visual Studio 2003)
	MSVC++ 7.0  _MSC_VER == 1300
	MSVC++ 6.0  _MSC_VER == 1200
	MSVC++ 5.0  _MSC_VER == 1100
*/

//-------------------------------------------------------------------



// Export //

#define GX_DLL_EXPORT			__declspec( dllexport )
#define GX_DLL_IMPORT			__declspec( dllimport )
#define GX_DLL_PRIVATE			


//-------------------------------------------------------------------


// Warnings //

#pragma warning (push)

// disable warnings
#pragma warning (disable: 4996)			// this function or variable may be unsafe
#pragma warning (disable: 4503)			// decorated name length exceeded, name was truncated (for debug info???)
#pragma warning (disable: 4838)			// conversion from int to uint
//#pragma warning (disable: 4251)		// class '%class_name%' must have dll-interface...


// for level 4
#pragma warning (disable: 4201)			// nonstandard extension used : nameless struct/union
#pragma warning (disable: 4459)			// declaration of '...' hides global declaration
#pragma warning (disable: 4127)			// conditional expression is constant
#pragma warning (disable: 4458)			// declaration of '...' hides class member
//#pragma warning (disable: 4481)		// nonstandard extension used
//#pragma warning (disable: 4127)		// conditional expression is constant
//#pragma warning (disable: 4512)		// assignment operator could not be generated
//#pragma warning (disable: 4480)		// nonstandard extension used: specifying underlying type for enum


// not all enums listed in switch block
//#pragma warning (error: 4061)
//#pragma warning (error: 4062)
//#pragma warning (error: 4063)			// case 'number' is not a valid value for switch of enum 'type'


// warning to errors
#pragma warning (error: 4018)			// signed/unsigned mismatch
#pragma warning (error: 4287)			// unsigned/negative constant mismatch
#pragma warning (error: 4715)			// 'function' : not all control paths return a value
#pragma warning (error: 4172)			// returning address of local variable or temporary
#pragma warning (error: 4099)			// 'identifier' : type name first seen using 'objecttype1' now seen using 'objecttype2'
#pragma warning (error: 4263)			// 'function' : member function does not override any base class virtual member function
#pragma warning (error: 4264)			// 'virtual_function' : no override available for virtual member function from base 'class'; function is hidden
#pragma warning (error: 4266)			// 'function' : no override available for virtual member function from base 'type'; function is hidden
#pragma warning (error: 4552)			// 'operator' : operator has no effect; expected operator with side-effect
#pragma warning (error: 4553)			// 'operator' : operator has no effect; did you intend 'operator'?


// error in debug, warning in release
#ifdef __GX_DEBUG__
#	pragma warning (error: 4189)		// 'variable': local variable is initialized but not referenced
#else
//#	pragma warning (1: 4189)			// 'variable': local variable is initialized but not referenced
#endif


// to check some errors
#if 1
#	pragma warning (disable: 4100)		// unreferenced formal parameter
#	pragma warning (disable: 4310)		// cast truncates constant value
#	pragma warning (disable: 4245)		// 'return': conversion from signed to unsigned
#	pragma warning (disable: 4365)		// signed/unsigned mismatch
#	pragma warning (disable: 4389)		// '==': signed/unsigned mismatch
#	pragma warning (disable: 4505)		// unreferenced local function has been removed
#else
#	pragma warning (error: 4100)		// unreferenced formal parameter
#	pragma warning (error: 4310)		// cast truncates constant value
#	pragma warning (error: 4245)		// 'return': conversion from signed to unsigned
#	pragma warning (error: 4365)		// signed/unsigned mismatch
#	pragma warning (error: 4389)		// '==': signed/unsigned mismatch
#	pragma warning (error: 4505)		// unreferenced local function has been removed
#endif


#undef  _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS

#define _ALLOW_KEYWORD_MACROS

//-------------------------------------------------------------------



// Compiler Settings //

// function name as string
#define	GX_FUNCTION_NAME			__FUNCTION__

// set align of struct in bytes
#if COMPILER_VERSION >= 1900
#	define	GX_ALIGN( ... )			alignas( __VA_ARGS__ )
#else
#	define	GX_ALIGN( ... )			__declspec( align( __VA_ARGS__ ) )
#endif

// returns align of type in bytes
#if COMPILER_VERSION >= 1900
#define GX_ALIGN_OF( ... )			alignof( __VA_ARGS__ )
#else
#define GX_ALIGN_OF( ... )			__alignof( __VA_ARGS__ )
#endif

// always inline function
#define	GX_FORCE_INLINE				__forceinline

// never inline function
#define GX_NO_INLINE				__declspec( noinline )

// no code be generated for the arguments
// Warning: no analogs for other compilers!
#define	GX_NOOP						__noop

// value is always true
#define	GX_ASSUME( ... )			__assume( __VA_ARGS__ )

// don't use virtual table for struct/class
// Warning: no analogs for other compilers!
#define GX_NO_VTABLE				__declspec( novtable )

// function return unique pointer
#define	GX_RESTRICT					__declspec( restrict )

// function does not modify or reference visible global state and only modifies the memory
// pointed to directly by pointer parameters (first-level indirections). (MDSN)
// Warning: no analogs for other compilers!
#define	GX_NO_ALIAS					__declspec( noalias )

// type qualifier for pointer types
// another pointer cannot be used to access the data pointed to by the __restrict pointer. (MDSN)
// Warning: no analogs for other compilers!
#define	GX_RESTRICT_PTR				__restrict

// declare variable is local variable for thread
// in WinXP it is for static linking only!
#if COMPILER_VERSION >= 1900
#	define GX_THREAD_LOCAL			thread_local
#else
#	define GX_THREAD_LOCAL			__declspec( thread )
#endif

// function not returns any value
// compilation warning for this function never generated
#define GX_NO_RETURN				__declspec( noreturn )

//
#define __CDECL						__cdecl

// null pointer
#if COMPILER_VERSION > 1500
#	define GX_NULL_PTR				nullptr
#endif

// pragma directive inside the macro
#define GX_PRAGMA( ... )			__pragma( __VA_ARGS__ )

// Debugger already check this values and don't needed to check in code!
// check pointer to null or magic numbers
// see: https://en.wikipedia.org/wiki/Magic_number_(programming)#Magic_debug_values
// or http://www.nobugs.org/developer/win32/debug_crt_heap.html#table
/*#ifdef __GX_DEBUG__
#	define GX_CHECK_POINTER( _ptr_ ) ( \
		(_ptr_ != 0) || \
		(static_cast<void const * const >(_ptr_) == reinterpret_cast<void const * const>(0xABABABABABABABABull)) || \
		(static_cast<void const * const >(_ptr_) == reinterpret_cast<void const * const>(0xBAADF00DBAADF00Dull)) || \
		(static_cast<void const * const >(_ptr_) == reinterpret_cast<void const * const>(0xCCCCCCCCCCCCCCCCull)) || \
		(static_cast<void const * const >(_ptr_) == reinterpret_cast<void const * const>(0xCDCDCDCDCDCDCDCDull)) || \
		(static_cast<void const * const >(_ptr_) == reinterpret_cast<void const * const>(0xDDDDDDDDDDDDDDDDull)) || \
		(static_cast<void const * const >(_ptr_) == reinterpret_cast<void const * const>(0xDEADDEADDEADDEADull)) || \
		(static_cast<void const * const >(_ptr_) == reinterpret_cast<void const * const>(0xFEEEFEEEFEEEFEEEull)) || \
		(static_cast<void const * const >(_ptr_) == reinterpret_cast<void const * const>(0xFDFDFDFDFDFDFDFDull)) )
#else
#	define GX_CHECK_POINTER( _ptr_ )	(_ptr_ != 0)
#endif	// __GX_DEBUG__*/

//-------------------------------------------------------------------



// C++ 11/14 features //

// C++11, constexpr
#if COMPILER_VERSION > 1800
#	define GX_CONSTEXPR_SUPPORTED			1
#endif

// final, constexpr
#if COMPILER_VERSION <= 1800
//	supported: decltype, override
#	define final
#	define constexpr
#	define noexcept							throw()
#endif


// override, decltype
#if COMPILER_VERSION <= 1500
#	define override
#	define decltype( _value_ )				STATIC_WARNING( "decltype not supported" )
#endif


// variable arguments in macro
#if COMPILER_VERSION > 1500
#	define GX_VA_ARGS_IN_MACRO_SUPPORTED	1
#	define ARGS(...)						__VA_ARGS__
#	define __GX_EMPTY_ARGS(...)		
#endif


// right value reference
#if COMPILER_VERSION >= 1600
#	define GX_RVALUE_REFERENCE_SUPPORTED	1
#endif


// auto
#if COMPILER_VERSION >= 1600
#	define GX_AUTO_SUPPERTED				1
#endif


// thread safe local static variable
#if COMPILER_VERSION >= 1600
#	define GX_THREADSAFE_STATIC_SUPPORTED	1
#endif


// extern template
#if COMPILER_VERSION >= 1500
#	define GX_EXTERN_TEMPLATE_SUPPORTED		1
#endif


// lambda
#if COMPILER_VERSION >= 1700
#	define GX_LAMBDA_SUPPORTED				1
#endif


// Unrestricted unions 
#if COMPILER_VERSION >= 1900
#	define GX_UNRESTRICTED_UNIONS_SUPPORTED	1
#endif


// Variadic templates 
#if COMPILER_VERSION >= 1800
#	define GX_VARIADIC_TEMPLATES_SUPPORTED	1
#endif


// Non-static data member initializers 
#if COMPILER_VERSION >= 1800
#	define GX_FIELD_INITIALIZERS_SUPPORTED	1
#endif


// static assert
#if COMPILER_VERSION < 1600
#	define  __GX_UNIQUE_STATIC_ASSERT_NAME( _a_ )	_a_##__LINE__
#	define	static_assert( _condition_, _text_ )	typedef int	__GX_UNIQUE_STATIC_ASSERT_NAME( __static_assert_ ) [ (_condition_) ? 1 : 0 ]
#endif


// deprecated attribute
#if COMPILER_VERSION >= 2000
#	define GX_DEPRECATED( _reason_ )	[[ deprecated(_reason_) ]]
#else
#	define GX_DEPRECATED( _reason_ )
#endif

//-------------------------------------------------------------------



// Other //

//#define GX_BREAK_POINT()		__asm { int 3 }
#define GX_BREAK_POINT()		__debugbreak()


#ifndef and
#	define and		&&
#endif

#ifndef or
#	define or		||
#endif

#ifndef not
#	define not		!
#endif


// Intrin for MSVC and Windows Platform //
#include <intrin.h>

#endif	// COMPILER_MSVC