// Copyright � 2014-2017  Zhirnov Andrey. All rights reserved.

#pragma once

#include "Base/Common.h"

#ifdef PLATFORM_SDL
#	include "SDL/SyncPrimitives.h"
#	include "SDL/Thread.h"
#	include "SDL/Library.h"
#	include "SDL/Timer.h"
#	include "SDL/FileSystem.h"
#	include "SDL/SDLUtils.h"
#	include "SDL/RandDevice.h"
#else

# ifdef PLATFORM_WINDOWS
#	include "Windows/SyncPrimitives.h"
#	include "Windows/Thread.h"
#	include "Windows/Library.h"
#	include "Windows/Timer.h"
#	include "Windows/FileSystem.h"
#	include "Windows/PlatformUtils.h"
#	include "Windows/RandDevice.h"
# endif

# ifdef PLATFORM_BASE_POSIX
#	include "Posix/SyncPrimitives.h"
#	include "Posix/Thread.h"
#	include "Posix/Library.h"
#	include "Posix/Timer.h"
#	include "Posix/FileSystem.h"
#	include "Posix/PlatformUtils.h"
#	include "Posix/RandDevice.h"
# endif

# ifdef PLATFORM_ANDROID
#	include "Android/OSAndroid.h"
# endif

#endif

#include "Base/BaseFileSystem.h"


namespace GX_STL
{
namespace OS
{
	typedef CriticalSection		Mutex;
	
}	// OS
}	// GX_STL
