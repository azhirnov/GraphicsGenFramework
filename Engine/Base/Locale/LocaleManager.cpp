// Copyright © 2014-2016  Zhirnov Andrey. All rights reserved.

#include "LocaleManager.h"

namespace Engine
{
namespace Base
{
	
/*
=================================================
	constructor
=================================================
*/
	LocaleManager::LocaleManager (const SubSystemsRef ss) :
		IResourceLoader( ss )
	{
	}
	
/*
=================================================
	destructor
=================================================
*/
	LocaleManager::~LocaleManager ()
	{
	}
	
/*
=================================================
	Load
=================================================
*/
	bool LocaleManager::Load (OUT ResourcePtr &resource, PackFileID fileID, const RFilePtr &file)
	{
		TODO( "" );
		return false;
	}

	
}	// Base
}	// Engine
