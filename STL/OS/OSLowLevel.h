// Copyright © 2014-2016  Zhirnov Andrey. All rights reserved.

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
#	include "Windows/OSUtils.h"
#	include "Windows/RandDevice.h"
# endif

# ifdef PLATFORM_BASE_POSIX
#	include "Posix/SyncPrimitives.h"
#	include "Posix/Thread.h"
#	include "Posix/Library.h"
#	include "Posix/Timer.h"
#	include "Posix/FileSystem.h"
#	include "Posix/OSUtils.h"
#	include "Posix/RandDevice.h"
# endif

# ifdef PLATFORM_ANDROID
#	include "Android/OSAndroid.h"
# endif

#endif


namespace GX_STL
{
namespace GXTypes
{
	typedef _platform_::CurrentThread		CurrentThread;
	typedef _platform_::Thread				Thread;		
	typedef _platform_::SyncEvent			SyncEvent;
	typedef _platform_::CriticalSection		Mutex;
	typedef _platform_::ReadWriteSync		ReadWriteSync;
	typedef _platform_::ConditionVariable	ConditionVariable;
	typedef _platform_::Semaphore			Semaphore;
	typedef _platform_::Library				Library;
	typedef _platform_::PerfTimer			PerfomanceTimer;
	typedef _platform_::Date				Date;
	typedef _platform_::FileSystem			FileSystem;
	typedef _platform_::OSUtils				PlatformUtils;
	typedef _platform_::OSRandomDevice		PlatformRandomDevice;
	typedef _platform_::EThreadPriority		EThreadPriority;
	
}	// GXTypes
}	// GX_STL
