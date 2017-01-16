// Copyright © 2014-2016  Zhirnov Andrey. All rights reserved.
/*
	TODO:
	- default event that send after new listener attached
*/

#pragma once

#include "Engine/Base/Parallel/ThreadManager.h"

namespace Engine
{
namespace Base
{

	class EventSystem;
	SHARED_POINTER( EventSystem );

	
	//
	// Event System
	//

	class EventSystem : public VirtualThread
	{
	// types
	private:

		//
		// Event Listener interface
		//
		class IEventListener : public Referenced
		{
		public:
			virtual void Subscribe (const VariantRef &callback) = 0;
			virtual void Unsubscribe (const VariantRef &callback) = 0;
			virtual void Send (const VariantRef &data) = 0;
		};

		SHARED_POINTER( IEventListener );


		//
		// Event Listener Implementation
		//
		template <typename T>
		class EventListenerImpl : public IEventListener
		{
		private:
			typedef Delegate< void (const T&) >	del_t;
			typedef Event< del_t >				event_t;

		private:
			event_t		_event;

		public:
			EventListenerImpl ()
			{
				_event.Reserve( 128 );
			}
		
			void Subscribe (const VariantRef &callback) override
			{
				_event.Add( callback.Get< del_t >() );
			}

			void Unsubscribe (const VariantRef &callback) override
			{
				_event.Remove( callback.Get< del_t >() );
			}

			void Send (const VariantRef &data) override
			{
				_event.Call( data.Get<T>() );
			}
		};

		typedef Map< TypeId_t, IEventListenerPtr >	events_map_t;

		typedef Set< EventSystemPtr >				event_systems_t;


	// variables
	private:
		events_map_t		_events;

		event_systems_t		_systems;

		Mutex				_lock;


	// methods
	public:
		explicit
		EventSystem (const SubSystemsRef ss);


		template <typename T>
		void Send (const T &data);

		void Clear ();


		bool Attach (const EventSystemPtr &eventSystem);
		bool Detach (const EventSystemPtr &eventSystem);


		template <typename T>
		bool Subscribe (const Delegate< void (const T &) > &cb);

		template <typename T>
		bool Unsubscribe (const Delegate< void (const T &) > &cb);


		static EventSystemPtr	New (const SubSystemsRef ss)
		{
			return BaseObject::_New( new EventSystem( ss ) );
		}
	};
	
	
/*
=================================================
	constructor
=================================================
*/
	inline EventSystem::EventSystem (const SubSystemsRef ss) :
		VirtualThread( ss->Get< ThreadManager >()->GetCurrent() )
	{
		_events.Reserve( 32 );
	}
		
/*
=================================================
	Send
=================================================
*/
	template <typename T>
	inline void EventSystem::Send (const T &data)
	{
		if ( not IsCurrentThread() )
		{
			ParallelOp	op;
			FunctionBuilder::Create( op.func, EventSystemPtr(this), &EventSystem::Send<T>, data );

			Push( RVREF( op ) );
			return;
		}

		const TypeId_t	id	= TypeId<T>();

		events_map_t::iterator	iter;

		if ( _events.Find( id, OUT iter ) )
		{
			iter->second->Send( VariantRef::FromConstRef( data ) );
		}	

		SCOPELOCK( _lock );

		FOR( i, _systems )
		{
			_systems[i]->Send( data );
		}
	}
	
/*
=================================================
	Clear
=================================================
*/
	inline void EventSystem::Clear ()
	{
		ASSERT( IsCurrentThread() );

		_events.Clear();

		SCOPELOCK( _lock );
		_systems.Clear();
	}
	
/*
=================================================
	Attach
=================================================
*/
	inline bool EventSystem::Attach (const EventSystemPtr &eventSystem)
	{
		SCOPELOCK( _lock );

		CHECK_ERR( eventSystem != this );

		_systems.Add( eventSystem );
		return true;
	}
	
/*
=================================================
	Detach
=================================================
*/
	inline bool EventSystem::Detach (const EventSystemPtr &eventSystem)
	{
		SCOPELOCK( _lock );

		CHECK_ERR( _systems.Erase( eventSystem ) );
		return true;
	}

/*
=================================================
	Subscribe
=================================================
*/
	template <typename T>
	inline bool EventSystem::Subscribe (const Delegate< void (const T &) > &cb)
	{
		ASSERT( IsCurrentThread() );

		const TypeId_t	id	= TypeId<T>();
		usize			index;

		if ( not _events.FindIndex( id, OUT index ) )
		{
			index = _events.Add( id, new EventListenerImpl<T>() );
		}

		_events[ index ].second->Subscribe( VariantRef::FromConstRef( cb ) );
		return true;
	}
	
/*
=================================================
	Unsubscribe
=================================================
*/
	template <typename T>
	inline bool EventSystem::Unsubscribe (const Delegate< void (const T &) > &cb)
	{
		ASSERT( IsCurrentThread() );

		const TypeId_t	id	= TypeId<T>();

		events_map_t::iterator	iter;

		CHECK_ERR( _events.Find( id, OUT iter ) );

		iter->second->Unsubscribe( VariantRef::FromConstRef( cb ) );
		return true;
	}

	
}	// Base
}	// Engine