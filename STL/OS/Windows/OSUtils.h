// Copyright © 2014-2016  Zhirnov Andrey. All rights reserved.

#pragma once

#include "OSWindows.h"

#ifdef PLATFORM_WINDOWS

namespace GX_STL
{
namespace _platform_
{
	using namespace winapi;


	//
	// OS Utils
	//

	struct OSUtils
	{
		static void OpenURL (StringCRef url);

		static void OpenFile (StringCRef filename);

		static bool CreateLink (StringCRef linkFilename, StringCRef link);

		static bool _CheckError (StringCRef file, int line);
	};
	

/*
=================================================
	OpenURL
=================================================
*/
	inline void OSUtils::OpenURL (StringCRef url)
	{
		ShellExecute( null, "open", url.cstr(), null, null, SW_SHOWNORMAL );
	}
	
/*
=================================================
	OpenFile
=================================================
*/
	inline void OSUtils::OpenFile (StringCRef filename)
	{
		ShellExecute( null, null, filename.cstr(), null, null, SW_SHOWNORMAL );
	}


#	define CHECK_OS_ERROR() \
		::GX_STL::_platform_::OSUtils::_CheckError( __FILE__, __LINE__ )


}	// _platform_
}	// GX_STL

#endif	// PLATFORM_WINDOWS