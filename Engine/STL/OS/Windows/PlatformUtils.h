// Copyright © 2014-2017  Zhirnov Andrey. All rights reserved.

#pragma once

#include "OSWindows.h"

#ifdef PLATFORM_WINDOWS

namespace GX_STL
{
namespace OS
{

	//
	// OS Utils
	//

	struct PlatformUtils
	{
		static void OpenURL (StringCRef url);

		static void OpenFile (StringCRef filename);

		static bool CreateLink (StringCRef linkFilename, StringCRef link);

		static bool _CheckError (StringCRef file, int line);

		static void IDEConsoleMessage (StringCRef message, StringCRef file, int line);

		struct Dialog
		{
			enum EResult
			{
				Unknown,
				Ignore,
				Retry,
				Abort,
			};

			static EResult ShowAssertion (StringCRef caption, StringCRef msg, StringCRef file, int line);
		};
	};


#	define CHECK_OS_ERROR() \
		::GX_STL::OS::PlatformUtils::_CheckError( __FILE__, __LINE__ )


}	// OS
}	// GX_STL

#endif	// PLATFORM_WINDOWS