// Copyright © 2014-2017  Zhirnov Andrey. All rights reserved.

#pragma once

#include "Engine/Base/Common/BaseObject.h"
#include "Engine/Base/Pack/PackFileID.h"
#include "ResourceMemStat.h"

namespace Engine
{
namespace Base
{

	// must be namespace to define constants in other place
	namespace EResource
	{
		enum type
		{
			Unknown		= 0,
			Audio		= 0x10000000,
			Graphics	= 0x20000000,
			Locale		= 0x30000000,
		};
	};


	struct EResourceStatus
	{
		enum type
		{
			None = 0,			// resource not created
			Created,			// resource created, but not loaded
			Deferred,			// resource will loading in separate thread and data will be moved to this resource by calling SwapData
			AsyncLoading,		// temporary resource, after loading all data will be moved to resource with Deferred status
			Loaded,				// resource ready to use and can be modified (mutable resource)
			LoadedAndLocked,	// resource ready to use, but can't be modified (immutable resource)
			Failed,				// resource loading or creation failed
		};
	};
	

	class Resource;
	SHARED_POINTER( Resource );



	//
	// Resource
	//

	class Resource : public BaseObject
	{
	// variables
	private:
		ResourceMemStat				_memStat;
		PackFileID					_fileID;
		EResourceStatus::type		_status;
		const EResource::type		_resType;

		DEBUG_ONLY(
			String					_debugName;
		)


	// methods
	public:
		Resource (PackFileID fileID, EResource::type resType, const SubSystemsRef ss) :
			BaseObject( ss ),
			_fileID( fileID ),
			_resType( resType ),
			_status( EResourceStatus::None )
		{}

		~Resource ()
		{}


		void SetDebugName (StringCRef name)						{ DEBUG_ONLY( _debugName = name; ) }


		EResource::type			GetType ()				const	{ return _resType; }

		ResourceMemStat const&	GetMemStat ()			const	{ return _memStat; }

		PackFileID const&		GetFileID ()			const	{ return _fileID; }

		EResourceStatus::type	GetStatus ()			const	{ return _status; }


	protected:
		ResourceMemStat &		_EditMemStat ()					{ return _memStat; }

		bool _IsResourceCreatedOrLoaded () const
		{
			return	_status == EResourceStatus::Created or
					_IsResourceLoaded();
		}

		bool _IsResourceLoaded () const
		{
			return	_status == EResourceStatus::LoadedAndLocked or
					_status == EResourceStatus::Loaded;
		}

		bool _IsResourceLocked () const
		{
			return	_status == EResourceStatus::LoadedAndLocked or
					_status == EResourceStatus::Deferred;
		}

		void _SetResourceStatus (EResourceStatus::type state)
		{
			// TODO: check lock status ?
			_status = state;
		}

		void _Destroy ()
		{
			_memStat	= ResourceMemStat();
			_fileID		= PackFileID();
			_status		= EResourceStatus::None;

			// TODO: clear _debugName ?
		}
		
		bool _LockResource ()
		{
			CHECK_ERR( _IsResourceLoaded() );

			_status = EResourceStatus::LoadedAndLocked;
			return true;
		}

		bool _UnlockResource ()
		{
			CHECK_ERR( _IsResourceLocked() );

			_status = EResourceStatus::Loaded;
			return true;
		}
		

	// interface
	public:
		virtual bool SwapData (const ResourcePtr &other)
		{
			CHECK_ERR( other and GetType() == other->GetType() );

			// this method available only for resource with Deferred status
			CHECK_ERR( this->GetStatus() == EResourceStatus::Deferred and
					   other->GetStatus() == EResourceStatus::AsyncLoading );

			_memStat.Swap( other->_memStat );
			SwapMembers( this, other.ptr(), &Resource::_fileID );

			// TODO: set 'Loaded' status?
			return true;
		}

		virtual bool IsValid () const = 0;
	};


}	// Base
}	// Engine
