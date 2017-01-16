// Copyright © 2014-2016  Zhirnov Andrey. All rights reserved.

#pragma once

#include "OSWindows.h"

#ifdef PLATFORM_WINDOWS


namespace GX_STL
{
namespace _platform_
{
	using namespace GXTypes;
	using namespace winapi;

	
	//
	// Atomic Operations
	//

	struct AtomicOp
	{
		// type cast //
#		define type_cast( _val_ )	& ReferenceCast< volatile CompileTime::Similar<T>::Signed,	volatile T >( _val_ )
#		define itype_cast( _val_ )  & ReferenceCast< volatile CompileTime::Similar<T>::Int,		volatile T >( _val_ )
#		define ctype_cast( _val_ )	ReferenceCast< const    CompileTime::Similar<T>::Int,		const    T >( _val_ )
#		define rtype_cast( _val_ )	ReferenceCast< const T, const CompileTime::Similar<T>::Int >( _val_ )


		// T //
		template <typename T>
		static T  Inc (volatile T & left) {
			return Inc( type_cast( left ) );
		}

		template <typename T>
		static T  Dec (volatile T & left) {
			return Dec( type_cast( left ) );
		}

		template <typename T>
		static T  Add (volatile T & left, const T& right) {
			return Add( type_cast( left ), ctype_cast( right ) );
		}

		template <typename T>
		static T  Sub (volatile T & left, const T& right) {
			return Sub( type_cast( left ), ctype_cast( right ) );
		}

		template <typename T>
		static T  Set (volatile T & left, const T& right) {
			return rtype_cast( Set( itype_cast( left ), ctype_cast( right ) ) );
		}

		template <typename T>
		static T  CmpEx (volatile T & left, const T& right, const T& cmp) {
			return rtype_cast( CmpEx( itype_cast( left ), ctype_cast( right ), ctype_cast( cmp ) ) );
		}

		template <typename T>
		static T  Or (volatile T & left, const T& right) {
			return Or( type_cast( left ), ctype_cast( right ) );
		}

		template <typename T>
		static T  Xor (volatile T & left, const T& right) {
			return Xor( type_cast( left ), ctype_cast( right ) );
		}

		template <typename T>
		static T  And (volatile T & left, const T& right) {
			return And( type_cast( left ), ctype_cast( right ) );
		}
		
		template <typename T>
		static T Get (volatile T const & left) {
			// TODO: memory barrier for non x86 processors 
			return left;
		}

#		undef  type_cast
#		undef  itype_cast
#		undef  ctype_cast
#		undef  rtype_cast


		// byte //
		static byte  Inc (volatile byte * ptr)							{ return Add( ptr, 1 ); }
		static byte  Dec (volatile byte * ptr)							{ return Add( ptr, -1 ); }
		static byte  Add (volatile byte * ptr, byte val)				{ return ::_InterlockedExchangeAdd8( (volatile char *)ptr, val ); }
		static byte  Sub (volatile byte * ptr, byte val)				{ return ::_InterlockedExchangeAdd8( (volatile char *)ptr, -val ); }
		static byte  Set (volatile byte * ptr, byte val)				{ return ::_InterlockedExchange8( (volatile char *)ptr, val ); }
		static byte  CmpEx (volatile byte * ptr, byte val, byte cmp)	{ return ::_InterlockedCompareExchange8( (volatile char *)ptr, val, cmp ); }
		static byte  Or  (volatile byte * ptr, byte val)				{ return ::_InterlockedOr8( (volatile char *)ptr, val ); }
		static byte  Xor (volatile byte * ptr, byte val)				{ return ::_InterlockedXor8( (volatile char *)ptr, val ); }
		static byte  And (volatile byte * ptr, byte val)				{ return ::_InterlockedAnd8( (volatile char *)ptr, val ); }
		
		// short //
		static short Inc (volatile short * ptr)							{ return ::_InterlockedIncrement16( (volatile short *)ptr ); }
		static short Dec (volatile short * ptr)							{ return ::_InterlockedDecrement16( (volatile short *)ptr ); }
		static short Add (volatile short * ptr, short val)				{ return ::_InterlockedExchangeAdd16( (volatile short *)ptr, val ); }
		static short Sub (volatile short * ptr, short val)				{ return ::_InterlockedExchangeAdd16( (volatile short *)ptr, -val ); }
		static short Set (volatile short * ptr, short val)				{ return ::_InterlockedExchange16( (volatile short *)ptr, val ); }
		static short CmpEx (volatile short * ptr, short val, short cmp)	{ return ::_InterlockedCompareExchange16( (volatile short *)ptr, val, cmp ); }
		static short Or  (volatile short * ptr, short val)				{ return ::_InterlockedOr16( (volatile short *)ptr, val ); }
		static short Xor (volatile short * ptr, short val)				{ return ::_InterlockedXor16( (volatile short *)ptr, val ); }
		static short And (volatile short * ptr, short val)				{ return ::_InterlockedAnd16( (volatile short *)ptr, val ); }

		// int //
		static int Inc (volatile int * ptr)								{ return ::_InterlockedIncrement( (volatile long *)ptr ); }
		static int Dec (volatile int * ptr)								{ return ::_InterlockedDecrement( (volatile long *)ptr ); }
		static int Add (volatile int * ptr, int val)					{ return ::_InterlockedExchangeAdd( (volatile long *)ptr, val ); }
		static int Sub (volatile int * ptr, int val)					{ return ::_InterlockedExchangeAdd( (volatile long *)ptr, -val ); }
		static int Set (volatile int * ptr, int val)					{ return ::_InterlockedExchange( (volatile long *)ptr, val ); }
		static int CmpEx (volatile int * ptr, int val, int cmp)			{ return ::_InterlockedCompareExchange( (volatile long *)ptr, val, cmp ); }
		static int Or  (volatile int * ptr, int val)					{ return ::_InterlockedOr( (volatile long *)ptr, val ); }
		static int Xor (volatile int * ptr, int val)					{ return ::_InterlockedXor( (volatile long *)ptr, val ); }
		static int And (volatile int * ptr, int val)					{ return ::_InterlockedAnd( (volatile long *)ptr, val ); }
		

#	if PLATFORM_BITS == 32
		
		// pointers //
		static void * SetPtr (volatile void ** ptr, void * val)			{ return (void *)::_InterlockedExchange( (volatile long *)ptr, (long)val ); }
		static void * CmpExP (volatile void **ptr, void *val, void *cmp){ return (void *)::_InterlockedCompareExchange( (volatile long *)ptr, (long)val, (long)cmp ); }

		// ilong //
		static ilong Inc (volatile ilong * ptr)							{ return InterlockedIncrement64( ptr ); }
		static ilong Dec (volatile ilong * ptr)							{ return InterlockedDecrement64( ptr ); }
		static ilong Add (volatile ilong * ptr, ilong val)				{ return InterlockedExchangeAdd64( ptr, val ); }
		static ilong Sub (volatile ilong * ptr, ilong val)				{ return InterlockedExchangeAdd64( ptr, -val ); }
		static ilong Set (volatile ilong * ptr, ilong val)				{ return InterlockedExchange64( ptr, val ); }
		static ilong CmpEx (volatile ilong * ptr, ilong val, ilong cmp)	{ return InterlockedCompareExchange64( ptr, val, cmp ); }
		static ilong Or  (volatile ilong * ptr, ilong val)				{ return InterlockedOr64( ptr, val ); }
		static ilong Xor (volatile ilong * ptr, ilong val)				{ return InterlockedXor64( ptr, val ); }
		static ilong And (volatile ilong * ptr, ilong val)				{ return InterlockedAnd64( ptr, val ); }

#	endif	// 32

#	if PLATFORM_BITS == 64

		// pointers //
		static void * SetPtr (void* volatile* ptr, void * val)			{ return ::_InterlockedExchangePointer( ptr, val ); }
		static void * CmpExP (void* volatile* ptr, void *val, void *cmp){ return ::_InterlockedCompareExchangePointer( ptr, val, cmp ); }
		
		// ilong //
		static ilong Inc (volatile ilong * ptr)							{ return ::_InterlockedIncrement64( ptr ); }
		static ilong Dec (volatile ilong * ptr)							{ return ::_InterlockedDecrement64( ptr ); }
		static ilong Add (volatile ilong * ptr, ilong val)				{ return ::_InterlockedExchangeAdd64( ptr, val ); }
		static ilong Sub (volatile ilong * ptr, ilong val)				{ return ::_InterlockedExchangeAdd64( ptr, -val ); }
		static ilong Set (volatile ilong * ptr, ilong val)				{ return ::_InterlockedExchange64( ptr, val ); }
		static ilong CmpEx (volatile ilong * ptr, ilong val, ilong cmp)	{ return ::_InterlockedCompareExchange64( ptr, val, cmp ); }
		static ilong Or  (volatile ilong * ptr, ilong val)				{ return ::_InterlockedOr64( ptr, val ); }
		static ilong Xor (volatile ilong * ptr, ilong val)				{ return ::_InterlockedXor64( ptr, val ); }
		static ilong And (volatile ilong * ptr, ilong val)				{ return ::_InterlockedAnd64( ptr, val ); }

#	endif	// 64

	};

}	// _platform_
}	// GX_STL

#endif	// PLATFORM_WINDOWS