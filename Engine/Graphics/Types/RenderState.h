// Copyright © 2014-2017  Zhirnov Andrey. All rights reserved.

#pragma once

#include "Enums.h"

namespace Engine
{
namespace Graphics
{

	//
	// Render State
	//
	
	struct RenderState : public CompileTime::PODStruct		// TODO: copy from StaticArray<...>
	{
	// types
	public:

		//
		// Color Buffer State
		//
		struct ColorBufferState : public CompileTime::PODStruct
		{
		// variables
			EBlendFunc::type	blendFuncSrcRGB,
								blendFuncSrcA,
								blendFuncDstRGB,
								blendFuncDstA;
			EBlendEq::type		blendModeRGB,
								blendModeA;
			bool4				colorMask;
			bool				blend;
		
		// methods
			ColorBufferState (GX_DEFCTOR) :
				blendFuncSrcRGB( EBlendFunc::One ), blendFuncSrcA( EBlendFunc::One ),
				blendFuncDstRGB( EBlendFunc::Zero ), blendFuncDstA( EBlendFunc::Zero ),
				blendModeRGB( EBlendEq::Add ), blendModeA( EBlendEq::Add ),
				colorMask( true ), blend( false )
			{}

			DEBUG_ONLY( String ToString () const; )
		};
		
		//
		// Depth Buffer State
		//
		struct DepthBufferState : public CompileTime::PODStruct
		{
		// variables
			ECompareFunc::type	func;
			float2				range;
			bool				write,
								test,
								clamp;

		// methods
			DepthBufferState (GX_DEFCTOR) :
				func( ECompareFunc::LEqual ), range( 0.0f, 1.0f ),
				write( true ), test( false ), clamp( false )
			{}

			DEBUG_ONLY( String ToString () const; )
		};
		
		//
		// Stencil Buffer State
		//
		struct StencilBufferState : public CompileTime::PODStruct
		{
		// variables
			EStencilOp::type	sfail,
								dfail,
								dppass;
			ECompareFunc::type	func;
			ubyte				funcRef,
								funcMask,
								mask;
			bool				test;

		// methods
			StencilBufferState (GX_DEFCTOR) :
				sfail( EStencilOp::Keep ), dfail( EStencilOp::Keep ),
				dppass( EStencilOp::Keep ), func( ECompareFunc::Always ),
				funcRef( 0 ), funcMask( -1 ), mask( -1 ), test( false )
			{}

			DEBUG_ONLY( String ToString () const; )
		};

		//
		// Cull Mode
		//
		struct CullMode : public CompileTime::PODStruct
		{
		// variables
			bool	cullBackFace;
			bool	frontFaceCCW;
			bool	enabled;
			
		// methods
			CullMode(GX_DEFCTOR):
				cullBackFace( true ), frontFaceCCW( true ), enabled( false )
			{}

			DEBUG_ONLY( String ToString () const; )
		};
		
		//
		// Polygon States
		//
		struct PolygonStates : public CompileTime::PODStruct
		{
		// variables
			EPolygonMode::type	mode;
			float				offsetFactor,
								offsetUnits;
			bool				offsetEnabled;

		// methods
			PolygonStates (GX_DEFCTOR) :
				mode( EPolygonMode::Fill ), offsetFactor( 0.0f ), offsetUnits( 0.0f ), offsetEnabled( false )
			{}

			DEBUG_ONLY( String ToString () const; )
		};

		//
		// Point State
		//
		struct PointState : public CompileTime::PODStruct
		{
		// variables
			float							pointSize;
			float							fadeThresholdSize;
			EPointSpriteCoordOrigin::type	spriteCoordOrigin;
			bool							programPointSize;

		// methods
			PointState (GX_DEFCTOR) :
				pointSize( 1.0f ), fadeThresholdSize( 1.0f ),
				spriteCoordOrigin( EPointSpriteCoordOrigin::UpperLeft ), programPointSize( false )
			{}

			DEBUG_ONLY( String ToString () const; )
		};

		typedef StaticArray< ColorBufferState, GlobalConst::Graphics_MaxColorBuffers >		ColorBuffers_t;


	// variables
	public:
		ColorBuffers_t		colors;
		color4f				blendColor;
		DepthBufferState	depth;
		StencilBufferState	stencil;
		CullMode			cullMode;
		PolygonStates		polygon;
		PointState			point;


	// methods
	public:
		RenderState (GX_DEFCTOR) :
			blendColor( 1.0f )
		{}

		DEBUG_ONLY( String ToString () const; )
	};

				
}	// Graphics
}	// Engine
