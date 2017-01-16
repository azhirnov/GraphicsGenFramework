// Copyright © 2014-2016  Zhirnov Andrey. All rights reserved.

#include "OSUtils.h"
#include "FileSystem.h"

#ifdef PLATFORM_WINDOWS

namespace GX_STL
{
namespace _platform_
{
	extern bool _CreateLinkWin32 (const char *linkFilename, const char *link, bool isFolder);
	extern bool _GetkWinApiError (const char *buf, int &bufSize);

/*
=================================================
	CreateLink
=================================================
*/
	bool OSUtils::CreateLink (StringCRef linkFilename, StringCRef link)
	{
		return _CreateLinkWin32( linkFilename.cstr(), link.cstr(), not FileSystem::IsFileExist( link ) );
	}
	
/*
=================================================
	_CheckError
=================================================
*/
	bool OSUtils::_CheckError (StringCRef file, int line)
	{
		char	buf[128] = {};
		int		size	 = (int)CountOf(buf);

		if ( _GetkWinApiError( buf, size ) )
			return true;

		String	str("WinAPI error: ");

		str << StringCRef( buf, size-2 );

		LOG_EXT( str.cstr(), ELog::Warning, file.cstr(), line );
		return false;
	}


}	// _platform_
}	// GX_STL

#endif	// PLATFORM_WINDOWS