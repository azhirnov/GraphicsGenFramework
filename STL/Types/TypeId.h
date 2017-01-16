// Copyright © 2014-2016  Zhirnov Andrey. All rights reserved.

#pragma once

#include "../BaseTypes/Types.h"
#include "../CompileTime/TypeQualifier.h"
#include "Hash.h"

namespace GX_STL
{
namespace GXTypes
{
	
	//
	// Type ID
	//

	namespace _types_hidden_
	{
		struct _TypeID : public CompileTime::PODType
		{
		private:
			usize	_value;

		public:
			_TypeID () : _value(0) {}

			bool operator == (_TypeID right) const	{ return _value == right._value; }
			bool operator != (_TypeID right) const	{ return _value != right._value; }
			bool operator >  (_TypeID right) const	{ return _value >  right._value; }
			bool operator <  (_TypeID right) const	{ return _value <  right._value; }
			bool operator >= (_TypeID right) const	{ return _value >= right._value; }
			bool operator <= (_TypeID right) const	{ return _value <= right._value; }

			usize Get () const						{ return _value; }
		};


		template <typename T>
		struct _TypeId
		{
			static _TypeID  Get ()
			{
				static usize id = (usize) &id;
				return (_TypeID const &) id;
			}
		};

	}	// _types_hidden_


	typedef _types_hidden_::_TypeID		TypeId_t;


	template <typename T>
	static TypeId_t  TypeId ()
	{
		return _types_hidden_::_TypeId<T>::Get();
	}

	template <typename T>
	static TypeId_t  TypeIdOf (const T&)
	{
		return TypeId<T>();
	}
	

	template <>
	struct Hash< TypeId_t > : private Hash< usize >
	{
		typedef TypeId_t			key_t;
		typedef Hash< usize >		base_t;
		typedef base_t::result_t	result_t;

		result_t operator () (const key_t &x) const
		{
			return base_t::operator ()( x.Get() );
		}
	};
	
}	// GXTypes
}	// GX_STL
