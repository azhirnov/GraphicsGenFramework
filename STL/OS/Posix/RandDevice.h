// Copyright © 2014-2016  Zhirnov Andrey. All rights reserved.

#pragma once

#include "OSPosix.h"
#include "FileSystem.h"

#ifdef PLATFORM_BASE_POSIX

namespace GX_STL
{
namespace _platform_
{
	using namespace posix;
	


	//
	// Random Device
	//

	struct OSRandomDevice
	{
	// variables
	private:
		int		_fd;


	// methods
	public:
		OSRandomDevice (StringCRef provider) : _fd(-1)
		{
			if ( not provider.Empty() ) {
				CHECK_ERR( _Create( provider ), );
			}
			else {
				CHECK_ERR( _CreateDefault(), );
			}
		}


		~OSRandomDevice ()
		{
			CHECK( close( _fd ) >= 0 ) );
		}


		template <typename T>
		bool Generate (T &value) const
		{
			const usize	readn = read( _fd, (char *) &value, sizeof(value) );
			return readn == sizeof(value);
		}


	private:
		bool _Create (StringCRef provider)
		{
			_fd = open( provider.cstr(), O_RDONLY );
			return ( _fd >= 0 );
		}

		bool _CreateDefault ()
		{
			return _Create( "/dev/urandom" );
		}
	};

	
}	// _platform_
}	// GX_STL

#endif	// PLATFORM_BASE_POSIX
