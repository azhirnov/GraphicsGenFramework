// Copyright © 2014-2016  Zhirnov Andrey. All rights reserved.

#pragma once

#include "PackFileWritter.h"
#include "PackManager.h"

namespace Engine
{
namespace Base
{

	//
	// Pack File ID Manager
	//

	class PackFileIDManager : public BaseObject
	{
	// types
	private:
		typedef HashMap< String, PackFileID >	FileIDs_t;


	// variables
	private:
		PackManager		_packManager;

		FileIDs_t		_fileIds;
		uint			_currId;

		Mutex			_lock;


	// methods
	public:
		explicit
		PackFileIDManager (const SubSystemsRef ss);
		
		bool MapFile (StringCRef filename);

		bool Add (StringCRef filename, OUT PackFileID &id);

		bool IsExist (StringCRef filename);

		bool GetID (StringCRef filename, OUT PackFileID &id);

	private:
		void _FindNextFreeID ();
	};

	
/*
=================================================
	constructor
=================================================
*/
	inline PackFileIDManager::PackFileIDManager (const SubSystemsRef ss) :
		BaseObject( ss ), _packManager( ss ),
		_currId(0)
	{
		ASSERT( _currId != ReferenceCast< uint >( PackFileID() ) );
	}
	
/*
=================================================
	Add
=================================================
*/
	inline bool PackFileIDManager::Add (StringCRef filename, OUT PackFileID &id)
	{
		SCOPELOCK( _lock );

		FileIDs_t::const_iterator	iter;

		if ( _fileIds.Find( filename, iter ) )
		{
			id = iter->second;
			return true;
		}

		_FindNextFreeID();
		_fileIds.Add( filename, PackFileID( _currId ) );

		CHECK( _currId < MaxValue( _currId ) );

		return true;
	}
	
/*
=================================================
	IsExist
=================================================
*/
	inline bool PackFileIDManager::IsExist (StringCRef filename)
	{
		SCOPELOCK( _lock );
		return _fileIds.IsExist( filename );
	}
	
/*
=================================================
	GetID
=================================================
*/
	inline bool PackFileIDManager::GetID (StringCRef filename, OUT PackFileID &id)
	{
		SCOPELOCK( _lock );

		FileIDs_t::const_iterator	iter;

		if ( _fileIds.Find( filename, iter ) )
		{
			id = iter->second;
			return true;
		}

		return false;
	}
	
/*
=================================================
	MapFile
=================================================
*/
	inline bool PackFileIDManager::MapFile (StringCRef filename)
	{
		CHECK_ERR( _packManager.MapFile( filename ) );

		// validate IDs

		FOR( i, _fileIds )
		{
			if ( _packManager.IsExist( _fileIds[i].second ) )
			{
				RETURN_ERR( "IDs conflict!" );

				//_FindNextFreeID();
				//_fileIds[i].second = PackFileID( _currId );
			}
		}
		return true;
	}

/*
=================================================
	_FindNextFreeID
=================================================
*/
	inline void PackFileIDManager::_FindNextFreeID ()
	{
		// TODO: check for looping

		for (; not _packManager.IsExist( PackFileID( _currId ) ); ++_currId)
		{
		}
	}


}	// Base
}	// Engine
