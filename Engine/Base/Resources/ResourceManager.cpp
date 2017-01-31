// Copyright © 2014-2017  Zhirnov Andrey. All rights reserved.

#include "ResourceManager.h"

namespace Engine
{
namespace Base
{
	
/*
=================================================
	constructor
=================================================
*/
	ResourceManager::ResourceManager (const SubSystemsRef ss) :
		BaseObject( ss ), _packManager( ss )
	{
		SubSystems()->GetSetter< ResourceManager >().Set( this );

		_resourceLoaders.Reserve( 16 );
		_resources.Reserve( 1024 );
	}
		
/*
=================================================
	destructor
=================================================
*/
	ResourceManager::~ResourceManager ()
	{
		Clear();

		SubSystems()->GetSetter< ResourceManager >().Set( null );
	}
	
/*
=================================================
	Clear
=================================================
*/
	void ResourceManager::Clear ()
	{
		SCOPELOCK( _lock );

		_resourceLoaders.Clear();
		_resources.Clear();
	}
	
/*
=================================================
	RegisterLoader
=================================================
*/
	bool ResourceManager::RegisterLoader (const IResourceLoaderPtr &loader)
	{
		SCOPELOCK( _lock );

		CHECK_ERR( loader );
		CHECK_ERR( not _resourceLoaders.IsExist( ResourceMap( loader ) ) );

		_resourceLoaders.Add( ResourceMap( loader ) );
		return true;
	}
	
/*
=================================================
	UnregisterLoader
=================================================
*/
	bool ResourceManager::UnregisterLoader (const IResourceLoaderPtr &loader)
	{
		CHECK_ERR( loader );
		
		SCOPELOCK( _lock );

		usize	index = -1;

		if ( _resourceLoaders.FindIndex( ResourceMap( loader->GetType() ), index ) )
		{
			ResourceMap const & rm = _resourceLoaders[index];

			if ( rm.loader == loader )
			{
				rm.loader = null;
			}
		}

		return true;
	}
	
/*
=================================================
	AddResource
=================================================
*/
	bool ResourceManager::AddResource (const ResourcePtr &res)
	{
		SCOPELOCK( _lock );

		CHECK_ERR( res );
		CHECK_ERR( _resources.IsExist( ResWrap( res ) ) );

		_resources.Add( ResWrap( res ) );
		return true;
	}
	
/*
=================================================
	RemoveResource
=================================================
*/
	bool ResourceManager::RemoveResource (const ResourcePtr &res)
	{
		SCOPELOCK( _lock );

		CHECK_ERR( res );

		_resources.Erase( ResWrap( res ) );
		return true;
	}
	
/*
=================================================
	_LoadFromID
=================================================
*/
	bool ResourceManager::_LoadFromID (EResource::type resType, PackFileID fileID, OUT ResourcePtr &res)
	{
		SCOPELOCK( _lock );

		res = null;

		if ( GetResource( fileID, res ) )
			return true;

		usize	index1 = -1;

		if ( _resourceLoaders.FindIndex( ResourceMap( resType ), index1 ) )
		{
			RFilePtr	file;
			CHECK_ERR( _packManager.Open( fileID, file ) );
			CHECK_ERR( _resourceLoaders[ index1 ].Load( file, fileID, res ) );
			CHECK_ERR( res->GetFileID() == fileID );

			_resources.Add( ResWrap( res ) );
			return true;
		}
		return false;
	}

/*
=================================================
	GetResource
=================================================
*/
	bool ResourceManager::GetResource (PackFileID fileID, OUT ResourcePtr &res)
	{
		SCOPELOCK( _lock );

		res = null;

		usize	index = -1;

		if ( _resources.FindIndex( ResWrap( fileID ), index ) )
		{
			res = _resources[ index ].ptr;
			return true;
		}
		return false;
	}
	
/*
=================================================
	GetResources
=================================================
*/
	void ResourceManager::GetResources (EResource::type resType, OUT Array< ResourcePtr > &resources)
	{
		SCOPELOCK( _lock );

		resources.Clear();

		FOR( i, _resources )
		{
			if ( _resources[i].ptr->GetType() == resType )
			{
				resources.PushBack( _resources[i].ptr );
			}
		}
	}
	
/*
=================================================
	GetUsedRamSize
=================================================
*/
	BytesU ResourceManager::GetUsedRamSize ()
	{
		SCOPELOCK( _lock );

		BytesU	size;

		FOR( i, _resources )
		{
			size += _resources[i].ptr->GetMemStat().GetRamSize();
		}
		return size;
	}
	
/*
=================================================
	GetUsedVRamSize
=================================================
*/
	BytesU ResourceManager::GetUsedVRamSize ()
	{
		SCOPELOCK( _lock );

		BytesU	size;
		
		FOR( i, _resources )
		{
			size += _resources[i].ptr->GetMemStat().GetVRamSize();
		}
		return size;
	}

	
}	// Base
}	// Engine
