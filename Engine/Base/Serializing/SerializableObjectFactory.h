// Copyright © 2014-2017  Zhirnov Andrey. All rights reserved.

#pragma once

#include "SerializableObject.h"

namespace Engine
{
namespace Base
{

	class SerializableObjectFactory;
	SHARED_POINTER( SerializableObjectFactory );



	//
	// Serializable Object Factory
	//

	class SerializableObjectFactory : public BaseObject
	{
	// types
	public:
		typedef SerializableObjectPtr (CtorFunc_t) (const SubSystemsRef ss, const SerializableObjectPtr &base);
		
		typedef SerializableObjectFactory					Self;
		typedef Delegate< CtorFunc_t >						ObjCtor_t;
		typedef Map< SerializableObjectID, ObjCtor_t >		CtorsMap_t;


	// variables
	private:
		CtorsMap_t		_ctors;
	
		OS::Mutex		_lock;


	// methods
	public:
		explicit
		SerializableObjectFactory (const SubSystemsRef ss);
		~SerializableObjectFactory ();

		bool RegisterType (SerializableObjectID id, const ObjCtor_t &ctor);
		bool UnregisterType (SerializableObjectID id);
		bool IsRegistered (SerializableObjectID id);

		SerializableObjectPtr  Create (SerializableObjectID id, const SerializableObjectPtr &base = null);

		template <typename T>
		bool RegisterType ();

		template <typename T>
		bool UnregisterType ();

		template <typename T>
		SHARED_POINTER_TYPE(T)	Create (const SerializableObjectPtr &base = null);

	private:
		template <typename T>
		static SerializableObjectPtr _CtorWrapper (const SubSystemsRef ss, const SerializableObjectPtr &base);
	};

	
	
/*
=================================================
	RegisterType
=================================================
*/
	template <typename T>
	inline bool SerializableObjectFactory::RegisterType ()
	{
		return RegisterType( SerializableObjectID( T::ID ), &SerializableObjectFactory::_CtorWrapper<T> );
	}
	
/*
=================================================
	UnregisterType
=================================================
*/
	template <typename T>
	inline bool SerializableObjectFactory::UnregisterType ()
	{
		return UnregisterType( SerializableObjectID( T::ID ) );
	}

/*
=================================================
	Create
=================================================
*/
	template <typename T>
	inline SHARED_POINTER_TYPE(T)  SerializableObjectFactory::Create (const SerializableObjectPtr &base)
	{
		return Create( SerializableObjectID( T::ID ), base ).To< SHARED_POINTER_TYPE(T) >();
	}
	
/*
=================================================
	_CtorWrapper
=================================================
*/
	template <typename T>
	inline SerializableObjectPtr  SerializableObjectFactory::_CtorWrapper (const SubSystemsRef ss, const SerializableObjectPtr &base)
	{
		return ( T::New( ss, base ) ).To< SerializableObjectPtr >();
	}


}	// Base
}	// Engine