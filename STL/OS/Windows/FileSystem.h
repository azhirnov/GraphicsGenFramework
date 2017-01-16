// Copyright © 2014-2016  Zhirnov Andrey. All rights reserved.

#pragma once

#include "OSWindows.h"
#include "OSUtils.h"
#include "../Base/BaseFileSystem.h"
#include "../../Types/FileAddress.h"

#ifdef PLATFORM_WINDOWS

namespace GX_STL
{
namespace _platform_
{
	using namespace winapi;

	
	//
	// Windows File System
	//

	struct WinFileSystem
	{
	public:
		static bool DeleteFile (StringCRef filename);

		static bool SetCurrentDirectory (StringCRef dir);
		static bool GetCurrentDirectory (OUT String &dir);

		static bool IsFileExist (StringCRef filename);
		static bool IsDirectoryExist (StringCRef dirname);

		static bool NewDirectory (StringCRef dir);			// create directory in current directory
		//static bool CreateDirectories (StringCRef path);	// create directories for path

		static bool DeleteEmptyDirectory (StringCRef dir);
		static bool DeleteDirectory (StringCRef dir);
		
		static bool GetAllFilesInPath (StringCRef path, OUT Array<String> &fileNames);
		static bool GetAllDirsInPath (StringCRef path, OUT Array<String> &directories);
		
		template <typename Filter>
		static bool RecursiveFindFiles (StringCRef path, Filter filter, INOUT Array<String> &fileNames);
		
		static bool CopyFile (StringCRef fromFile, StringCRef toFile);
		static bool CopyDirectory (StringCRef fromDir, StringCRef toDir);

		// TODO: MoveFile
		// TODO: MoveDirectory

		static bool MoveFile (StringCRef oldName, StringCRef newName, bool async = false);

		static ulong GetFileLastModificationTime (StringCRef filename);
		static ulong GetFileCreationTime (StringCRef filename);
		static ulong GetFileSize (StringCRef filename);
	};

	
	typedef BaseFileSystem< WinFileSystem >		FileSystem;


/*
=================================================
	DeleteFile
=================================================
*/
	inline bool WinFileSystem::DeleteFile (StringCRef filename)
	{
		return winapi::DeleteFile( filename.cstr() ) == winapi::TRUE;
	}
	
/*
=================================================
	SetCurrentDirectory
=================================================
*/
	inline bool WinFileSystem::SetCurrentDirectory (StringCRef dir)
	{
		if ( dir.Empty() )
			return false;

		return winapi::SetCurrentDirectory( dir.cstr() ) == winapi::TRUE;
	}
	
/*
=================================================
	GetCurrentDirectory
=================================================
*/
	inline bool WinFileSystem::GetCurrentDirectory (OUT String &dir)
	{
		dir.Reserve( winapi::MAX_PATH+1 );

		if ( winapi::GetCurrentDirectory( (winapi::DWORD) dir.Capacity(), dir.ptr() ) != 0 )
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
	inline bool WinFileSystem::IsFileExist (StringCRef filename)
	{
		if ( filename.Empty() )
			return false;

		int i_code = winapi::GetFileAttributes( filename.cstr() );
		return (i_code != -1) and not (winapi::FILE_ATTRIBUTE_DIRECTORY & i_code);
	}
	
/*
=================================================
	IsDirectoryExist
=================================================
*/
	inline bool WinFileSystem::IsDirectoryExist (StringCRef dirname)
	{
		if ( dirname.Empty() )
			return false;

		int i_code = winapi::GetFileAttributes( dirname.cstr() );
		return (i_code != -1) and (winapi::FILE_ATTRIBUTE_DIRECTORY & i_code);
	}
	
/*
=================================================
	NewDirectory
=================================================
*/
	inline bool WinFileSystem::NewDirectory (StringCRef dir)
	{
		if ( dir.Empty() )
			return true;

		ASSERT( not IsDirectoryExist( dir ) );

		return winapi::CreateDirectory( dir.cstr(), null ) == winapi::TRUE;
	}
	
/*
=================================================
	DeleteEmptyDirectory
=================================================
*/
	inline bool WinFileSystem::DeleteEmptyDirectory (StringCRef dir)
	{
		ASSERT( IsDirectoryExist( dir ) );
		return winapi::RemoveDirectory( dir.cstr() ) == winapi::TRUE;
	}
	
/*
=================================================
	DeleteDirectory
=================================================
*/
	inline bool WinFileSystem::DeleteDirectory (StringCRef dir)
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
*/
	template <typename T>
	template <typename B>
	inline bool BaseFileSystem<T>::_SearchForward (StringCRef dir, StringCRef name, uint depth, OUT String &result)
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
					FileAddressUtils::AddDirectoryToPath( tmp, fname );
					FileAddressUtils::AddNameToPath( tmp, name );

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
					FileAddressUtils::AddNameToPath( tmp, dirname );

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
	inline bool WinFileSystem::GetAllFilesInPath (StringCRef path, Array<String> &fileNames)
	{
		ASSERT( IsDirectoryExist( path ) );

		String	dir( path );
		
		if ( dir.Back() == '\\' )	dir << '*';
		else						dir << "\\*";

		winapi::WIN32_FIND_DATAA	s_wfd;
		winapi::HANDLE				h_search = winapi::FindFirstFile( dir.cstr(), &s_wfd );
		
		if ( h_search == winapi::INVALID_HANDLE_VALUE )
			return true;

		do
		{
			if ( not (s_wfd.dwFileAttributes & winapi::FILE_ATTRIBUTE_DIRECTORY) )
			{
				fileNames.PushBack( FileAddressUtils::GetNameAndExt( StringCRef( s_wfd.cFileName ) ) );
			}
		}
		while ( winapi::FindNextFile( h_search, &s_wfd ) );
		
		winapi::FindClose( h_search );
		return true;
	}

/*
=================================================
	GetAllDirsInPath
=================================================
*/
	inline bool WinFileSystem::GetAllDirsInPath (StringCRef path, Array<String> &directories)
	{
		ASSERT( IsDirectoryExist( path ) );

		String	dir( path );
		
		if ( dir.Empty() or dir.Back() == '\\' or dir.Back() == '/' )	dir << '*';
		else															dir << "\\*";

		winapi::WIN32_FIND_DATAA	s_wfd;
		winapi::HANDLE				h_search   = winapi::FindFirstFile( dir.cstr(), &s_wfd );
		
		if ( h_search == winapi::INVALID_HANDLE_VALUE )
			return true;

		do
		{
			if  ( s_wfd.dwFileAttributes & winapi::FILE_ATTRIBUTE_DIRECTORY )
			{
				if ( s_wfd.cFileName[0] == '.' and s_wfd.cFileName[1] == '\0' )
					continue;
				
				if ( s_wfd.cFileName[0] == '.' and s_wfd.cFileName[1] == '.' and s_wfd.cFileName[2] == '\0' )
					continue;
				
				directories.PushBack( FileAddressUtils::GetNameAndExt( StringCRef( s_wfd.cFileName ) ) );
			}
		}
		while ( winapi::FindNextFile( h_search, &s_wfd ) );
		
		winapi::FindClose( h_search );
		return true;
	}
	
/*
=================================================
	RecursiveFindFiles
=================================================
*/
	template <typename Filter>
	inline bool WinFileSystem::RecursiveFindFiles (StringCRef dir, Filter filter, INOUT Array<String> &fileNames)
	{
		using namespace winapi;

		struct DirStackItem
		{
			HANDLE		search;
			String		path;
			///
			DirStackItem () : search(null) {}
			DirStackItem (HANDLE h, StringCRef dir) : search(h), path(dir) {}
		};

		typedef Array< DirStackItem >	dir_stack_t;

		struct Util
		{
			static void AddPath (String &path, StringCRef add)
			{
				if ( add.Empty() )
					return;

				if ( not path.Empty() and ( path.Back() != '/' or path.Back() != '\\' ) ) {
					path << '/';
				}
				path << add;
			}

			static bool CheckDirName (const char *dir)
			{
				return	not ( ( dir[0] == '.' and dir[1] == '\0' ) or					// "."
							  ( dir[0] == '.' and dir[1] == '.' and dir[2] == '\0' ) );	// ".."
			}

			static bool FindFirst (String &fullPath, const String &path, WIN32_FIND_DATAA &wfd, dir_stack_t &dirstack)
			{
				HANDLE	h_search = null;

				usize	len = fullPath.Length();

				AddPath( fullPath, path );

				if ( fullPath.Empty() )
					return false;

				if ( fullPath.Back() == '\\' or fullPath.Back() == '/' ) {
					fullPath << '*';
					len--;
				} else {
					fullPath << "\\*";
				}
				
				h_search = FindFirstFile( fullPath.cstr(), &wfd );
		
				fullPath.SetLength( len );

				if ( h_search != INVALID_HANDLE_VALUE )
				{
					dirstack.PushBack( DirStackItem( h_search, path ) );
					return true;
				}

				//CHECK_OS_ERROR();
				return false;
			}
		};


		CHECK_ERR( IsDirectoryExist( dir ) );

		WIN32_FIND_DATAA	s_wfd;
		dir_stack_t			search_stack;

		Ptr< HANDLE >		search_ref;

		Array<String>		files;

		String				full_path = dir;
		String				path;
		String				fname;

		bool				find_next = false;

		CHECK_ERR( Util::FindFirst( full_path, path, s_wfd, search_stack ) );


		while ( not search_stack.Empty() )
		{
			search_ref	= &search_stack.Back().search;
			path		=  search_stack.Back().path;

			while ( true )
			{
				if ( find_next and FindNextFile( *search_ref, &s_wfd ) == FALSE )
					break;

				if  ( s_wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY )
				{
					if ( Util::CheckDirName( s_wfd.cFileName ) )
					{
						Util::AddPath( path, s_wfd.cFileName );
						Util::FindFirst( full_path, path, s_wfd, search_stack );

						search_ref = &search_stack.Back().search;
						find_next  = false;
						continue;
					}
				}
				else
				{
					StringCRef	filename( s_wfd.cFileName );

					if ( filter( filename ) )
					{
						fname = path;
						Util::AddPath( fname, filename );

						fileNames.PushBack( fname );
					}
				}

				find_next = true;
			}

			FindClose( *search_ref );
			search_stack.PopBack();	// remove current search

			find_next = true;
		}

		return true;
	}
	
/*
=================================================
	CopyFile
=================================================
*/
	inline bool WinFileSystem::CopyFile (StringCRef fromFile, StringCRef toFile)
	{
		return winapi::CopyFile( fromFile.cstr(), toFile.cstr(), winapi::FALSE ) == winapi::TRUE;
	}
	
/*
=================================================
	CopyDirectory
=================================================
*/
	inline bool WinFileSystem::CopyDirectory (StringCRef fromDir, StringCRef toDir)
	{
		using namespace winapi;

		SHFILEOPSTRUCTA s;
		ZeroMem( &s, sizeof(s) );

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
	inline bool WinFileSystem::MoveFile (StringCRef oldName, StringCRef newName, bool async)
	{
		using namespace winapi;

		const uint	flags = MOVEFILE_COPY_ALLOWED | (async ? 0 : MOVEFILE_WRITE_THROUGH);

		return MoveFileEx( oldName.cstr(), newName.cstr(), flags ) == TRUE;
	}

/*
=================================================
	GetFileLastModificationTime
=================================================
*/
	inline ulong WinFileSystem::GetFileLastModificationTime (StringCRef filename)
	{
		using namespace winapi;

		FILETIME	time = {0};
		HANDLE		file = winapi::CreateFile( filename.cstr(), GENERIC_READ,
							FILE_SHARE_DELETE | FILE_SHARE_READ | FILE_SHARE_WRITE,
							null, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, null );

		if ( file == null or GetFileTime( file, null, &time, null ) != TRUE )
			return 0;

		return (ulong(time.dwHighDateTime) << 32) | ulong(time.dwLowDateTime);
	}
	
/*
=================================================
	GetFileCreationTime
=================================================
*/
	inline ulong WinFileSystem::GetFileCreationTime (StringCRef filename)
	{
		using namespace winapi;

		FILETIME	time = {0};
		HANDLE		file = winapi::CreateFile( filename.cstr(), GENERIC_READ,
							FILE_SHARE_DELETE | FILE_SHARE_READ | FILE_SHARE_WRITE,
							null, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, null );

		if ( file == null or GetFileTime( file, &time, null, null ) != TRUE )
			return 0;

		return (ulong(time.dwHighDateTime) << 32) | ulong(time.dwLowDateTime);
	}
	
/*
=================================================
	GetFileSize
=================================================
*/
	inline ulong WinFileSystem::GetFileSize (StringCRef filename)
	{
		using namespace winapi;

		HANDLE		file = winapi::CreateFile( filename.cstr(), GENERIC_READ,
							FILE_SHARE_DELETE | FILE_SHARE_READ | FILE_SHARE_WRITE,
							null, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, null );

		if ( file == null )
			return 0;

		LARGE_INTEGER	size;
		GetFileSizeEx( file, &size );

		return ulong(size.QuadPart);
	}

	
}	// _platform_
}	// GX_STL

#endif	// PLATFORM_WINDOWS