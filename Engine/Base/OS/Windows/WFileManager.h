// Copyright © 2014-2016  Zhirnov Andrey. All rights reserved.

#pragma once

#include "Engine/Base/OS/Shared/FileManager.h"

#if defined( PLATFORM_WINDOWS ) and not defined( PLATFORM_SDL )

namespace Engine
{
namespace WinPlatform
{

	//
	// Windows File System
	//

	class WindowsFileManager : public Base::FileManager
	{
	// methods
	public:
		explicit
		WindowsFileManager (const SubSystemsRef ss) :
			FileManager( ss )
		{}
	};

}	// WinPlatform
	

namespace Base
{

//------------------------ FileManager -------------------------//
/*
=================================================
	OpenResource
=================================================
*/
	inline bool FileManager::OpenResource (StringCRef resname, OUT RFilePtr &file) const
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
		return FileSystem::GetFileSize( filename );
	}


}	// Base
}	// Engine

#endif	// PLATFORM_WINDOWS