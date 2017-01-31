// Copyright © 2014-2017  Zhirnov Andrey. All rights reserved.
/*
	Serializable object format:

		file id = _FILE_BEGIN	(1)
		version					(4)

			struct type id = STRUCT	(1)
			object id				(4)

				field type id			(1)
				(additional types ids)	(...)
				field value				(...)

			struct end				(1)

		file end				(1)
*/

#pragma once

#include "SerializableValue.h"
#include "SerializableObject.h"

namespace Engine
{
namespace Base
{

	//
	// Serializer
	//

	class Serializer
	{
	// types
	public:
		typedef Serializer		Self;

		typedef uint			usize_t;
		typedef ubyte			vector_count_t;		// type that contains vector and matrix elements count
		typedef ubyte			value_typeid_t;

		
		template <typename T, typename A>
		struct ArrayElementBinder
		{
		private:
			A const &	arr;
			T const &	elem;
			usize_t		index;

		public:
			ArrayElementBinder (A const& arr, T const& elem) :
				arr(arr), elem(elem), index(-1)
			{
				usize idx = -1;
				arr.Find( idx, elem );
				index = idx;
			}
				
			ArrayElementBinder (const ArrayElementBinder &other) :
				arr(other.arr), elem(other.elem), index(other.index)
			{}

			usize_t const&	Get ()	const	{ return index; }
		};
	
		struct Binder
		{
			Ptr< Self >		_s;

			Binder (Self *s) : _s(s)	{}

			template <typename T>
			Binder & operator , (const T& value)		{ _s->Bind( value );  return *this; }
		};


	// variables
	private:
		WFilePtr				_file;

		const value_typeid_t	_end;


	// methods
	public:
		explicit
		Serializer (const WFilePtr &file, bool readOnly = false, uint version = 1) :
			_file( file ),
			_end( ESerializableValue::_FILE_END )
		{
			const value_typeid_t	begin = readOnly ? ESerializableValue::_FILE_BEGIN_READ_ONLY : ESerializableValue::_FILE_BEGIN;

			_file->Write( begin );
			_file->Write( version );
		}


		Serializer (const Serializer &other, SerializableObjectID objID) :
			_file( other._file ),
			_end( ESerializableValue::_STRUCT_END )
		{
			const value_typeid_t	obj_type = ESerializableValue::STRUCT;

			_file->Write( obj_type );
			_file->Write( objID );
		}


		~Serializer ()
		{
			_file->Write( _end );
		}


		template <typename T>
		Self & Parent (const T& obj)
		{
			const value_typeid_t	parent = ESerializableValue::_STRUCT_PARENT;

			_file->Write( parent );
			obj.Serialize( *this );

			return *this;
		}

		
		template <typename T>
		Self & Bind (const T& field)
		{
			_Save( field );
			return *this;
		}


		Binder	GetBinder ()		{ return Binder( this ); }


	private:
		template <typename T>
		static constexpr value_typeid_t  _GetTypeOf (const T& t)
		{
			return ESerializableValue::GetValueTypeOf( t );
		}


		template <typename T>
		static constexpr value_typeid_t  _GetType ()
		{
			return _GetTypeOf( * (const T *) null );
		}


		template <typename T, typename S, typename A>
		void _Save (const Array<T,S,A> &value) const
		{
			return _SaveArray( value );
		}
			

		template <typename T, usize C, typename S>
		void _Save (const StaticArray<T,C,S> &value) const
		{
			return _SaveArray( value );
		}
	

		template <typename K, typename T, typename S, typename A>
		void _Save (const Map<K,T,S,A> &value) const
		{
			const value_typeid_t	key_type = _GetType<K>();
			const value_typeid_t	val_type = _GetType<T>();

			_file->Write( _GetTypeOf( value ) );
			_file->Write( key_type );
			_file->Write( val_type );
			
			_file->Write( (usize_t) value.Count() );
			
			typedef typename Map<K,T,S,A>::pair_t	MapPair_t;

			_SavePodHelper< ESerializableValue::IsPOD( _GetType< MapPair_t >() ) >::SaveMap( this, value );
		}


		template <typename F, typename S>
		void _SavePair (const Pair<F,S> &pair) const
		{
			_Save( pair.first );
			_Save( pair.second );
		}
	

		template <typename K, typename S, typename A>
		void _Save (const Set<K,S,A> &value) const
		{
			return _SaveArray( value );
		}


		template <typename T>
		void _SaveArray (const T& value) const
		{
			_file->Write( _GetTypeOf( value ) );
			_file->Write( _GetType< typename T::value_t >() );

			_file->Write( (usize_t) value.Count() );
			
			_SavePodHelper< ESerializableValue::IsPOD( _GetType< typename T::value_t >() ) >::SaveArray( this, value );
		}

		
		template <typename T, usize I>
		void _Save (const Vec<T,I> &value) const
		{
			return _SaveVector( value );
		}

		
		void _Save (const color4u &value) const
		{
			return _SaveVector( value );
		}


		template <typename T, usize C, usize R>
		void _Save (const Matrix<T,C,R> &value) const
		{
			return _SaveVector( value );
		}


		template <typename T>
		void _SaveVector (const T& value) const
		{
			_file->Write( _GetTypeOf( value ) );
			_file->Write( _GetType< typename T::value_t >() );
			_file->Write( (vector_count_t) value.Count() );

			CHECK( ESerializableValue::IsPOD( _GetType< typename T::value_t >() ) );

			_file->Write( value );
		}

		
		void _Save (const bool &value)		const	{ return _SavePodValue( value ); }
		void _Save (const char &value)		const	{ return _SavePodValue( value ); }
		void _Save (const wchar &value)		const	{ return _SavePodValue( value ); }
		void _Save (const byte &value)		const	{ return _SavePodValue( value ); }
		void _Save (const short &value)		const	{ return _SavePodValue( value ); }
		void _Save (const int &value)		const	{ return _SavePodValue( value ); }
		void _Save (const ilong &value)		const	{ return _SavePodValue( value ); }
		void _Save (const ubyte &value)		const	{ return _SavePodValue( value ); }
		void _Save (const ushort &value)	const	{ return _SavePodValue( value ); }
		void _Save (const uint &value)		const	{ return _SavePodValue( value ); }
		void _Save (const ulong &value)		const	{ return _SavePodValue( value ); }
		void _Save (const float &value)		const	{ return _SavePodValue( value ); }
		void _Save (const double &value)	const	{ return _SavePodValue( value ); }
		

		template <typename T>
		void _SavePodValue (const T &value) const
		{
			CHECK( ESerializableValue::IsPOD( _GetTypeOf( value ) ) );
			
			_file->Write( _GetTypeOf( value ) );
			_file->Write( value );
		}

		
		template <typename T, typename S, typename A>
		void _Save (const TString<T,S,A> &value) const
		{
			return _SaveString( value );
		}

			
		template <typename T>
		void _SaveString (const T &value) const
		{
			_file->Write( _GetTypeOf( value ) );
			_file->Write( _GetType< typename T::value_t >() );
			
			CHECK( ESerializableValue::IsPOD( _GetType< typename T::value_t >() ) );

			_file->Write( (usize_t) value.Length() );
			_file->Write( value.ptr(), value.Size() );
		}
		

#		ifdef GX_PHYSICS_DIMENSIONS_ENABLED
		template <typename V, typename D, typename S>
		void _Save (const PhysicsValue<V,D,S> &value) const
		{
			return _Save( value.ref() );
		}
		

		template <typename V, typename D, typename S, usize I>
		void _Save (const PhysicsValueVec< PhysicsValue<V,D,S>, I > &value) const
		{
			return _SaveVector( value );
		}
#		endif	// GX_PHYSICS_DIMENSIONS_ENABLED
		

		template <typename T>
		void _Save (const Radians<T> &value) const
		{
			return _Save( value.ref() );
		}
		

		template <typename T>
		void _Save (const Degrees<T> &value) const
		{
			return _Save( value.ref() );
		}
		
		
		template <typename T>
		void _Save (const Percentage<T> &value) const
		{
			return _Save( value.ref() );
		}


		template <typename T>
		void _Save (T * const &value) const
		{
			_file->Write( _GetTypeOf( value ) );

			_Save( *value );
		}


		template <typename T>
		void _Save (Ptr<T> const &value) const
		{
			return _Save( value.ref() );
		}

		
		template <typename T, typename B, typename S>
		void _Save (CRefCounter<T,B,S> const &value) const
		{
			_file->Write( _GetTypeOf( value ) );
			
			const SerializableObjectID	id = value->GetID();
			_file->Write( id );

			_Save( *value );
		}


		template <typename T>
		void _Save (const T &value) const
		{
			_SavePodHelper< ESerializableValue::IsPOD( _GetType< T >() ) >::SaveStruct( this, value );
		}

		template <bool IsPOD>	// TODO
		struct _SavePodHelper;
	};
	

	template <>
	struct Serializer::_SavePodHelper<true>
	{
		template <typename T>
		static void SaveArray (const Serializer *ser, const T &value)
		{
			STATIC_ASSERT( CompileTime::IsPOD< typename T::value_t > );	// TODO

			if ( not value.Empty() )
			{
				ser->_file->Write( value.ptr(), value.Size() );
			}
		}

		template <typename T>
		static void SaveMap (const Serializer *ser, const T &value)
		{
			return SaveArray( ser, value );
		}

		template <typename T>
		static void SaveStruct (const Serializer *ser, const T &value)
		{
			return ser->_SavePodValue( value );
		}
	};
	

	template <>
	struct Serializer::_SavePodHelper<false>
	{
		template <typename T>
		static void SaveArray (const Serializer *ser, const T &value)
		{
			STATIC_ASSERT( not CompileTime::IsPOD< typename T::value_t > );	// TODO
			
			FOR( i, value ) {
				ser->_Save( value[i] );
			}
		}

		template <typename T>
		static void SaveMap (const Serializer *ser, const T &value)
		{
			FOR( i, value ) {
				ser->_SavePair( value[i] );
			}
		}

		template <typename T>
		static void SaveStruct (const Serializer *ser, const T &value)
		{
			value.Serialize( *ser );
		}
	};


}	// Base
}	// Engine