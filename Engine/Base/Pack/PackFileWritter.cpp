// Copyright © 2014-2017  Zhirnov Andrey. All rights reserved.

#include "PackFileWritter.h"

namespace Engine
{
namespace Base
{
	
/*
=================================================
	constructor
=================================================
*/
	PackFileWritter::PackFileWritter (const SubSystemsRef ss) :
		BaseObject( ss ), _totalSize(0)
	{
		_fileHeaders.Reserve( 1024 );
		_fileMap.Reserve( 1024 );
	}
	
/*
=================================================
	Create
=================================================
*/
	bool PackFileWritter::Create ()
	{
		Close();

		return true;
	}
	
/*
=================================================
	Save
=================================================
*/
	bool PackFileWritter::Save (StringCRef filename)
	{
		CHECK_ERR( not _fileMap.Empty() );
		CHECK_ERR( SubSystems()->Get< FileManager >()->IsFileExist( filename ) );


		// build files info
		ulong	offset = sizeof(PackFileHeader) + _fileMap.Count() * sizeof(FileHeader);

		FOR( i, _fileMap )
		{
			RFilePtr	file;
			CHECK_ERR( SubSystems()->Get< FileManager >()->OpenForRead( _fileMap[i].second, file ) );

			const usize	file_size = (usize)file->Size() + sizeof(PackFileID);

			CHECK_ERR( offset + file_size < _maxSize );

			_fileHeaders.PushBack( FileHeader( _fileMap[i].first, (uint)offset, (uint)file_size ) );

			offset += file_size;
		}


		// create pack
		WFilePtr	file;
		CHECK_ERR( SubSystems()->Get< FileManager >()->CreateFile( filename, file ) );

		const PackFileHeader	header( (uint)_fileHeaders.Count(), (uint)offset );

		CHECK_ERR( file->Write( header ) );
		CHECK_ERR( file->Write( (Buffer<FileHeader>) _fileHeaders ) );

		FOR( i, _fileMap )
		{
			PackFileID	id = _fileMap[i].first;

			CHECK( id == _fileHeaders[i].id );
			CHECK_ERR( file->Pos() == _fileHeaders[i].offset );

			file->Write( id );
			
			// copy
			RFilePtr	rfile;
			CHECK_ERR( SubSystems()->Get< FileManager >()->OpenForRead( _fileMap[i].second, rfile ) );

			CHECK_ERR( SubSystems()->Get< FileManager >()->CopyFile( rfile, file ) );
		}

		return true;
	}
	
/*
=================================================
	Close
=================================================
*/
	void PackFileWritter::Close ()
	{
		_fileHeaders.Clear();
		_fileMap.Clear();
		_totalSize = 0;
	}
	
/*
=================================================
	Add
=================================================
*/
	bool PackFileWritter::Add (PackFileID id, StringCRef filename)
	{
		FileMap_t::iterator	iter;

		if ( _fileMap.Find( id, iter ) )
		{
			CHECK_ERR( iter->second == filename );
			return true;
		}

		CHECK_ERR( SubSystems()->Get< FileManager >()->IsFileExist( filename ) );

		_totalSize += SubSystems()->Get< FileManager >()->GetFileSize( filename );
		CHECK_ERR( _totalSize < _maxSize );

		_fileMap.Add( id, filename );
		return true;
	}
	
/*
=================================================
	UpdatePackSize
=================================================
*/
	bool PackFileWritter::UpdatePackSize ()
	{
		Ptr< FileManager >	fm = SubSystems()->Get< FileManager >();

		_totalSize = 0;

		FOR( i, _fileMap ) {
			_totalSize += fm->GetFileSize( _fileMap[i].second );
		}

		return true;
	}


}	// Base
}	// Engine
