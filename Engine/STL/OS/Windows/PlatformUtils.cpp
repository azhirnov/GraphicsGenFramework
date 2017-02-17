// Copyright © 2014-2017  Zhirnov Andrey. All rights reserved.

#include "PlatformUtils.h"
#include "FileSystem.h"

#ifdef PLATFORM_WINDOWS

#include "Engine/STL/OS/Base/BaseFileSystem.h"
#include "WinHeader.h"

#include "winnls.h"
#include "shobjidl.h"
#include "objbase.h"
#include "objidl.h"
#include "shlguid.h"

namespace GX_STL
{
namespace OS
{
	static bool _CreateLinkWin32 (const char *linkFilename, const char *link, bool isFolder);
	static bool _GetkWinApiError (const char *buf, int &bufSize);
	
/*
=================================================
	OpenURL
=================================================
*/
	void PlatformUtils::OpenURL (StringCRef url)
	{
		::ShellExecute( null, "open", url.cstr(), null, null, SW_SHOWNORMAL );
	}
	
/*
=================================================
	OpenFile
=================================================
*/
	void PlatformUtils::OpenFile (StringCRef filename)
	{
		::ShellExecute( null, null, filename.cstr(), null, null, SW_SHOWNORMAL );
	}

/*
=================================================
	CreateLink
=================================================
*/
	bool PlatformUtils::CreateLink (StringCRef linkFilename, StringCRef link)
	{
		return _CreateLinkWin32( linkFilename.cstr(), link.cstr(), not FileSystem::IsFileExist( link ) );
	}
	
/*
=================================================
	IDEConsoleMessage
=================================================
*/
	void PlatformUtils::IDEConsoleMessage (StringCRef message, StringCRef file, int line)
	{
		String	str;
		str << file << '(' << line << "): " << message << '\n';

		::OutputDebugStringA( str.cstr() );
	}

/*
=================================================
	ShowAssertion
=================================================
*/
	PlatformUtils::Dialog::EResult  PlatformUtils::Dialog::ShowAssertion (StringCRef caption, StringCRef msg, StringCRef file, int line)
	{
		if ( msg.Length() > 1024 )
			msg = msg.SubString( 0, 1024 );

		String	str;
		str << "File: " << file
			<< "\nLine: " << line
			<< "\n\nMessage:\n" << msg
			<< "\n\n(Press Retry to debug application)";

		int	result = ::MessageBoxA( 0, str.cstr(), caption.cstr(), 
									MB_ABORTRETRYIGNORE | MB_ICONERROR | MB_SETFOREGROUND | MB_TOPMOST );

		switch ( result )
		{
			case IDIGNORE :		return EResult::Ignore;
			case IDRETRY :		return EResult::Retry;
			case IDABORT :		return EResult::Abort;
			default :			return EResult::Unknown;
		};
	}

/*
=================================================
	_CheckError
=================================================
*/
	bool PlatformUtils::_CheckError (StringCRef file, int line)
	{
		char	buf[128] = {};
		int		size	 = (int)CountOf(buf);

		if ( _GetkWinApiError( buf, size ) )
			return true;

		String	str("WinAPI error: ");

		str << StringCRef( buf, size-2 );

		LOG( str.cstr(), ELog::Warning, file.cstr(), line );
		return false;
	}
	
/*
=================================================
	_CreateLinkWin32
=================================================
*/
	static bool _CreateLinkWin32 (const char *linkFilename, const char *link, bool isFolder)
	{
		HRESULT hres;
		IShellLinkA* psl;
 
		hres = CoInitialize( 0 );
		
		if ( FAILED(hres) )
			return false;

		hres = CoCreateInstance( isFolder ? CLSID_FolderShortcut : CLSID_ShellLink,
								 NULL,
								 CLSCTX_INPROC_SERVER,
								 IID_IShellLink,
								 (LPVOID*)&psl );

		if ( FAILED(hres) )
			return false;
 
		psl->SetPath( link );
		//psl->SetDescription( "" );
		
		IPersistFile* ppf;
 
		hres = psl->QueryInterface( IID_IPersistFile, (LPVOID*)&ppf );
 
		if ( SUCCEEDED(hres) )
		{
			WCHAR wsz[MAX_PATH]; 
 
			MultiByteToWideChar( CP_ACP, 0, linkFilename, -1, wsz, MAX_PATH ); 
            
			hres = ppf->Save( wsz, TRUE ); 
			ppf->Release(); 
		}

		psl->Release();
		CoUninitialize();

		if ( FAILED(hres) )
			return false;

		return true;
	}
	
/*
=================================================
	_GetkWinApiError
=================================================
*/
	static bool _GetkWinApiError (const char *buf, int &bufSize)
	{
		DWORD	dw = ::GetLastError();

		if ( dw == 0 )
			return true;

		DWORD	dw_count = ::FormatMessageA(
									FORMAT_MESSAGE_FROM_SYSTEM |
									FORMAT_MESSAGE_IGNORE_INSERTS,
									0,
									dw,
									MAKELANGID( LANG_ENGLISH, SUBLANG_DEFAULT ),
									(LPTSTR) buf,
									bufSize,
									0 );

		bufSize = dw_count;
		return false;
	}


}	// OS
}	// GX_STL

#endif	// PLATFORM_WINDOWS