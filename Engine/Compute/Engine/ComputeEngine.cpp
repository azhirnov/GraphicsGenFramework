// Copyright © 2014-2017  Zhirnov Andrey. All rights reserved.

#include "ComputeEngine.h"

namespace Engine
{
namespace Compute
{
	
/*
=================================================
	constructor
=================================================
*/
	ComputeEngine::ComputeEngine (const SubSystemsRef ss) :
		BaseComputeEngine( ss )
	{
		SubSystems()->GetSetter< ComputeEngine >().Set( this );
	}

/*
=================================================
	destructor
=================================================
*/
	ComputeEngine::~ComputeEngine ()
	{
		SubSystems()->GetSetter< ComputeEngine >().Set( null );
	}


}	// Compute
}	// Engine