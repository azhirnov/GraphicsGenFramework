// Copyright © 2014-2016  Zhirnov Andrey. All rights reserved.

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
	

	class Resource;
	SHARED_POINTER( Resource );



	//
	// Resource
	//

	class Resource : public BaseObject
	{
	// types
	public:
		struct ELoadState
		{
			enum type
			{
				None = 0,
				Created,
				AsyncLoading,		// before SwapData called
				Loaded,
				LoadedAndLocked,
				Failed,
			};
		};


	// variables
	private:
		ResourceMemStat			_memStat;
		PackFileID				_fileID;
		ELoadState::type		_loadState;
		const EResource::type	_resType;

		DEBUG_ONLY(
			String				_name;
		)


	// methods
	public:
		Resource (PackFileID fileID, EResource::type resType, const SubSystemsRef ss) :
			BaseObject( ss ),
			_fileID( fileID ),
			_resType( resType ),
			_loadState( ELoadState::None )
		{}

		~Resource ()
		{}


		void SetDebugName (StringCRef name)						{ DEBUG_ONLY( _name = name; ) }


		EResource::type			GetType ()				const	{ return _resType; }

		ResourceMemStat const&	GetMemStat ()			const	{ return _memStat; }

		PackFileID const&		GetFileID ()			const	{ return _fileID; }

		ELoadState::type		GetLoadState ()			const	{ return _loadState; }

		bool					IsResourceCreated ()	const	{ return _loadState == ELoadState::Created; }
		bool					IsResourceLoaded ()		const	{ return _loadState == ELoadState::Loaded or IsResourceLocked(); }
		bool					IsResourceLocked ()		const	{ return _loadState == ELoadState::LoadedAndLocked; }


	protected:
		ResourceMemStat &		_EditMemStat ()					{ return _memStat; }

		void _SetLoadState (ELoadState::type state)
		{
			_loadState = state;
		}

		void _Destroy ()
		{
			_memStat	= ResourceMemStat();
			_fileID		= PackFileID();
			_loadState	= ELoadState::None;
		}
		/*
		bool _Lock ()
		{
			CHECK_ERR( IsResourceLoaded() );
			_loadState = ELoadState::LoadedAndLocked;
			return true;
		}

		bool _Unlock ()
		{
			CHECK_ERR( IsResourceLocked() );
			_loadState = ELoadState::Loaded;
			return true;
		}
		*/

	// interface
	public:
		virtual bool SwapData (const ResourcePtr &other)
		{
			CHECK_ERR( other.IsNotNull() and GetType() == other->GetType() );

			_memStat.Swap( other->_memStat );
			SwapMembers( this, other.ptr(), &Resource::_fileID );
			return true;
		}

		virtual bool IsValid () const = 0;
	};


}	// Base
}	// Engine
