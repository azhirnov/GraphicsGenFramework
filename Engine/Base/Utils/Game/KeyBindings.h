// Copyright © 2014-2016  Zhirnov Andrey. All rights reserved.

#pragma once

#include "Engine/Base/OS/OS.h"

namespace Engine
{
namespace Base
{
namespace GameUtils
{

	//
	// Key Bindings
	//

	template <typename ActionType>
	class KeyBindings : public BaseObject
	{
	// types
	public:
		typedef Delegate< void (ActionType) >	Delegate_t;
		typedef Pair< EKey::type, EKey::type >	KeyPair_t;	// one key or combination of two keys
		
		typedef KeyBindings< ActionType >		Self;

		SHARED_POINTER( Self );


	protected:

		struct KeyBind
		{
		// types
			enum EState
			{
				ON_KEY_DOWN,
				ON_KEY_UP,
				ON_PRESSED,
			};

		// variables
			KeyPair_t	main;	// main key
			KeyPair_t	alt;	// alternative key
			EState		state;

		// methods
			KeyBind (KeyPair_t main, KeyPair_t alt, EState state) :
				main(main), alt(alt), state(state)
			{}
		};

		typedef Map< ActionType, KeyBind >		KeyMap_t;
		typedef Event< Delegate_t >				AnyActionEvent_t;
		typedef StaticBitArray< EKey::_COUNT >	KeyStates_t;
		typedef StaticArray< KeyStates_t, 2 >	KeyStates2_t;

		typedef StaticLimit< ubyte, 0, 1, StaticLimitStrategy::Wrap >	Index01_t;


	// variables
	protected:
		KeyMap_t			_keys;
		AnyActionEvent_t	_event;
		KeyStates2_t		_keyStates;
		Index01_t			_currKeyState;


	// methods
	public:
		explicit
		KeyBindings (const SubSystemsRef ss) :
			BaseObject( ss ), _currKeyState( 0 )
		{
			EventSystemPtr	es = SubSystems()->Get< Application >()->GetEventSystem();

			es->Subscribe( DelegateBuilder::Create( SelfPtr(this), &KeyBindings::_OnKey ) );
		}

		~KeyBindings ()
		{
			EventSystemPtr	es = SubSystems()->Get< Application >()->GetEventSystem();

			if ( es.IsNotNull() )
			{
				es->Unsubscribe( DelegateBuilder::Create( SelfPtr(this), &KeyBindings::_OnKey ) );
			}
		}
		
		static SelfPtr New (const SubSystemsRef ss)
		{
			return BaseObject::_New( new Self( ss ) );
		}

		void AttachListener (const Delegate_t &listener)
		{
			_event.Add( listener );
		}

		void DetachListener (const Delegate_t &listener)
		{
			_event.Remove( listener );
		}

		void AddOnKeyDownAction (ActionType action, KeyPair_t first, KeyPair_t second)
		{
			_Validate( first );
			_Validate( second );
			_keys.Add( action, KeyBind( first, second, KeyBind::ON_KEY_DOWN ) );
		}

		void AddOnKeyUpAction (ActionType action, KeyPair_t first, KeyPair_t second)
		{
			_Validate( first );
			_Validate( second );
			_keys.Add( action, KeyBind( first, second, KeyBind::ON_KEY_UP ) );
		}

		void AddOnKeyPressedAction (ActionType action, KeyPair_t first, KeyPair_t second)
		{
			_Validate( first );
			_Validate( second );
			_keys.Add( action, KeyBind( first, second, KeyBind::ON_PRESSED ) );
		}

		bool Check (ActionType action)
		{
			KeyMap_t::iterator	iter;

			if ( not _keys.Find( action, iter ) )
			{
				RETURN_ERR( (String("Unknown action type: ") << uint(action)).cstr() );
			}

			bool checked = false;

			switch ( iter->second.state )
			{
				case KeyBind::ON_KEY_DOWN :	checked = _IsKeyDown( iter->main ) or _IsKeyDown( iter->alt );	break;

				case KeyBind::ON_KEY_UP :	checked = _IsKeyUp( iter->main ) or _IsKeyUp( iter->alt );	break;

				case KeyBind::ON_PRESSED :	checked = _IsPressed( iter->main ) or _IsPressed( iter->alt );	break;
			}

			if ( checked )
				_event( iter->first );

			return checked;
		}


	protected:
		// TODO: swap states array on start next frame

		KeyStates_t &		_CurrKeyStates ()			{ return _keyStates[ _currKeyState ]; }
		KeyStates_t &		_PrevKeyStates ()			{ return _keyStates[ _currKeyState+1 ]; }

		KeyStates_t const&	_CurrKeyStates ()	const	{ return _keyStates[ _currKeyState ]; }
		KeyStates_t const&	_PrevKeyStates ()	const	{ return _keyStates[ _currKeyState+1 ]; }


		static void _Validate (INOUT KeyPair_t &p)
		{
			if ( p.frist == EKey::_UNKNOWN and p.second != EKey::_UNKNOWN )
			{
				p.first  = p.second;
				p.second = EKey::_UNKNOWN;
			}
		}

		static bool _IsAllValid (const KeyPair_t &p)
		{
			return p.first != EKey::_UNKNOWN and p.second != EKey::_UNKNOWN;
		}


		void _OnKey (const SysEvent::RawKey &ev)
		{
			_CurrentKeyStates().SetAt( ev.key, ev.pressed );
		}


		bool _IsPressed (EKey::type k)	const	{ return _CurrKeyStates()[k]; }

		bool _IsKeyDown (EKey::type k)	const	{ return _CurrKeyStates()[k] and not _PrevKeyStates()[k]; }

		bool _IsKeyUp (EKey::type k)	const	{ return not _CurrKeyStates()[k] and _PrevKeyStates()[k]; }


		bool _IsPressed (const KeyPair_t &p) const
		{
			return	_IsAllValid( p ) ?
					_IsPressed( p.first ) and _IsPressed( p.second ) :
					_IsPressed( p.frist );
		}

		bool _IsKeyDown (const KeyPair_t &p) const
		{
			return	_IsAllValid( p ) ?
					( _IsPressed( p.first ) and _IsKeyDown( p.second ) ) or
					( _IsKeyDown( p.first ) and _IsPressed( p.second ) ) :
					_IsKeyDown( p.first );
		}

		bool _IsKeyUp (const KeyPair_t &p) const
		{
			return	_IsAllValid( p ) ?
					_IsKeyUp( p.first ) and _IsKeyUp( p.second ) :
					_IsKeyUp( p.first );
		}
	};


}	// GameUtils
}	// Base
}	// Engine
