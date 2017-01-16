// Copyright © 2014-2016  Zhirnov Andrey. All rights reserved.

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

		CHECK_ERR( loader.IsNotNull() );
		CHECK_ERR( not _resourceLoaders.IsExist( ResourceMap( loader ) ) );

		_resourceLoaders.Add( ResourceMap( loader ) );
		return true;
	}
	
/*
=================================================
	UnregisterLoader
=================================================
*/
	void ResourceManager::UnregisterLoader (const IResourceLoaderPtr &loader)
	{
		CHECK_ERR( loader.IsNotNull(), void() );
		
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
	}
	
/*
=================================================
	AddResource
=================================================
*/
	void ResourceManager::AddResource (const ResourcePtr &res)
	{
		SCOPELOCK( _lock );

		CHECK_ERR( res.IsNotNull(), void() );
		CHECK_ERR( _resources.IsExist( ResWrap( res ) ), void() );

		_resources.Add( ResWrap( res ) );
	}
	
/*
=================================================
	RemoveResource
=================================================
*/
	void ResourceManager::RemoveResource (const ResourcePtr &res)
	{
		SCOPELOCK( _lock );

		CHECK_ERR( res.IsNotNull(), void() );

		_resources.Erase( ResWrap( res ) );
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
	Bytes<usize> ResourceManager::GetUsedRamSize ()
	{
		SCOPELOCK( _lock );

		Bytes<usize>	size;

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
	Bytes<usize> ResourceManager::GetUsedVRamSize ()
	{
		SCOPELOCK( _lock );

		Bytes<usize>	size;
		
		FOR( i, _resources )
		{
			size += _resources[i].ptr->GetMemStat().GetVRamSize();
		}
		return size;
	}

	
}	// Base
}	// Engine
