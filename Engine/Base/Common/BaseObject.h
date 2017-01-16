// Copyright © 2014-2016  Zhirnov Andrey. All rights reserved.
/*
	BaseObject - base class for Systems, Resources, Event Listeners and other.
	BaseObject can be static or dynamic, for dynamic creation use static function,
	which return shared pointer type, 'new' operator supported, but not safe.
*/

#pragma once

#include "Engine/Base/Common/Common.h"
#include "Engine/Base/Common/EngineSubSystems.h"

namespace Engine
{
namespace Base
{

	//
	// Base Object
	//

	class BaseObject : public CReferencedObject
	{
	// variables
	private:
		SubSystemsRef		_engineSubSystems;


	// methods
	public:
		explicit
		BaseObject (const SubSystemsRef ss) :
			_engineSubSystems(ss)
		{}

		SubSystemsRef		SubSystems ()			{ return _engineSubSystems; }
		SubSystemsRef const	SubSystems () const		{ return _engineSubSystems; }
	};


	SHARED_POINTER( BaseObject );


}	// Base
}	// Engine