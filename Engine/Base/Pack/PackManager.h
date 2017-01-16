// Copyright © 2014-2016  Zhirnov Andrey. All rights reserved.

#pragma once

#include "Engine/Base/Common/BaseObject.h"
#include "Engine/Base/OS/Shared/FileManager.h"
#include "PackFileID.h"

namespace Engine
{
namespace Base
{

	//
	// Pack Files Manager
	//

	class PackManager : public BaseObject
	{
		friend class PackFileWritter;

	// types
	private:
		struct FileInPack : public CompileTime::PODStruct
		{
		// variables
			PackFileID	id;
			uint		pack;
			uint		offset;
			uint		size;

		// methods
			FileInPack () :
				id(), offset(0), size(0), pack(-1)
			{}

			explicit
			FileInPack (PackFileID id) :
				id(id), offset(0), size(0), pack(-1)
			{}

			FileInPack (uint pack, PackFileID id, uint offset, uint size) :
				pack(pack), id(id), offset(offset), size(size)
			{}

			bool operator == (const FileInPack &right) const	{ return id == right.id; }
			bool operator >  (const FileInPack &right) const	{ return id >  right.id; }
			bool operator <  (const FileInPack &right) const	{ return id <  right.id; }
		};
		

		struct FileHeader : public CompileTime::PODStruct
		{
		// variables
			PackFileID	id;
			uint		offset;
			uint		size;

		// methods
			FileHeader () : id(), offset(0), size(0) {}
			FileHeader (PackFileID id, uint offset, uint size) : id(id), offset(offset), size(size) {}
			
			bool operator == (const FileHeader &right) const	{ return id == right.id; }
			bool operator >  (const FileHeader &right) const	{ return id >  right.id; }
			bool operator <  (const FileHeader &right) const	{ return id <  right.id; }
		};


		struct PackFileHeader : public CompileTime::PODStruct
		{
			CompileTime::Signature4::value_t	signature;
			uint								numFiles;
			uint								packSize;

			PackFileHeader (uint numFiles, uint packSize) :
				signature( CompileTime::Signature4::Pack< 'P', 'A', 'C', 'K' >::value ),
				numFiles( numFiles ),
				packSize( packSize )
			{}
		};


		struct MappedFile : public CompileTime::FastCopyable
		{
		// variables
			RFilePtr	file;
			String		filename;
			uint		pack;

		// methods
			MappedFile () : pack(-1)
			{}

			MappedFile (uint packId, StringCRef fname, const RFilePtr &file) :
				filename(fname), file(file), pack(packId)
			{}

			bool operator == (const MappedFile &right) const	{ return filename == right.filename and pack == right.pack; }

			friend bool operator == (StringCRef left, const MappedFile &right)	{ return left == right.filename; }
		};

		typedef Set< FileInPack >		FileSet_t;
		typedef Array< MappedFile >		MappedFiles_t;


	// variables
	private:
		FileSet_t		_fileSet;
		MappedFiles_t	_mappedFiles;

		Mutex			_lock;


	// methods
	public:
		explicit
		PackManager (const SubSystemsRef ss);
		~PackManager ();

		bool MapFile (StringCRef filename);
		bool IsMapped (StringCRef filename);

		bool Open (PackFileID id, OUT RFilePtr &file);

		bool IsExist (PackFileID id);


	private:
		bool _ReadHeader (uint packId, StringCRef filename, OUT RFilePtr &file);
	};
	

	
/*
=================================================
	constructor
=================================================
*/
	inline PackManager::PackManager (const SubSystemsRef ss) :
		BaseObject( ss )
	{
		_mappedFiles.Reserve( 16 );
		_fileSet.Reserve( 1024 );
	}
	
/*
=================================================
	destructor
=================================================
*/
	inline PackManager::~PackManager ()
	{
	}
	
/*
=================================================
	MapFile
=================================================
*/
	inline bool PackManager::MapFile (StringCRef filename)
	{
		SCOPELOCK( _lock );

		CHECK_ERR( not _mappedFiles.IsExist( filename ) );

		RFilePtr	file;
		uint		pack_id = (uint) _mappedFiles.Count();

		CHECK_ERR( _ReadHeader( pack_id, filename, file ) );

		_mappedFiles.PushBack( MappedFile( pack_id, filename, file ) );
		return true;
	}
	
/*
=================================================
	IsMapped
=================================================
*/
	inline bool PackManager::IsMapped (StringCRef filename)
	{
		SCOPELOCK( _lock );
		return _mappedFiles.IsExist( filename );
	}

/*
=================================================
	Open
=================================================
*/
	inline bool PackManager::Open (PackFileID id, OUT RFilePtr &file)
	{
		SCOPELOCK( _lock );

		FileSet_t::const_iterator	file_result;

		CHECK_ERR( _fileSet.Find( FileInPack( id ), file_result ) );

		MappedFile const &	mapped_file = _mappedFiles[ file_result->pack ];

		file = File::SubRFile::New( mapped_file.file, Bytes<usize>( file_result->offset ), Bytes<usize>( file_result->size ) );
		CHECK_ERR( file.IsNotNull() );

		return true;
	}
	
/*
=================================================
	IsExist
=================================================
*/
	inline bool PackManager::IsExist (PackFileID id)
	{
		SCOPELOCK( _lock );
		return _fileSet.IsExist( FileInPack( id ) );
	}

/*
=================================================
	_ReadHeader
=================================================
*/
	inline bool PackManager::_ReadHeader (uint packId, StringCRef filename, OUT RFilePtr &file)
	{
		RFilePtr	rfile;
		CHECK_ERR( SubSystems()->Get< FileManager >()->OpenForRead( filename, rfile ) );

		file = File::MtRFile::New( rfile );

		uint	num_ids = 0;
		CHECK( file->Read( num_ids ) );

		Array< FileHeader >		ids;
		ids.Resize( num_ids, false );
		
		CHECK( file->Read( ids.ptr(), ids.Size() ) );

		_fileSet.Reserve( _fileSet.Count() + ids.Count() );

		FOR( i, ids ) {
			_fileSet.Add( FileInPack( packId, ids[i].id, ids[i].offset, ids[i].size ) );
		}
		return true;
	}


}	// Base
}	// Engine
