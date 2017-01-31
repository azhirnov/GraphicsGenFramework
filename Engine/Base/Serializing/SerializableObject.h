// Copyright © 2014-2017  Zhirnov Andrey. All rights reserved.

#pragma once

#include "SerializableObjectID.h"
#include "Engine/Base/Common/BaseObject.h"

namespace Engine
{
namespace Base
{
	
	class Serializer;
	class Deserializer;

	class SerializableObject;
	SHARED_POINTER( SerializableObject );



	//
	// Serializable Object
	//

	class SerializableObject : public BaseObject
	{
	// methods
	public:
		explicit
		SerializableObject (const SubSystemsRef ss) :
			BaseObject( ss )
		{}

		virtual SerializableObjectID	GetID () const = 0;

		virtual void Serialize (const Serializer &) const = 0;
		virtual void Deserialize (const Deserializer &) = 0;
	};


}	// Base
}	// Engine
