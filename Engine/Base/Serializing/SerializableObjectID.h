// Copyright © 2014-2016  Zhirnov Andrey. All rights reserved.

#pragma once

#include "Engine/Base/Common/Common.h"

namespace Engine
{
namespace Base
{

	//
	// Serializable Object ID
	//

	struct SerializableObjectID : public CompileTime::PODStruct
	{
	// variables
	private:
		uint	_id;

	// methods
	public:
		SerializableObjectID (GX_DEFCTOR) : _id(-1)
		{}

		explicit
		SerializableObjectID (uint id) : _id(id)
		{}

		bool operator == (const SerializableObjectID &right) const	{ return _id == right._id; }
		bool operator != (const SerializableObjectID &right) const	{ return _id != right._id; }
		bool operator >  (const SerializableObjectID &right) const	{ return _id >  right._id; }
		bool operator <  (const SerializableObjectID &right) const	{ return _id <  right._id; }
		bool operator >= (const SerializableObjectID &right) const	{ return _id >= right._id; }
		bool operator <= (const SerializableObjectID &right) const	{ return _id <= right._id; }
	};


}	// Base
}	// Engine
