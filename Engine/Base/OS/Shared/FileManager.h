// Copyright � 2014-2017  Zhirnov Andrey. All rights reserved.

#pragma once

#include "Engine/Base/Common/BaseObject.h"

namespace GX_STL
{
namespace File
{
	typedef RCryptFile< SimpleFileCryptAlgorithm< 16 > >	SimpleRCryptFile;
	typedef WCryptFile< SimpleFileCryptAlgorithm< 16 > >	SimpleWCryptFile;

}	// File
}	// GX_STL

namespace Engine
{
namespace Base
{

	// define ResourceFile in platform specific files
	class ResourceFile;
	SHARED_POINTER( ResourceFile );



	//
	// File System
	//

	class FileManager : public BaseObject
	{
	// methods
	public:
		explicit
		FileManager (const SubSystemsRef ss);
		~FileManager ();

		bool OpenForRead (StringCRef filename, RFilePtr &file) const;
		bool OpenForWrite (StringCRef filename, WFilePtr &file) const;
		bool OpenForAppend (StringCRef filename, WFilePtr &file) const;
		bool CreateFile (StringCRef filename, WFilePtr &file) const;

		bool OpenForDecrypt (StringCRef filename, RFilePtr &file, StringCRef password) const;
		bool CreateEncryptFile (StringCRef filename, WFilePtr &file, StringCRef password) const;

		bool CreateMemFile (WFilePtr &file, BytesU reserve = BytesU()) const;
		bool SaveMemFile (StringCRef filename, const WFilePtr &file) const;
		bool ReadToMem (StringCRef filename, RFilePtr &file) const;
		bool ReadToMem (const RFilePtr &fromFile, RFilePtr &toFile) const;
		bool ReadToMemIfSmall (const RFilePtr &fromFile, RFilePtr &toFile) const;

		bool CopyFile (const RFilePtr &fromFile, const WFilePtr &toFile) const;
		bool CopyFile (StringCRef fromFilename, StringCRef toFilename) const;


		bool DeleteFile (StringCRef filename);

		bool FindAndSetCurrentDir (StringCRef dir, uint searchDepth = 4) const;
		bool SetCurrentDirectory (StringCRef dir) const;
		bool GetCurrentDirectory (OUT String &dir) const;

		bool OpenResource (StringCRef resname, RFilePtr &file) const;

		bool IsFileExist (StringCRef filename) const;
		bool IsResourceExist (StringCRef filename) const;
		bool IsDirectoryExist (StringCRef dirname) const;

		bool NewDirectory (StringCRef dir) const;			// create directory in current directory
		bool CreateDirectories (StringCRef path) const;		// create directories for path

		bool DeleteEmptyDirectory (StringCRef dir) const;
		bool DeleteDirectory (StringCRef dir) const;

		ulong GetFileSize (StringCRef filename) const;
		ulong GetFreeSpace (StringCRef path) const;
		ulong GetTotalSpace (StringCRef path) const;
		
		bool SearchFile (StringCRef file, uint depth, OUT String &result) const;
		bool SearchFileForward (StringCRef file, uint depth, OUT String &result) const;
		bool SearchFileBackward (StringCRef file, uint depth, OUT String &result) const;

		bool SearchDir (StringCRef dir, uint depth, OUT String &result) const;
		bool SearchDirForward (StringCRef dir, uint depth, OUT String &result) const;
		bool SearchDirBackward (StringCRef dir, uint depth, OUT String &result) const;


		bool MakeUniqueName (StringCRef filename, OUT String &result) const;
	};


}	// Base
}	// Engine
