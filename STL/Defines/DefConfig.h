// Copyright © 2014-2016  Zhirnov Andrey. All rights reserved.

#pragma once

// Define this in project preprocessor only //
//#define __GX_DEBUG__
//#define __GX_FAST__
//#define __GX_BUILD__
//#define __GX_BUILD_STATIC_LIB__

//-------------------------------------------------------------------



// Options //

// compile time calculation of types limits, otherwise used constant values.
//#define GX_MATH_REAL_LIMITS


// use single or double precision float-point types.
#define GX_REAL_TYPE_SIZE	32


// disable try/catch sections.
#define GX_CATCHING_EXCEPTIONS_DISABLE


// all string must be in unicode.
// (TODO)
//#define GX_UNICODE


// enable temporary code, otherwise enabled static asserts instead of temporary code.
//#define GX_TEMP_CODE


// use SDL
//#define PLATFORM_SDL


// runtime error checks
#if !defined( __GX_FAST__ )
#	define __GX_ERROR_CHECKS__
#endif


// log show warning window in runtime
#if defined( __GX_DEBUG__ )
#	define __GX_LOG_SHOW_WARNING_ENABLED__
#endif


// log to file/console enabled
#if !defined( __GX_FAST__ )
#	define __GX_LOG_WRITE_ENABLED__
#endif


// defines a log output method
#ifndef LOG_EXT

#	if defined( __GX_LOG_SHOW_WARNING_ENABLED__ ) and defined( PLATFORM_WINDOWS )
#		define __LOG_SHOW_WARNING( _type_, _text_ )		if ( ::GX_STL::ELog::IsError( _type_ ) ) { WARNING( _text_ ); }
#	else
#		define __LOG_SHOW_WARNING( _type_, _text_ )
#	endif

#	if !defined( __GX_LOG_WRITE_ENABLED__ )
#		define LOG_EXT( _text_, _type_, _file_, _line_ )
#	else
#		define LOG_EXT( _text_, _type_, _file_, _line_ )	\
		{ \
			::GX_STL::__write_to_log( _text_, _type_, _file_, _line_ ); \
			\
			__LOG_SHOW_WARNING( _type_, _text_ ); \
		}
#	endif	// __GX_FAST__

#endif	// LOG_EXT

#define LOG( _text_, _type_ )		LOG_EXT( _text_, _type_, __FILE__, __LINE__ )


// files
//#define GX_LZMA_ENABLED
//#define GX_ZLIB_ENABLED

// scripts
//#define GX_ANGELSCRIPT_ENABLED

// database
//#define GX_SQLITE_DATABASE_ENABLED

// Java
#if defined( PLATFORM_ANDROID )
#	define GX_JNI_ENABLED
#endif

//-------------------------------------------------------------------



// Optimizations //

namespace GX_STL
{
namespace GlobalConst
{
	// size of static buffer for delegate and function interface
	static const int	STL_DelegateInterfaceSize	= sizeof(void*) * 6;
	static const int	STL_FunctionInterfaceSize	= sizeof(void*) * 16;

}	// GlobalConst
}	// GX_STL

//-------------------------------------------------------------------



// Checks //
#ifndef GX_RVALUE_REFERENCE_SUPPORTED
#	error right value reference must be supported!
#endif

#ifndef GX_VA_ARGS_IN_MACRO_SUPPORTED
#	error variable arguments in macro must be supported!
#endif

#ifndef GX_AUTO_SUPPERTED
#	error auto must be supported!
#endif
