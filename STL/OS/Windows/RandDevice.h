// Copyright © 2014-2016  Zhirnov Andrey. All rights reserved.

#pragma once

#include "OSWindows.h"
#include "../../Defines/Errors.h"

#ifdef PLATFORM_WINDOWS

namespace GX_STL
{
namespace _platform_
{
	using namespace winapi;


	//
	// Random Device
	//

	struct OSRandomDevice
	{
	// variables
	private:
		HCRYPTPROV	_prov;


	// methods
	public:
		OSRandomDevice (StringCRef provider) : _prov(0)
		{
			if ( not provider.Empty() ) {
				CHECK( _CreateFromProviderName( provider ) );
			}
			else {
				CHECK( _CreateDefault() );
			}
		}


		~OSRandomDevice ()
		{
			if ( _prov != 0 ) {
				CHECK( CryptReleaseContext( _prov, 0 ) == TRUE );
			}
		}


		template <typename T>
		bool Generate (T &value) const
		{
			return CryptGenRandom( _prov, sizeof(value), (BYTE *) &value ) == TRUE;
		}


	private:
		bool _CreateFromProviderName (StringCRef provider)
		{
			char	buf[ 128 ];
			DWORD	type;
			DWORD	len;

			// get type
			for (uint i = 0; ; ++i)
			{
				len = sizeof(buf);

				if ( CryptEnumProviders( i, null, 0, &type, buf, &len ) != TRUE )
					return false;

				if ( provider == buf )
					break;
			}

			return CryptAcquireContext( &_prov, null, provider.cstr(), type, CRYPT_VERIFYCONTEXT | CRYPT_SILENT ) == TRUE;
		}


		bool _CreateFromType (DWORD type)
		{
			return CryptAcquireContext( &_prov, null, (LPCSTR)null, type, CRYPT_VERIFYCONTEXT | CRYPT_SILENT ) == TRUE;
		}


		bool _CreateDefault ()
		{
			const DWORD	types[] = {
				PROV_RSA_FULL, PROV_RSA_AES, PROV_RSA_SIG, PROV_RSA_SCHANNEL, PROV_DSS,
				PROV_DSS_DH, PROV_DH_SCHANNEL, PROV_FORTEZZA, PROV_MS_EXCHANGE, PROV_SSL
			};

			for (usize i = 0; i < CountOf(types); ++i)
			{
				if ( _CreateFromType( types[i] ) )
					return true;
			}
			return false;
		}


		bool _CreateDefault2 ()
		{
			return _CreateFromProviderName( "Microsoft Base Cryptographic Provider v1.0" );
		}
	};
	
	
}	// _platform_
}	// GX_STL

#endif	// PLATFORM_WINDOWS