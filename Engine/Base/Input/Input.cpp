// Copyright © 2014-2016  Zhirnov Andrey. All rights reserved.

#include "Engine/Base/OS/OS.h"
#include "Input.h"

namespace Engine
{
namespace Base
{
	
/*
=================================================
	constructor
=================================================
*/
	Input::Input (const SubSystemsRef ss) :
		BaseObject( ss ),
		_lastTouchId(0), _numTouches(0), _keys( (ubyte)EState::Released )
	{
		SubSystems()->GetSetter< Input >().Set( this );
		
		EventSystemPtr	es = SubSystems()->Get< Application >()->GetEventSystem();

		es->Subscribe( DelegateBuilder::Create( InputPtr(this), &Input::_OnTouch ) );
		es->Subscribe( DelegateBuilder::Create( InputPtr(this), &Input::_OnKey ) );
	}
	
/*
=================================================
	destructor
=================================================
*/
	Input::~Input ()
	{
		EventSystemPtr	es = SubSystems()->Get< Application >()->GetEventSystem();

		if ( es.IsNotNull() )
		{
			es->Unsubscribe( DelegateBuilder::Create( InputPtr(this), &Input::_OnTouch ) );
			es->Unsubscribe( DelegateBuilder::Create( InputPtr(this), &Input::_OnKey ) );

			//es->Wait();
		}
		
		SubSystems()->GetSetter< Input >().Set( null );
	}
	
/*
=================================================
	_OnTouch
=================================================
*/
	void Input::_OnTouch (const SysEvent::RawTouch &ev)
	{
		_lastTouchId = ev.id;

		Touch &	touch = _touches[ _lastTouchId ];

		const int2	pos = RoundToInt( ev.point );

		if ( EnumEq( touch.state, EState::Up ) and ev.pressed )
		{
			touch.startPos	= pos;
			touch.pos		= pos;	// for zero delta
			touch.time		= ev.timestamp;
		}

		const ubyte	state	 = ev.pressed ? EState::Down : EState::Up;

		touch.active	 = true;
		touch.delta		+= pos - touch.pos;
		touch.pos		 = pos;
		touch.motion	 = ev.pressed and ev.inMotion;
		touch.state		 = EnumEq( touch.state, state ) ? (touch.state | state) : state;
		touch.pressure	 = ev.pressure;

		_numTouches = Max<uint>( _numTouches, _lastTouchId+1 );
	}
	
/*
=================================================
	_OnKey
=================================================
*/
	void Input::_OnKey (const SysEvent::RawKey &ev)
	{
		_keys[ ev.key ] = ev.pressed ? EState::Down : EState::Up;
	}
	
/*
=================================================
	_OnMouseMove
=================================================
*/
	void Input::_OnMouseMove (const SysEvent::RawMouseMove &ev)
	{
		_mouseMotionDelta += ev.delta;
	}

/*
=================================================
	PreUpdate
=================================================
*/
	void Input::PreUpdate ()
	{
		// TODO:
		// - detect zooming and rotating
		// - motion tracker

	}
	
/*
=================================================
	PostUpdate
=================================================
*/
	void Input::PostUpdate ()
	{
		_numTouches			= 0;
		_lastTouchId		= 0;
		_mouseMotionDelta	= float2();

		FOR( i, _touches )
		{
			_touches[i].PostUpdate();
		}

		_keys[ EKey::M_WHEEL_UP ]	= EState::Released;
		_keys[ EKey::M_WHEEL_DOWN ]	= EState::Released;
	}


}	// Base
}	// Engine