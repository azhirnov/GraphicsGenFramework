// Copyright © 2014-2017  Zhirnov Andrey. All rights reserved.

#pragma once

#include "Application.h"
#include "Display.h"
#include "Enums.h"
#include "VideoSettings.h"

namespace Engine
{
namespace Base
{

	//
	// Platform interface
	//

	class Platform : public VirtualThread
	{
	// types
	public:
		struct WindowDesc
		{
		// variables
			StringCRef		caption;
			uint2			size;
			int2			pos;			// set min value to align in center
			bool			fullscreen;
			bool			resizeble;

		// methods
			WindowDesc () :
				caption(), size(0), pos(), fullscreen(false), resizeble(false)
			{}

			WindowDesc (StringCRef caption, const uint2 &size, const int2 &pos, bool fullscreen, bool resizeble) :
				caption(caption), size(size), pos(pos), fullscreen(fullscreen), resizeble(resizeble)
			{}
		};


		struct MemoryDesc
		{
		// variables
			Bytes<ulong>		total;
			Bytes<ulong>		available;
		};


		struct CursorDesc
		{
		// variables
			bool	visible;
			bool	alwaysInCenter;

		// methods
			CursorDesc () :
				visible(true), alwaysInCenter(false)
			{}
				
			CursorDesc (bool visible, bool alwaysInCenter) :
				visible(visible), alwaysInCenter(alwaysInCenter)
			{}
		};


	// methods
	protected:
		Platform (const IParallelThreadPtr &thread) :
			VirtualThread( thread )
		{
			SubSystems()->GetSetter< Platform >().Set( this );
		}

		~Platform ()
		{
			SubSystems()->GetSetter< Platform >().Set( null );
		}


	// interface
	public:

		// Render //
		virtual bool InitDisplay (const Display &disp) = 0;
		virtual bool InitWindow (const WindowDesc &window) = 0;
		virtual bool InitRender (const VideoSettings &vs) = 0;
		virtual void SetDisplayOrientation (EDisplayOrientation::type orientation) = 0;
		virtual void SwapBuffers () = 0;
		

		// Get //
		virtual void GetDisplay (OUT Display &display) const = 0;
		virtual void GetWindowDesc (OUT WindowDesc &window) const = 0;
		virtual void GetVideoSettings (OUT VideoSettings &vs) const = 0;
		virtual void GetCursorState (OUT CursorDesc &cursor) const = 0;
		virtual void GetMemory (OUT MemoryDesc &mem) const = 0;


		// Audio //
		virtual void EnableAudio (bool enabled) = 0;
		

		// Other //
		virtual void SetCursorState (const CursorDesc &cursor) = 0;

		virtual void Exit () = 0;

		virtual void OpenURL (StringCRef url) = 0;

		virtual StringCRef 	GetDirectoryPath (EDirectory::type type) = 0;
	};


}	// Base
}	// Engine