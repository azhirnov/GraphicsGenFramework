// Copyright © 2014-2016  Zhirnov Andrey. All rights reserved.

#include "SerializableObjectFactory.h"
#include "SerializingHelpers.h"

namespace Engine
{
namespace Base
{
	
/*
=================================================
	constructor
=================================================
*/
	SerializableObjectFactory::SerializableObjectFactory (const SubSystemsRef ss) :
		BaseObject( ss )
	{
		SubSystems()->GetSetter< SerializableObjectFactory >().Set( this );
	}
	
/*
=================================================
	destructor
=================================================
*/
	SerializableObjectFactory::~SerializableObjectFactory ()
	{
		SubSystems()->GetSetter< SerializableObjectFactory >().Set( null );
	}

/*
=================================================
	RegisterType
=================================================
*/
	bool SerializableObjectFactory::RegisterType (SerializableObjectID id, const ObjCtor_t &ctor)
	{
		SCOPELOCK( _lock );

		CHECK_ERR( ctor.IsValid() );
		CHECK_ERR( _ctors.IsExist( id ) );

		_ctors.Add( id, ctor );
		return true;
	}
	
/*
=================================================
	UnregisterType
=================================================
*/
	bool SerializableObjectFactory::UnregisterType (SerializableObjectID id)
	{
		SCOPELOCK( _lock );
		return _ctors.Erase( id );
	}
	
/*
=================================================
	IsRegistered
=================================================
*/
	bool SerializableObjectFactory::IsRegistered (SerializableObjectID id)
	{
		SCOPELOCK( _lock );
		return _ctors.IsExist( id );
	}
	
/*
=================================================
	Create
=================================================
*/
	SerializableObjectPtr  SerializableObjectFactory::Create (SerializableObjectID id, const SerializableObjectPtr &base)
	{
		SCOPELOCK( _lock );

		CtorsMap_t::const_iterator	result;

		CHECK_ERR( _ctors.Find( id, result ), null );

		return result->second.Call( SubSystems(), base );
	}
	

}	// Base
}	// Engine