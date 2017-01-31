// Copyright © 2014-2017  Zhirnov Andrey. All rights reserved.

#pragma once

#include "Engine/Base/OS/Shared/FileManager.h"

#if defined( PLATFORM_SDL )

namespace Engine
{
namespace SdlPlatform
{

	//
	// SDL File System
	//

	class SDLFileManager : public Base::FileManager
	{
	// methods
	public:
		explicit
		SDLFileManager (const SubSystemsRef ss) :
			FileManager( ss )
		{}
	};

}	// SdlPlatform


namespace Base
{

//------------------------ FileManager -------------------------//
/*
=================================================
	OpenResource
=================================================
*/
	inline bool FileManager::OpenResource (StringCRef resname, RFilePtr &file) const
	{
		String res = "assets/";	res << resname;
		return OpenForRead( res, file );
	}
	
/*
=================================================
	IsResourceExist
=================================================
*/
	inline bool FileManager::IsResourceExist (StringCRef filename) const
	{
		String res = "assets/";	res << filename;
		return IsFileExist( res );
	}

/*
=================================================
	GetFreeSpace
----
	returns free space on disc (path)
=================================================
*/
	inline ulong FileManager::GetFreeSpace (StringCRef path) const
	{
		TODO( "" );
		return MaxValue<ulong>();
	}
	
/*
=================================================
	GetTotalSpace
----
	returns total space on disc (path)
=================================================
*/
	inline ulong FileManager::GetTotalSpace (StringCRef path) const
	{
		TODO( "" );
		return MaxValue<ulong>();
	}
	
/*
=================================================
	GetFileSize
=================================================
*/
	inline ulong FileManager::GetFileSize (StringCRef filename) const
	{
		return OS::FileSystem::GetFileSize( filename );
	}


}	// Base
}	// Engine

#endif	// PLATFORM_SDL
