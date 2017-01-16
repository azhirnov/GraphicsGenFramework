// Copyright © 2014-2016  Zhirnov Andrey. All rights reserved.

#pragma once

#include "Engine/Base/Common/BaseObject.h"

namespace Engine
{
namespace Base
{

	//
	// Input
	//

	class Input : public BaseObject
	{
	// types
	public:
		
		struct EState
		{
			enum type : ubyte
			{
				Up			= 0x1,	// up in this moment
				Down		= 0x2,	// down in this moment
				_Completed	= 0x4,	// at next moment

				Released	= Up | _Completed,
				Pressed		= Down | _Completed,
			};
		};

		typedef SysEvent::time_t	time_t;

		struct Touch
		{
			// in pixels
			int2	startPos;
			int2	pos;
			int2	delta;

			float	pressure;
			time_t	time;	// time when button pressed
			ubyte	state;
			bool	active  : 1;
			bool	motion	: 1;
			///
			Touch () :
				pressure(0.0f), state(EState::Released),
				active(false), motion(false)
			{}

			bool InMotion ()		const		{ return motion; }
			bool IsPressed ()		const		{ return EnumEq( state, EState::Down ); }
			bool IsClicked ()		const		{ return state == EState::Up; }
			bool IsFirstPress ()	const		{ return state == EState::Down; }

			void PostUpdate ()
			{
				state		|= EState::_Completed;
				delta		 = int2();
				motion		 = false;
			}
		};

	private:

		enum {
			MAX_TOUCHES	= 6,
		};

		typedef StaticArray< Touch, MAX_TOUCHES >		touch_data_t;
		typedef StaticArray< ubyte, EKey::_COUNT >		keys_t;

		SHARED_POINTER( Input );


	// variables
	private:
		touch_data_t	_touches;
		keys_t			_keys;
		float2			_mouseMotionDelta;
		uint			_lastTouchId;
		uint			_numTouches;


	// methods
	private:
		void _OnTouch (const SysEvent::RawTouch &ev);
		void _OnKey (const SysEvent::RawKey &ev);
		void _OnMouseMove (const SysEvent::RawMouseMove &ev);

	public:
		explicit
		Input (const SubSystemsRef ss);

		~Input ();

		void PreUpdate ();
		void PostUpdate ();

		//void EnableSystems (EInputSystem::type type);
		//void DisableSystems (EInputSystem::type type);

		// Touches //
		Touch const &	GetTouch (uint i)	const		{ return _touches[i]; }

		float2 const&	GetMouseMotion ()	const		{ return _mouseMotionDelta; }

		uint			GetLastTouchId ()	const		{ return _lastTouchId; }
		uint			GetNumTouches ()	const		{ return _numTouches; }


		// Keys //
		bool IsKeyPressed (EKey::type key) const
		{
			return EnumEq( _keys[ key ], EState::Down );
		}

		bool IsKeyClicked (EKey::type key)
		{
			uint	state = _keys[ key ];
			_keys[ key ] |= EState::_Completed;
			return state == EState::Up;
		}

		bool IsKeyFirstPress (EKey::type key)
		{
			uint	state = _keys[ key ];
			_keys[ key ] |= EState::_Completed;
			return state == EState::Down;
		}
	};

}	// Base
}	// Engine