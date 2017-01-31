// Copyright © 2014-2017  Zhirnov Andrey. All rights reserved.

#pragma once

#include "Engine/STL/Common/TypeId.h"
#include "Engine/STL/CompileTime/TypeTraits.h"
#include "Ptr.h"

namespace GX_STL
{
namespace GXTypes
{

	//
	// Variant Reference
	//

	struct VariantRef
	{
	// variables
	private:
		mutable void *	_reference;
		TypeId_t		_typeId;


	// methods
	public:
		VariantRef (GX_DEFCTOR) : _reference(null)
		{}

		
		template <typename T>
		forceinline VariantRef (T &ref) :
			_reference( static_cast<void *>( &ref ) ),
			_typeId( TypeId< TypeTraits::RemoveConstVolatile<T> >() )
		{}


		template <typename T>
		forceinline static VariantRef  From (T &ref)
		{
			return VariantRef( ref );
		}
		
		template <typename T>
		forceinline static VariantRef  From (const T *ptr)
		{
			return VariantRef( *const_cast<T *>( ptr ) );
		}

		forceinline static VariantRef  FromVoid (void *ptr, TypeId_t typeId)
		{
			VariantRef	ref;
			ref._reference	= ptr;
			ref._typeId		= typeId;
			return ref;
		}


		// Warning: this is not safe!
		// const reference may be a reference to a temporary object.
		template <typename T>
		forceinline static VariantRef  FromConst (const T& ref)
		{
			return VariantRef( const_cast<T &>( ref ) );
		}


		template <typename T>
		forceinline bool IsType () const
		{
			return TypeId< TypeTraits::RemoveConstVolatile<T> >() == _typeId;
		}

		template <typename T>
		forceinline bool IsTypeAs (const T&) const
		{
			return IsType<T>();
		}


		forceinline TypeId_t	GetValueTypeId () const
		{
			return _typeId;
		}


		template <typename T>
		forceinline T &			Get () const
		{
			return *GetPtr<T>();
		}

		template <typename T>
		forceinline T *			GetPtr () const
		{
			CHECK( IsType<T>() );
			return static_cast<T *>( _reference );
		}

		/*void *		RawPtr () const
		{
			return _reference;
		}*/


		template <typename T>
		forceinline void CopyTo (OUT T &value) const
		{
			value = Get<T>();
		}
	};

	
	/*template <>
	struct Hash< VariantRef > : private Hash< void* >
	{
		typedef VariantRef			key_t;
		typedef Hash< void* >		base_t;
		typedef base_t::result_t	result_t;

		result_t operator () (const key_t &x) const
		{
			return base_t::operator ()( x.RawPtr() );
		}
	};*/

}	// GXTypes
}	// GX_STL
