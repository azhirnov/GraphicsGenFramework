// Copyright © 2014-2017  Zhirnov Andrey. All rights reserved.

#include "FileSystem.h"

#ifdef PLATFORM_WINDOWS

#include "Engine/STL/OS/Base/BaseFileSystem.h"
#include "WinHeader.h"

namespace GX_STL
{
namespace OS
{

/*
=================================================
	DeleteFile
=================================================
*/
	bool OSFileSystem::DeleteFile (StringCRef filename)
	{
		return ::DeleteFileA( filename.cstr() ) == TRUE;
	}
	
/*
=================================================
	SetCurrentDirectory
=================================================
*/
	bool OSFileSystem::SetCurrentDirectory (StringCRef dir)
	{
		if ( dir.Empty() )
			return false;

		return ::SetCurrentDirectoryA( dir.cstr() ) == TRUE;
	}
	
/*
=================================================
	GetCurrentDirectory
=================================================
*/
	bool OSFileSystem::GetCurrentDirectory (OUT String &dir)
	{
		dir.Reserve( MAX_PATH+1 );

		if ( ::GetCurrentDirectoryA( (DWORD) dir.Capacity(), dir.ptr() ) != 0 )
		{
			dir.CalculateLength();
			return true;
		}
		dir.Clear();
		return false;
	}
	
/*
=================================================
	IsFileExist
=================================================
*/
	bool OSFileSystem::IsFileExist (StringCRef filename)
	{
		if ( filename.Empty() )
			return false;

		int i_code = ::GetFileAttributes( filename.cstr() );
		return (i_code != -1) and not (FILE_ATTRIBUTE_DIRECTORY & i_code);
	}
	
/*
=================================================
	IsDirectoryExist
=================================================
*/
	bool OSFileSystem::IsDirectoryExist (StringCRef dirname)
	{
		if ( dirname.Empty() )
			return true;

		int i_code = ::GetFileAttributes( dirname.cstr() );
		return (i_code != -1) and (FILE_ATTRIBUTE_DIRECTORY & i_code);
	}
	
/*
=================================================
	NewDirectory
=================================================
*/
	bool OSFileSystem::NewDirectory (StringCRef dir)
	{
		if ( dir.Empty() )
			return true;

		ASSERT( not IsDirectoryExist( dir ) );

		return ::CreateDirectoryA( dir.cstr(), null ) == TRUE;
	}
	
/*
=================================================
	DeleteEmptyDirectory
=================================================
*/
	bool OSFileSystem::DeleteEmptyDirectory (StringCRef dir)
	{
		ASSERT( IsDirectoryExist( dir ) );
		return ::RemoveDirectoryA( dir.cstr() ) == TRUE;
	}
	
/*
=================================================
	DeleteDirectory
=================================================
*/
	bool OSFileSystem::DeleteDirectory (StringCRef dir)
	{
		ASSERT( IsDirectoryExist( dir ) );

		SHFILEOPSTRUCTA file_op = {
			null,
			FO_DELETE,
			dir.cstr(),
			"",
			FOF_NOCONFIRMATION |
			FOF_NOERRORUI |
			FOF_SILENT,
			false,
			0,
			"" };

		return SHFileOperation( &file_op ) == 0;
	}
		
/*
=================================================
	_SearchForward
=================================================
*
	template <typename B>
	bool FileSystem::_SearchForward (StringCRef dir, StringCRef name, uint depth, OUT String &result)
	{
		// search in current dir
		if ( depth == 0 )
			return false;
		
		String	s_dir( dir );

		if ( s_dir.Empty() or s_dir.Back() == '\\' or s_dir.Back() == '/' )
			s_dir << '*';
		else
			s_dir << "/*";

		WIN32_FIND_DATAA	s_wfd;
		HANDLE				h_search   = FindFirstFile( s_dir.cstr(), &s_wfd );

		if ( h_search == INVALID_HANDLE_VALUE )
			return false;

		String	tmp;

		do
		{
			if  ( s_wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY )
			{
				const StringCRef	fname( s_wfd.cFileName );

				if ( fname != "." and fname != ".." )
				{
					tmp = dir;
					FileAddress::AddDirectoryToPath( tmp, fname );
					FileAddress::AddNameToPath( tmp, name );

					if ( B::IsExist( tmp ) )
					{
						FindClose( h_search );
						result = RVREF( tmp );
						return true;
					}
				}
			}
		}
		while ( FindNextFile( h_search, &s_wfd ) );
		
		FindClose( h_search );


		// recursive search
		h_search = FindFirstFile( s_dir.cstr(), &s_wfd );
		
		if ( h_search == INVALID_HANDLE_VALUE )
			return false;

		do
		{
			if  ( s_wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY )
			{
				const StringCRef	dirname( s_wfd.cFileName );

				if ( dirname != "." and dirname != ".." )
				{
					tmp = dir;
					FileAddress::AddNameToPath( tmp, dirname );

					if ( _SearchForward<B>( tmp, name, depth-1, result ) )
					{
						FindClose( h_search );
						return true;
					}
				}
			}
		}
		while ( FindNextFile( h_search, &s_wfd ) );
		
		FindClose( h_search );
		return false;
	}
	
/*
=================================================
	GetAllFilesInPath
=================================================
*/
	bool OSFileSystem::GetAllFilesInPath (StringCRef path, Array<String> &fileNames)
	{
		ASSERT( IsDirectoryExist( path ) );

		fileNames.Clear();

		String	dir( path );
		
		if ( dir.Empty() or dir.Back() == '\\' or dir.Back() == '/' )	dir << '*';
		else															dir << "\\*";

		WIN32_FIND_DATAA	s_wfd;
		HANDLE				h_search = ::FindFirstFileA( dir.cstr(), &s_wfd );
		
		if ( h_search == INVALID_HANDLE_VALUE )
			return true;

		do
		{
			if ( not (s_wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) )
			{
				fileNames.PushBack( FileAddress::GetNameAndExt( StringCRef( s_wfd.cFileName ) ) );
			}
		}
		while ( ::FindNextFileA( h_search, &s_wfd ) );
		
		::FindClose( h_search );
		return true;
	}

/*
=================================================
	GetAllDirsInPath
=================================================
*/
	bool OSFileSystem::GetAllDirsInPath (StringCRef path, Array<String> &directories)
	{
		ASSERT( IsDirectoryExist( path ) );

		directories.Clear();

		String	dir( path );
		
		if ( dir.Empty() or dir.Back() == '\\' or dir.Back() == '/' )	dir << '*';
		else															dir << "\\*";

		WIN32_FIND_DATAA	s_wfd;
		HANDLE				h_search   = ::FindFirstFileA( dir.cstr(), &s_wfd );
		
		if ( h_search == INVALID_HANDLE_VALUE )
			return true;

		do
		{
			if  ( s_wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY )
			{
				if ( s_wfd.cFileName[0] == '.' and s_wfd.cFileName[1] == '\0' )
					continue;
				
				if ( s_wfd.cFileName[0] == '.' and s_wfd.cFileName[1] == '.' and s_wfd.cFileName[2] == '\0' )
					continue;
				
				directories.PushBack( FileAddress::GetNameAndExt( StringCRef( s_wfd.cFileName ) ) );
			}
		}
		while ( ::FindNextFileA( h_search, &s_wfd ) );
		
		::FindClose( h_search );
		return true;
	}
	
/*
=================================================
	CopyFile
=================================================
*/
	bool OSFileSystem::CopyFile (StringCRef fromFile, StringCRef toFile)
	{
		return ::CopyFileA( fromFile.cstr(), toFile.cstr(), FALSE ) == TRUE;
	}
	
/*
=================================================
	CopyDirectory
=================================================
*/
	bool OSFileSystem::CopyDirectory (StringCRef fromDir, StringCRef toDir)
	{
		SHFILEOPSTRUCTA s;
		UnsafeMem::ZeroMem( &s, SizeOf(s) );

		String	from_dir	= fromDir;
		String	to_dir		= toDir;

		// can't use << '\0' - it's have no effect
		from_dir << ' ';		from_dir.Back() = '\0';
		to_dir   << ' ';		from_dir.Back() = '\0';

		s.wFunc		= FO_COPY;
		s.fFlags	= FOF_NOCONFIRMATION | FOF_NOERRORUI | FOF_SILENT;
		s.pTo		= to_dir.cstr();
		s.pFrom		= from_dir.cstr();

		int res = SHFileOperation( &s );

		ASSERT( res == 0 );
		return res == 0;
	}
	
/*
=================================================
	GetFileLastModificationTime
=================================================
*/
	bool OSFileSystem::MoveFile (StringCRef oldName, StringCRef newName, bool async)
	{
		const uint	flags = MOVEFILE_COPY_ALLOWED | (async ? 0 : MOVEFILE_WRITE_THROUGH);

		return ::MoveFileEx( oldName.cstr(), newName.cstr(), flags ) == TRUE;
	}

/*
=================================================
	GetFileLastModificationTime
=================================================
*/
	ulong OSFileSystem::GetFileLastModificationTime (StringCRef filename)
	{
		FILETIME	time = {0};
		HANDLE		file = ::CreateFileA( filename.cstr(), GENERIC_READ,
							FILE_SHARE_DELETE | FILE_SHARE_READ | FILE_SHARE_WRITE,
							null, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, null );

		if ( file == null or ::GetFileTime( file, null, &time, null ) != TRUE )
			return 0;

		return (ulong(time.dwHighDateTime) << 32) | ulong(time.dwLowDateTime);
	}
	
/*
=================================================
	GetFileCreationTime
=================================================
*/
	ulong OSFileSystem::GetFileCreationTime (StringCRef filename)
	{
		FILETIME	time = {0};
		HANDLE		file = ::CreateFileA( filename.cstr(), GENERIC_READ,
							FILE_SHARE_DELETE | FILE_SHARE_READ | FILE_SHARE_WRITE,
							null, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, null );

		if ( file == null or ::GetFileTime( file, &time, null, null ) != TRUE )
			return 0;

		return (ulong(time.dwHighDateTime) << 32) | ulong(time.dwLowDateTime);
	}
	
/*
=================================================
	GetFileSize
=================================================
*/
	ulong OSFileSystem::GetFileSize (StringCRef filename)
	{
		HANDLE		file = ::CreateFileA( filename.cstr(), GENERIC_READ,
							FILE_SHARE_DELETE | FILE_SHARE_READ | FILE_SHARE_WRITE,
							null, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, null );

		if ( file == null )
			return 0;

		LARGE_INTEGER	size;
		::GetFileSizeEx( file, &size );

		return ulong(size.QuadPart);
	}


}	// OS
}	// GX_STL

#endif	// PLATFORM_WINDOWS
