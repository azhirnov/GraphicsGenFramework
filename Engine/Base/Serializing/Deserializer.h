// Copyright © 2014-2016  Zhirnov Andrey. All rights reserved.

#pragma once

#include "Serializer.h"
#include "SerializableObjectFactory.h"

namespace Engine
{
namespace Base
{

	//
	// Deserializer
	//

	class Deserializer
	{
	// types
	public:
		typedef Deserializer					Self;

		typedef Serializer::usize_t				usize_t;
		typedef Serializer::vector_count_t		vector_count_t;
		typedef Serializer::value_typeid_t		value_typeid_t;

		
		template <typename T, typename A>
		struct ArrayElementBinder
		{
		private:
			A &			arr;
			T &			elem;
			usize_t		index;
			bool		isCopy;

		public:
			ArrayElementBinder (A& arr, T& elem) :
				arr(arr), elem(elem), index(-1), isCopy(false)
			{}
				
			ArrayElementBinder (ArrayElementBinder &other) :
				arr(other.arr), elem(other.elem), index(other.index), isCopy(false)
			{
				other.isCopy = true;
			}

			~ArrayElementBinder ()
			{
				if ( isCopy ) return;
				
				if ( index < arr.Count() ) {
					elem = arr[ index ];
				} else {
					CHECK( false );
				}
			}

			usize_t &	Get ()	{ return index; }
		};

		struct Binder
		{
			Ptr< Self >		_s;

			Binder (Self *s) : _s(s)	{}

			template <typename T>
			Binder & operator , (T& value)			{ _s->Bind( value );  return *this; }
		};


	// variables
	private:
		SerializableObjectFactoryPtr	_objFactory;

		RFilePtr						_file;

		SerializableObjectPtr			_baseObj;

		mutable value_typeid_t			_currValueType;

		const value_typeid_t			_end;


	// methods
	public:
		Deserializer (const SerializableObjectFactoryPtr &objFactory, const RFilePtr &file, uint version = 1) :
			_objFactory( objFactory ),
			_file(file),
			_end( ESerializableValue::_FILE_END ),
			_currValueType(0)
		{
			uint	ver = 0;

			_file->Read( _currValueType );
			CHECK( _currValueType == ESerializableValue::_FILE_BEGIN );

			_file->Read( ver );
			CHECK( ver == version );
		}


		Deserializer (const Deserializer &other, const SerializableObjectPtr &baseObj, SerializableObjectID objID) :
			_objFactory( other._objFactory ),
			_file(other._file), 
			_end( ESerializableValue::_STRUCT_END ),
			_currValueType(0),
			_baseObj(baseObj)
		{
			SerializableObjectID	obj_id;

			_file->Read( _currValueType );
			CHECK( _currValueType == ESerializableValue::STRUCT );

			_file->Read( obj_id );
			CHECK( obj_id == objID );
		}


		~Deserializer ()
		{
			_file->Read( _currValueType );
			CHECK( _currValueType == _end );
		}
		

		template <typename T>
		Self & Parent (T& obj)
		{
			value_typeid_t	parent;

			_file->Read( parent );
			CHECK( parent == ESerializableValue::_STRUCT_PARENT );

			obj.Deserialize( *this );
			return *this;
		}


		template <typename T>
		Self & Bind (T& field)
		{
			_Load( field );
			return *this;
		}


		Binder	GetBinder ()		{ return Binder( this ); }


		SerializableObjectPtr const& GetBaseObject ()	const	{ return _baseObj; }


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


		template <typename T>
		void _CheckTypeOf (const T& t) const
		{
			_file->Read( _currValueType );
			CHECK( _currValueType == _GetTypeOf( t ) );
		}


		template <typename T>
		void _CheckType () const
		{
			_file->Read( _currValueType );
			CHECK( _currValueType == _GetType<T>() );
		}


		template <typename T, typename S, typename A>
		void _Load (Array<T,S,A> &value) const
		{
			_CheckTypeOf( value );
			_CheckType<T>();

			const usize_t	count = _file->ReadValue<usize_t>();

			value.Resize( count, false );
			
			_LoadPodHelper< ESerializableValue::IsPOD( _GetType< T >() ) >::LoadArray( this, value );
		}

		
		template <typename T, usize C, typename S>
		void _Load (StaticArray<T,C,S> &value) const
		{
			_CheckTypeOf( value );
			_CheckType<T>();
			
			const usize_t	count = _file->ReadValue<usize_t>();

			CHECK( count == value.Count() );
			
			_LoadPodHelper< ESerializableValue::IsPOD( _GetType< T >() ) >::LoadArray( this, value );
		}
		

		template <typename K, typename T, typename S, typename A>
		void _Load (Map<K,T,S,A> &value) const
		{
			_CheckTypeOf( value );
			_CheckType<K>();
			_CheckType<T>();
			
			const usize_t	count = _file->ReadValue<usize_t>();

			value.Resize( count );

			typedef typename Map<K,T,S,A>::pair_t	MapPair_t;
			typedef Buffer< MapPair_t >				PairBuffer_t;
			
			PairBuffer_t	arr = PairBuffer_t( const_cast< MapPair_t *>( value.ptr() ), value.Count() );
			
			_LoadPodHelper< ESerializableValue::IsPOD( _GetType< MapPair_t >() ) >::LoadMap( this, arr );
		}


		template <typename F, typename S>
		void _LoadPair (Pair<F,S> &pair) const
		{
			_Load( pair.first );
			_Load( pair.second );
		}
		

		template <typename K, typename S, typename A>
		void _Load (Set<K,S,A> &value) const
		{
			_CheckTypeOf( value );
			_CheckType<K>();

			const usize_t	count = _file->ReadValue<usize_t>();

			value.Resize( count );
			
			Buffer<K>	arr = Buffer<K>( const_cast< K *>( value.ptr() ), value.Count() );

			_LoadPodHelper< ESerializableValue::IsPOD( _GetType< K >() ) >::LoadArray( this, arr );
		}


		void _Load (bool &value)	const	{ return _LoadPodValue( value ); }
		void _Load (char &value)	const	{ return _LoadPodValue( value ); }
		void _Load (wchar &value)	const	{ return _LoadPodValue( value ); }
		void _Load (byte &value)	const	{ return _LoadPodValue( value ); }
		void _Load (short &value)	const	{ return _LoadPodValue( value ); }
		void _Load (int &value)		const	{ return _LoadPodValue( value ); }
		void _Load (ilong &value)	const	{ return _LoadPodValue( value ); }
		void _Load (ubyte &value)	const	{ return _LoadPodValue( value ); }
		void _Load (ushort &value)	const	{ return _LoadPodValue( value ); }
		void _Load (uint &value)	const	{ return _LoadPodValue( value ); }
		void _Load (ulong &value)	const	{ return _LoadPodValue( value ); }
		void _Load (float &value)	const	{ return _LoadPodValue( value ); }
		void _Load (double &value)	const	{ return _LoadPodValue( value ); }


		template <typename T>
		void _LoadPodValue (T &value) const
		{
			_CheckTypeOf( value );
			_file->Read( value );
		}


		template <typename T, typename S, typename A>
		void _Load (TString<T,S,A> &value) const
		{
			_CheckTypeOf( value );
			_CheckType<T>();
			
			const usize_t	length = _file->ReadValue<usize_t>();

			value.Reserve( length );

			CHECK( value.Capacity() >= length+1 );

			_file->Read( value.ptr(), (length+1) * sizeof(T) );

			value.CalculateLength();

			CHECK( value.Length() == length );
		}
		

		template <typename T, usize I>
		void _Load (Vec<T,I> &value) const
		{
			return _LoadVector( value );
		}
		

		void _Load (color4u &value) const
		{
			return _LoadVector( value );
		}


		template <typename T, usize C, usize R>
		void _Load (Matrix<T,C,R> &value) const
		{
			return _LoadVector( value );
		}


		template <typename T>
		void _LoadVector (T &value) const
		{
			_CheckTypeOf( value );
			_CheckType< typename T::value_t >();

			vector_count_t	count = 0;

			_file->Read( count );
			_file->Read( value );
			
			CHECK( ESerializableValue::IsPOD( _GetType< typename T::value_t >() ) );
		}
		

		template <typename V, typename D, typename S>
		void _Load (PhysicsValue<V,D,S> &value) const
		{
			return _Load( value.ref() );
		}
		

		template <typename V, typename D, typename S, usize I>
		void _Load (PhysicsValueVec< PhysicsValue<V,D,S>, I > &value) const
		{
			return _LoadVector( value );
		}
		

		template <typename T>
		void _Load (Radians<T> &value) const
		{
			return _Load( value.ref() );
		}
		

		template <typename T>
		void _Load (Degrees<T> &value) const
		{
			return _Load( value.ref() );
		}
		
		
		template <typename T>
		void _Load (Percentage<T> &value) const
		{
			return _Load( value.ref() );
		}


		template <typename T>
		void _Load (T * &value) const
		{
			RETURN_ERR( "not supported", void() );
			/*
			_CheckTypeOf( value );

			value = CreateDeserializedDynamicObject<T>( _dataPtr );
			CHECK( value != null );

			_Load( *value );*/
		}


		template <typename T>
		void _Load (Ptr<T> &value) const
		{
			return _Load( value.ref() );
		}

		
		template <typename T, typename B, typename S>
		void _Load (CRefCounter<T,B,S> &value) const
		{
			_CheckTypeOf( value );

			SerializableObjectID	id;
			_file->Read( id );

			value = _objFactory->Create( id, _baseObj ).To< CRefCounter<T,B,S> >();
			CHECK( value.IsNotNull() );

			_Load( *value );
		}


		template <typename T>
		void _Load (T &value) const
		{
			_LoadPodHelper< ESerializableValue::IsPOD( _GetType< T >() ) >::LoadStruct( this, value );
		}


		template <bool IsPOD>	// TODO
		struct _LoadPodHelper;
	};
	


	template <>
	struct Deserializer::_LoadPodHelper<true>
	{
		template <typename T>
		static void LoadArray (const Deserializer *des, T &value)
		{
			STATIC_ASSERT( CompileTime::IsPOD< typename T::value_t > );		// TODO

			if ( not value.Empty() )
			{
				des->_file->Read( value.ptr(), value.Size() );
			}
		}

		template <typename T>
		static void LoadMap (const Deserializer *des, T &value)
		{
			return LoadArray( des, value );
		}

		template <typename T>
		static void LoadStruct (const Deserializer *des, T &value)
		{
			CHECK( _GetTypeOf( value ) == ESerializableValue::POD_STRUCT );

			des->_LoadPodValue( value );
		}
	};
	

	template <>
	struct Deserializer::_LoadPodHelper<false>
	{
		template <typename T>
		static void LoadArray (const Deserializer *des, T &value)
		{
			STATIC_ASSERT( not CompileTime::IsPOD< typename T::value_t > );		// TODO
			
			FOR( i, value ) {
				des->_Load( value[i] );
			}
		}

		template <typename T>
		static void LoadMap (const Deserializer *des, T &value)
		{
			FOR( i, value ) {
				des->_LoadPair( value[i] );
			}
		}

		template <typename T>
		static void LoadStruct (const Deserializer *des, T &value)
		{
			CHECK( _GetTypeOf( value ) == ESerializableValue::STRUCT );

			value.Deserialize( *des );
		}
	};


}	// Base
}	// Engine