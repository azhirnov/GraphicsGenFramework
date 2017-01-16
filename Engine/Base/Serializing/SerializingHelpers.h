// Copyright © 2014-2016  Zhirnov Andrey. All rights reserved.

#pragma once

#include "Serializer.h"
#include "Deserializer.h"

namespace Engine
{
namespace Base
{/*
	namespace _BaseHidden_
	{
		template <typename T, typename A>
		static inline Serializer::ArrayElementBinder<T,A> GetArrayBinding (const A &arr, const T& elem)
		{
			return Serializer::ArrayElementBinder<T,A>( arr, elem );
		}

		template <typename T, typename A>
		static inline Deserializer::ArrayElementBinder<T,A> GetArrayBinding (A &arr, T& elem)
		{
			return Deserializer::ArrayElementBinder<T,A>( arr, elem );
		}
		
	}	// _BaseHidden_


#	define SI_BIND_ELEMENT( _arr_, _elem_ ) \
		_BaseHidden_::GetArrayBinding( _arr_, _elem_ ).Get()


#	define SI_BIND_ENUM( _enum_ ) \
		ReferenceCast< CompileTime::NearUInt::FromSize< sizeof(_enum_) >::type >( _enum_ )
	

#	define SI_BIND_POD( _pod_ ) \
		ReferenceCast< _BaseHidden_::PODStruct< sizeof(_pod_) > >( _pod_ )


#	define SI_BIND_CUSTOM( _fieldPtr_, _getSerializing_, _getDeserializing_ ) \
		fieldPtr	// TODO


#	define SI_SERIALIZED_OBJECT( _id_, _parent_, _fields_, _beforeSerialize_, _afterDeserialize_ ) \
		public: \
			void Serialize (const Serializer& __ser) const \
			{ \
				_beforeSerialize_; \
				Serializer ser( __ser, SerializableObjectID( _id_ ) ); \
				ser _parent_; \
				ser.GetBinder() _fields_; \
			} \
			\
			void Deserialize (const Deserializer &__des) \
			{ \
				Deserializer deser( __des, *this, SerializableObjectID( _id_ ) ); \
				deser _parent_; \
				deser.GetBinder() _fields_; \
				_afterDeserialize_; \
			}
	
	
#	define SI_SERIALIZED_OBJECT_H() \
		public: \
			void Serialize (const Serializer&) const; \
			void Deserialize (const Deserializer &);
			

#	define SI_SERIALIZED_OBJECT_VIRTUAL_H() \
		public: \
			virtual void Serialize (const Serializer&) const {}; \
			virtual void Deserialize (const Deserializer &) {};
			

#	define SI_SERIALIZED_OBJECT_OVERRIDE_H() \
		public: \
			void Serialize (const Serializer&) const override; \
			void Deserialize (const Deserializer &) override;


#	define SI_SERIALIZED_OBJECT_CPP( classType, __id, fields, beforeSerialize, afterDeserialize ) \
		\
		void classType::Serialize (const Serializer& __ser) const \
		{ \
			beforeSerialize; \
			Serializer ser( __ser, SerializableObjectID( __id ) ); \
			ser.GetBinder() fields; \
		} \
		\
		void classType::Deserialize (const Deserializer &__des) \
		{ \
			Deserializer deser( __des, this, SerializableObjectID( __id ) ); \
			deser.GetBinder() fields; \
			afterDeserialize; \
		}


#	define SI_SERIALIZED_OBJECT_PARENT_CPP( _classType_, _id_, _parent_, _fields_, _beforeSerialize_, _afterDeserialize_ ) \
		\
		void classType::Serialize (const Serializer& __ser) const \
		{ \
			parent::Serialize( __ser ); \
			beforeSerialize; \
			Serializer ser( __ser, SerializableObjectID( __id ) ); \
			ser.GetBinder() fields; \
		} \
		\
		void classType::Deserialize (const Deserializer &__des) \
		{ \
			parent::Deserialize( __des ); \
			Deserializer deser( __des, this, SerializableObjectID( _id_ ) ); \
			deser.GetBinder() fields; \
			afterDeserialize; \
		}*/


}	// Base
}	// Engine
