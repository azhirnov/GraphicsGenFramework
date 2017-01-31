// Copyright © 2014-2017  Zhirnov Andrey. All rights reserved.

#pragma once

#include "Engine/STL/OS/Posix/AtomicOp.h"
#include "Engine/STL/OS/Windows/AtomicOp.h"

namespace GX_STL
{
namespace GXTypes
{

	//
	// Atomic Pointer
	//

	template <typename T>
	struct AtomicPtr
	{
	// types
	public:
		typedef AtomicPtr<T>	Self;
		typedef T *				value_t;
		typedef OS::AtomicOp	Op;


	// variables
	private:
		volatile usize	_ptr;


	// methods
	public:
		AtomicPtr (GX_DEFCTOR) : _ptr(0) {}
		
		explicit AtomicPtr (value_t ptr) : _ptr( ReferenceCast<usize>(ptr) ) {}
		explicit AtomicPtr (const AtomicPtr<T> &other) : _ptr(other.Get()) {}
		
		operator const volatile vaue_t () const		{ return Get(); }
		
		vaue_t		Get ()	const					{ return ReferenceCast< value_t >( Op::Get<usize>( _ptr ) ); }

		void		Set (vaue_t right)				{ Op::Set<usize>( _ptr, ReferenceCast< usize >( right ) ); }
		
		value_t operator -> () const
		{
			value_t ptr = Get();
			ASSUME( ptr != null );
			return ptr;
		}

		T & operator * () const
		{
			value_t ptr = Get();
			ASSUME( ptr != null );
			return *ptr;
		}

		bool IsNull () const
		{
			return ( Get() == null );
		}

		bool IsNotNull () const
		{
			return ( Get() != null );
		}
	};


}	// GXTypes
}	// GX_STL
