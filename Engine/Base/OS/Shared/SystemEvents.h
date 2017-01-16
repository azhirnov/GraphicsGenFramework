// Copyright © 2014-2016  Zhirnov Andrey. All rights reserved.

#pragma once

#include "Engine/Base/Events/EventSystem.h"

namespace Engine
{
namespace Base
{

	struct SysEvent
	{
	// types
	public:
		typedef Time<double>	time_t;

		struct Update
		{
		// variables
			time_t		timeDelta;
			bool		forceRedraw;
			bool		renderAvailable;

		// methods
			Update (const time_t &timeDelta, bool forceRedraw, bool renderAvailable) :
				timeDelta(timeDelta),
				forceRedraw(forceRedraw),
				renderAvailable(renderAvailable)
			{}
		};

		struct RawTouch
		{
		// variables
			time_t		timestamp;
			float2		point;
			float		pressure;
			ushort		id;
			bool		pressed;
			bool		inMotion;

		// methods
			RawTouch (const time_t &timestamp, const float2 &point, float pressure, uint id, bool pressed, bool inMotion) :
				timestamp(timestamp), point(point), pressure(pressure), id((ushort)id), pressed(pressed), inMotion(inMotion)
			{}
		};

		struct RawKey
		{
		// variables
			time_t		timestamp;
			EKey::type	key;
			bool		pressed;

		// methods
			RawKey (const time_t &timestamp, EKey::type key, bool pressed) :
				timestamp(timestamp), key(key), pressed(pressed)
			{}
		};
		
		struct RawMouseMove
		{
		// variables
			time_t		timestamp;
			float2		delta;

		// methods
			RawMouseMove (const time_t &timestamp, const float2 &delta) :
				timestamp(timestamp), delta(delta)
			{}
		};

		struct Application
		{
		// types
			enum EType
			{
				NONE = 0,
				CREATED,			// after application creation (on enter to render thread)
				DESTROY,			// before aplication destroing (on exit of render thread)
				ENTER_BACKGROUND,	// pause
				ENTER_FOREGROUND,	// resume
				SURFACE_CREATED,	// after this, rendering is available
				SURFACE_DESTROYED,	// after this, rendering is not available
				LOW_MEMORY,			// application in background will be destroyed
			};

		// variables
			EType	type;
			bool	completed;		// ENTER_BACKGROUND, ENTER_FOREGROUND	// state changing pending or completed

		// methods
			explicit
			Application (EType type, bool completed = true) :
				type(type), completed(completed)
			{
				ASSERT( completed or (type == ENTER_BACKGROUND or type == ENTER_FOREGROUND) );
			}
		};
		
		struct Audio
		{
		// types
			enum EType
			{
				NONE = 0,
				SET_FOCUS,		// load audio resources in this event
				KILL_FOCUS,		// release audio resource in this event
				PAUSE,			// pause audio and may release some resources in this event
			};

		// variables
			EType	type;

		// methods
			explicit
			Audio ( EType type) :
				type(type)
			{}
		};

		struct Window
		{
		// types
			enum EType
			{
				RESIZED = 1,
				SHOWN,
				HIDDEN,
				ORIENTATION_CHANGED,
			};

		// variables
			EType						type;
			int2						size;			// RESIZED
			EDisplayOrientation::type	orientation;	// ORIENTATION_CHANGED

		// methods
			explicit
			Window (EType type) :
				type(type)
			{}

			Window (EType type, const int2 &size) :
				type(type), size(size)
			{
				ASSERT( type == RESIZED );
			}

			Window (EType type, EDisplayOrientation::type orient) :
				type(type), orientation(orient)
			{
				ASSERT( type == ORIENTATION_CHANGED );
			}
		};

		struct BeforeFlushFrame
		{
			// TODO
		};

		struct AfterFlushFrame
		{
			// TODO
		};
	};



}	// Base
}	// Engine
