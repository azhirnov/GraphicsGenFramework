// Copyright © 2014-2017  Zhirnov Andrey. All rights reserved.

#pragma once

#include "Engine/Base/Common/Common.h"

namespace Engine
{
namespace Base
{
	
	//
	// Video Settings
	//

	struct VideoSettings
	{
	// types
		enum EColorFormat
		{
			R8_G8_B8_A8		= 0x08080808,
			R4_G4_B4_A4		= 0x04040404,
			R5_G6_B5		= 0x05060500,

			RGBA8			= R8_G8_B8_A8,
			RGBA4			= R4_G4_B4_A4,
			RGB565			= R5_G6_B5,
		};

		enum EDepthFromat : ubyte
		{
			NO_DEPTH		= 0,
			DEPTH_16		= 16,
			DEPTH_24		= 24,
			DEPTH_32		= 32,
		};

		enum EStencilFormat : ubyte
		{
			NO_STENCIL		= 0,
			STENCIL_8		= 8,
		};

		enum EVersion
		{
			// GLES
			OPENGLES		= 0x10000000,
			OPENGLES_2_0	= OPENGLES | 0x20,
			OPENGLES_3_0	= OPENGLES | 0x30,
			OPENGLES_LAST	= OPENGLES_3_0,		// last known version
			OPENGLES_AUTO	= OPENGLES | 0xFF,

			// GL
			OPENGL			= 0x20000000,
			OPENGL_3_3		= OPENGL | 0x33,
			OPENGL_4_5		= OPENGL | 0x45,
			OPENGL_LAST		= OPENGL_4_5,		// last known version
			OPENGL_AUTO		= OPENGL | 0xFF,

			// DirectX
			DIRECTX			= 0x40000000,
			DIRECTX_9_3		= DIRECTX | 0x93,
			DIRECTX_11_0	= DIRECTX | 0xB0,
			DIRECTX_12_0	= DIRECTX | 0xC0,
			DIRECTX_LAST	= DIRECTX_12_0,		// last known version
			DIRECTX_AUTO	= DIRECTX | 0xFF,

			AUTO			= 0xFFFFFFFF
		};

		enum EContextFlags
		{
			CONTEXT_NONE		= 0,
			CONTEXT_DEBUG		= 1 << 0,
			CONTEXT_NO_ERROR	= 1 << 1,
		};


	// variables
		EColorFormat	colorFormat;	// R, G, B, A size in bits
		EVersion		version;
		EDepthFromat	depthBits;
		EStencilFormat	stencilBits;
		EContextFlags	contextFlags;
		ubyte			multiSamples;
		bool			stereo;
		bool			vsync;
		

	// methods
		VideoSettings (	EColorFormat	colorFmt = RGBA8,
						EDepthFromat	depth    = DEPTH_24,
						EStencilFormat	stencil  = NO_STENCIL,
						ubyte			samples  = 0,
						EVersion		ver		 = AUTO,
					    EContextFlags	flags	 = CONTEXT_NONE,
						bool			vsync	 = true,
						bool			stereo	 = false) :
			colorFormat(colorFmt),	depthBits(depth),
			stencilBits(stencil),	multiSamples(samples),
			version(ver),			contextFlags(flags),
			stereo(stereo),			vsync(vsync)
		{}

		uint RedBits ()				const	{ return (colorFormat >> 24) & 0xFF; }
		uint GreenBits ()			const	{ return (colorFormat >> 16) & 0xFF; }
		uint BlueBits ()			const	{ return (colorFormat >>  8) & 0xFF; }
		uint AlphaBits ()			const	{ return (colorFormat & 0xFF); }

		uint MajorVersion ()		const	{ return (version & 0xF0) >> 4; }
		uint MinorVersion ()		const	{ return (version & 0xF); }

		bool IsOpenGLES ()			const	{ return EnumEq( version, OPENGLES ); }
		bool IsOpenGL ()			const	{ return EnumEq( version, OPENGL ); }
		bool IsDirectX ()			const	{ return EnumEq( version, DIRECTX ); }
		bool IsAutoRenderer ()		const	{ return EnumEq( version, AUTO ); }

		bool IsDebugContext ()		const	{ return EnumEq( contextFlags, CONTEXT_DEBUG ); }
		bool IsNoErrorContext ()	const	{ return EnumEq( contextFlags, CONTEXT_NO_ERROR ); }

		void ValidateVersion (bool useKnownVersions = false)
		{
			bool is_valid = (version >= OPENGLES_2_0 and version <= OPENGLES_AUTO) or
							(version >= OPENGL_3_3   and version <= OPENGL_AUTO)   or
							(version >= DIRECTX_9_3  and version <= DIRECTX_AUTO);

			if ( not is_valid or version == AUTO )
			{
#				if defined( GRAPHICS_API_OPENGL )
					version = OPENGL_AUTO;
#				elif defined( GRAPHICS_API_OPENGLES )
					version = OPENGLES_AUTO;
#				elif defined( GRAPHICS_API_DIRECTX )
					version = DIRECTX_AUTO;
#				else
#					pragma message( "Graphics API not defined!" )
#				endif
			}

			if ( useKnownVersions )
			{
				if ( version == OPENGL_AUTO )
					version = OPENGL_LAST;

				if ( version == OPENGLES_AUTO )
					version = OPENGLES_LAST;
			
				if ( version == DIRECTX_AUTO )
					version = DIRECTX_LAST;
			}
		}
	};

}	// Base
}	// Engine