// Copyright © 2014-2016  Zhirnov Andrey. All rights reserved.

#pragma once

#include "Defines.h"
#include "../Log/ELog.h"

// helpers //
#define __GX_LOG( _text_, _level_ ) \
	LOG( (::GX_STL::GXTypes::String( "Error: " ) << _text_ << ", in function: " GX_FUNCTION_NAME ".").cstr(), (_level_) )



// release & debug //
#if !defined( __GX_ERROR_CHECKS__ )

#	define __GX_RETURN_ERR( _text_, _ret_, ... ) \
		{ return (_ret_); }

#	define __GX_CHECK_ERRX( _check_, _text_, _level_, _ret_ ) \
		{ (void)( _check_ ); }

#	define __GX_CHECK_WARN( _check_, _text_, _level_ ) \
		{ (void)( _check_ ); }

#else
		
#	define __GX_RETURN_ERR( _text_, _ret_, ... ) \
		{ __GX_LOG( _text_, ::GX_STL::ELog::_DefError );  return (_ret_); }

#	define __GX_CHECK_ERRX( _check_, _text_, _level_, _ret_ ) \
		{ \
			if ( not (_check_) ) { \
				__GX_LOG( _text_, (_level_) ); \
				return (_ret_); \
			} \
		}

#	define __GX_CHECK_WARN( _check_, _text_, _level_ ) \
		{ \
			if ( not (_check_) ) { \
				__GX_LOG( _text_, (_level_) ); \
			} \
		}

#endif	// __GX_FAST__
		

// return error
#define RETURN_ERR( ... ) \
	__GX_RETURN_ERR( AUXDEF_GET_FIRST( __VA_ARGS__ ), AUXDEF_GET_SECOND( __VA_ARGS__, false ) )


// check and return error if false
#define __GX_CHECK_ERR( _func_, _ret_, ... ) \
	__GX_CHECK_ERRX( (_func_), GX_TO_ANSI_STRING( _func_ ), ::GX_STL::ELog::_DefError, (_ret_)  )

#define CHECK_ERR( ... ) \
	__GX_CHECK_ERR( AUXDEF_GET_FIRST( __VA_ARGS__ ), AUXDEF_GET_SECOND( __VA_ARGS__, false ) )


// check only
#define CHECK( _func_ ) \
	__GX_CHECK_WARN( (_func_), GX_TO_ANSI_STRING( _func_ ), ::GX_STL::ELog::_DefError )


// debug only //
#if defined( __GX_DEBUG__ )

	// check and return error if false
#	define __GX_DBG_CHECK_ERR( _func_, _ret_, ... ) \
		__GX_CHECK_ERRX( (_func_), GX_TO_ANSI_STRING( _func_ ), ::GX_STL::ELog::_DefWarning, (_ret_)  )

#	define DBG_CHECK_ERR( ... ) \
		__GX_DBG_CHECK_ERR( AUXDEF_GET_FIRST( __VA_ARGS__ ), AUXDEF_GET_SECOND( __VA_ARGS__, false ) )
	
	// check only
#	define DBG_CHECK( ... ) \
		__GX_CHECK_WARN( (__VA_ARGS__), GX_TO_ANSI_STRING( __VA_ARGS__ ), ::GX_STL::ELog::_DefWarning )

#else
	
	// check and return error if false
#	define DBG_CHECK_ERR( ... )						{}

	// check only
#	define DBG_CHECK( ... )							{}

#endif



//----------------------- Critical Errors -------------------------//

// Check Memory Allocation Exception
#define CHECK_ALLOC_RE( _func_, _onException_, _text_ ) \
	TRY { \
		_func_; \
	} CATCH( ..., \
	{ \
		LOG_WARNING( _text_, ::GX_STL::ELog::Error ); \
		_onException_; \
	} )

#define CHECK_ALLOC_RT( _func_, _text_, _ret_ )	CHECK_ALLOC_RE( _func_, return _ret_, _text_ )
#define CHECK_ALLOC_B( _func_, _res_ )			CHECK_ALLOC_RE( { _res_ = true; _func_; }, _res_ = false, "Can't allocate memory" )
#define CHECK_ALLOC_T( _func_, _text_ )			CHECK_ALLOC_RT( _func_, _text_, false )
#define CHECK_ALLOC_R( _func_, _ret_ )			CHECK_ALLOC_RT( _func_, "Can't allocate memory in function: \"" TOSTRING(_func_) "\"", _ret_ )
#define CHECK_ALLOC( _func_ )					CHECK_ALLOC_R( _func_, false )

