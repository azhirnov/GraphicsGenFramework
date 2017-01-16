// Copyright © 2014-2016  Zhirnov Andrey. All rights reserved.

#pragma once


// Emulators //

//#define ANDROID_EMULATOR	// TODO: EMULATE_ANDROID
//#define IOS_EMULATOR		// TODO: EMULATE_IOS

//-------------------------------------------------------------------



// Options //

#define OP_ENABLE_PROFILING

//#define OP_AUDIO_IN_SEPARATE_THREAD

//#define OP_USE_LODEPNG

// OS Platform
//#define PLATFORM_SDL		// define this in STL/Defines/DefConfig.h

// Graphics
#define GRAPHICS_API_OPENGL		450		// 4.x
//#define GRAPHICS_API_OPENGLES	200		// 2.x
//#define GRAPHICS_API_DIRECTX	110		// 11.x

// Compute
//#define COMPUTE_API_OPENCL		120		// 1.x or 2.x
//#define COMPUTE_API_DIRECTX	110			// 11.x
//#define COMPUTE_API_CUDA		7000		// --
#define COMPUTE_API_OPENGL		450			// 4.x

//-------------------------------------------------------------------



// Error Checks //

// Graphics API error checks
#if !defined( __GX_FAST__ )
#	define __GX_GRAPHICS_API_ERROR_CHECKS__
#endif

// Compute API error checks, exept: OpenGL, DirectX
#if !defined( __GX_FAST__ )
#	define __GX_COMPUTE_API_ERROR_CHECKS__
#endif

//-------------------------------------------------------------------



// Constants //

namespace GX_STL
{
	//
	// Global Constants
	//

	namespace GlobalConst
	{
		static const int	Base_MaxSubSystems					= 64;

		static const int	Graphics_MaxColorBuffers			= 6;	// cube map
		static const int	Graphics_MaxAttribs					= 16;
		static const int	Graphics_MaxMaterialTextures		= 8;
		static const int	Graphics_MaxMaterialUniforms		= 16;
		static const int	Graphics_MaxRenderPassTextures		= 6;	// render pass input textures
		static const int	Graphics_MaxViewports				= 16;
		static const int	Graphics_MaxTextureSize				= 1<<14;
		static const int	Graphics_MaxModelDrawGroups			= 32;
		static const int	Graphics_MaxRenderPassDrawGroups	= 32;
		static const int	Graphics_MaxUniformBlocks			= 6;
		static const int	Graphics_MaxUniformBlockSizeKb		= 128;
		static const int	Graphics_MaxUniformNameLength		= 32;
		static const int	Graphics_MaterialCompatibilityBits	= 32;

		static const int	Compute_MaxTextureSize				= 1<<14;
	};

}	// Engine


//---------------------------------------------------------
// Auto-Config

/*
// Emulator //
#ifdef PLATFORM_TYPE_DESCTOP

#	define MOBILE_EMULATOR
#	define GLES_EMULATE

	// Android
#	ifdef ANDROID_EMULATOR
#		define _POSIX_EMULATOR_
#	endif

	// use SDL for iOS platform emulator
#	ifdef IOS_EMULATOR
#		define PLATFORM_SDL
//#		define _POSIX_EMULATOR_		// 
#	endif

//#	define USE_ANGLE_GLES	// TODO

#else	// not PLATFORM_TYPE_DESCTOP

	// no emulators for mobile systems
#	undef ANDROID_EMULATOR
#	undef IOS_EMULATOR

	// use SDL for iOS platform
#	ifdef PLATFORM_IPHONE
#		define PLATFORM_SDL
#	endif

#endif	// PLATFORM_TYPE_DESCTOP


// Emulate OpenGL ES 2.0 //
#if defined( USE_ANGLE_GLES ) and defined( GLES_EMULATE )
#	undef GLES_EMULATE
#	pragma message ("used 3ty party emulator, GLES_EMULATE undefined")
#endif


// Mobile Emulator //
#ifdef MOBILE_EMULATOR
#	define EMULATOR_ONLY	ARGS
#else
#	define EMULATOR_ONLY	__EMPTY_ARGS__
#endif
*/
