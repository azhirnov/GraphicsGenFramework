// Copyright © 2014-2017  Zhirnov Andrey. All rights reserved.

#pragma once

#include "KeyBindings.h"

namespace Engine
{
namespace Base
{
namespace GameUtils
{
	
	//
	// Key Bindings with Events
	//

	template <typename ActionType>
	class KeyBindingsEvents : public BaseObject
	{
	// types
	public:
		typedef typename KeyBindings< ActionType >::Delegate_t	Delegate_t;
		typedef typename KeyBindings< ActionType >::KeyPair_t	KeyPair_t;
		
		typedef KeyBindingsEvents< ActionType >		Self;
		
		SHARED_POINTER( Self );

	private:
		typedef Map< ActionType, Event< Delegate_t > >		OnActionMap_t;


	// variables
	private:
		KeyBindings< ActionType >	_bindings;
		OnActionMap_t				_onAction;


	// methods
	public:
		explicit
		KeyBindingsEvents (const SubSystemsRef ss)
		{}

		static SelfPtr New (const SubSystemsRef ss)
		{
			return BaseObject::_New( new Self( ss ) );
		}

		void AddOnKeyDownAction (ActionType action, KeyPair_t first, KeyPair_t second)
		{
			_bindings.AddOnKeyDownAction( action, first, second );
		}

		void AddOnKeyUpAction (ActionType action, KeyPair_t first, KeyPair_t second)
		{
			_bindings.AddOnKeyUpAction( action, first, second );
		}

		void AddOnKeyPressedAction (ActionType action, KeyPair_t first, KeyPair_t second)
		{
			_bindings.AddOnKeyPressedAction( action, first, second );
		}

		void AttachListener (ActionType action, const Delegate_t &listener)
		{
			usize	idx;

			if ( not_onAction.FindIndex( action, idx ) )
				idx = _onAction.Add( action, Event<Delegate_t>() );

			_onAction[idx].Add( listener );
		}

		void DetachListener (ActionType action, const Delegate_t &listener)
		{
			OnActionMap_t::iterator	iter;

			if ( _onAction.Find( action, iter ) )
			{
				iter->second.Remove( listener );
			}
		}

		void DetachListener (const Delegate_t &listener)
		{
			FOR( i, _onAction ) {
				_onAction[i].second.Remove( listener );
			}
		}

		void CheckAll ()
		{
			FOR( i, _onAction )
			{
				OnActionMap_t::const_pair_t &	item = _onAction[i];

				if ( _bindings.Check( item.first ) )
				{
					item.second( item.first );
				}
			}
		}
	};


}	// GameUtils
}	// Base
}	// Engine
