// Copyright © 2014-2017  Zhirnov Andrey. All rights reserved.

#pragma once

#include "Resource.h"
#include "IResourceLoader.h"
#include "Engine/Base/Pack/PackManager.h"

namespace Engine
{
namespace Base
{

	//
	// Resource Manager
	//

	class ResourceManager : public BaseObject
	{
	// types
	private:

		//
		// Resource Wrapper
		//
		struct ResWrap
		{
		// variables
			ResourcePtr		ptr;
			PackFileID		id;

		// methods
			ResWrap () {}
			explicit ResWrap (const ResourcePtr &p) : ptr(p), id(p->GetFileID()) {}
			explicit ResWrap (PackFileID id) : ptr(null), id(id) {}

			bool operator == (const ResWrap &right) const	{ return id == right.id; }
			bool operator >  (const ResWrap &right) const	{ return id <  right.id; }
			bool operator <  (const ResWrap &right) const	{ return id >  right.id; }
		};


		//
		// Resource Map
		//
		struct ResourceMap
		{
		// variables
		public:
			mutable IResourceLoaderPtr	loader;
			EResource::type				type;


		// methods
		public:
			ResourceMap () : type( EResource::Unknown )
			{}
				
			explicit
			ResourceMap (EResource::type type) : type( type )
			{}

			explicit
			ResourceMap (const IResourceLoaderPtr &loader) : loader( loader ), type( loader->GetType() )
			{
				CHECK( loader );
			}

			bool Load (const RFilePtr &file, PackFileID fileID, ResourcePtr &res) const
			{
				CHECK_ERR( loader );
				CHECK_ERR( loader->Load( res, fileID, file ) );
				return true;
			}

			bool operator == (const ResourceMap &right) const	{ return type == right.type; }
			bool operator >  (const ResourceMap &right) const	{ return type >  right.type; }
			bool operator <  (const ResourceMap &right) const	{ return type <  right.type; }
		};

		typedef Set< ResWrap >		ResourceSet_t;
		typedef Set< ResourceMap >	ResourceLoaders_t;


	// variables
	private:
		ResourceLoaders_t		_resourceLoaders;
		
		ResourceSet_t			_resources;

		PackManager				_packManager;

		OS::Mutex				_lock;


	// methods
	public:
		explicit
		ResourceManager (const SubSystemsRef ss);

		~ResourceManager ();

		void Clear ();

		bool RegisterLoader (const IResourceLoaderPtr &loader);
		bool UnregisterLoader (const IResourceLoaderPtr &loader);

		bool AddResource (const ResourcePtr &res);
		bool RemoveResource (const ResourcePtr &res);


		template <typename T>
		bool Load (EResource::type resType, PackFileID fileID, OUT T &res);

		bool GetResource (PackFileID fileID, OUT ResourcePtr &res);

		void GetResources (EResource::type resType, OUT Array< ResourcePtr > &resources);

		BytesU GetUsedRamSize ();
		BytesU GetUsedVRamSize ();


	private:
		bool _LoadFromID (EResource::type resType, PackFileID fileID, OUT ResourcePtr &res);
	};


	
/*
=================================================
	Load
=================================================
*/
	template <typename T>
	inline bool ResourceManager::Load (EResource::type resType, PackFileID fileID, OUT T &res)
	{
		return _LoadFromID( resType, fileID, (ResourcePtr &) res );
	}


}	// Base
}	// Engine