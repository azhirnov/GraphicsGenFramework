// Copyright © 2014-2016  Zhirnov Andrey. All rights reserved.

#include "RenderState.h"

namespace Engine
{
namespace Graphics
{
DEBUG_ONLY(	
/*
=================================================
	ToString
=================================================
*/
	String RenderState::ColorBufferState::ToString () const
	{
		return String( "ColorBufferState {\n" )
			<< "	blend:           " << blend << '\n'
			<< "	blendFuncSrcRGB: " << EBlendFunc::ToString( blendFuncSrcRGB ) << '\n'
			<< "	blendFuncSrcA:   " << EBlendFunc::ToString( blendFuncSrcA )   << '\n'
			<< "	blendFuncDstRGB: " << EBlendFunc::ToString( blendFuncDstRGB ) << '\n'
			<< "	blendFuncDstA:   " << EBlendFunc::ToString( blendFuncDstA )   << '\n'
			<< "	blendModeRGB:    " << EBlendEq::ToString( blendModeRGB )      << '\n'
			<< "	blendModeA:      " << EBlendEq::ToString( blendModeA )        << '\n'
			<< "	colorMask:       (" << colorMask.ToString() << ")\n}";
	}

/*
=================================================
	ToString
=================================================
*/
	String RenderState::DepthBufferState::ToString () const
	{
		return String( "DepthBufferState {\n" )
			<< "	test:  " << test << '\n'
			<< "	func:  " << ECompareFunc::ToString( func ) << '\n'
			<< "	range: (" << range.ToString() << ")\n"
			<< "	write: " << write << '\n'
			<< "	clamp: " << clamp << "\n}";
	}

/*
=================================================
	ToString
=================================================
*/
	String RenderState::StencilBufferState::ToString () const
	{
		return String( "StencilBufferState {\n" )
			<< "	test:     " << test << '\n'
			<< "	sfail:    " << EStencilOp::ToString( sfail ) << '\n'
			<< "	dfail:    " << EStencilOp::ToString( dfail ) << '\n'
			<< "	dppass:   " << EStencilOp::ToString( dppass ) << '\n'
			<< "	func:     " << ECompareFunc::ToString( func ) << '\n'
			<< "	funcRef:  " << String().FormatI( funcRef, 16 ) << '\n'
			<< "	funcMask: " << String().FormatI( funcMask, 16 ) << '\n'
			<< "	mask:     " << String().FormatI( mask, 16 ) << "\n}";
	}

/*
=================================================
	ToString
=================================================
*/
	String RenderState::CullMode::ToString () const
	{
		return String( "CullMode {\n" )
			<< "	cullBackFace: " << cullBackFace << '\n'
			<< "	frontFaceCCW: " << frontFaceCCW << '\n'
			<< "	enabled:      " << enabled << "\n}";
	}

/*
=================================================
	ToString
=================================================
*/
	String RenderState::PolygonStates::ToString () const
	{
		return String( "PolygonStates {\n" )
			<< "	mode:          " << EPolygonMode::ToString( mode ) << '\n'
			<< "	offsetFactor:  " << offsetFactor << '\n'
			<< "	offsetUnits:   " << offsetUnits << '\n'
			<< "	offsetEnabled: " << offsetEnabled << "\n}";
	}

/*
=================================================
	ToString
=================================================
*/
	String RenderState::PointState::ToString () const
	{
		return String( "PointState {\n" )
			<< "	pointSize:         " << pointSize << '\n'
			<< "	fadeThresholdSize: " << fadeThresholdSize << '\n'
			<< "	spriteCoordOrigin: " << EPointSpriteCoordOrigin::ToString( spriteCoordOrigin ) << '\n'
			<< "	programPointSize:  " << programPointSize << "\n}";
	}

/*
=================================================
	ToString
=================================================
*/
	String RenderState::ToString () const
	{
		String	str;

		FOR( i, colors ) {
			str << '[' << i << ']' << colors[i].ToString() << '\n';
		}

		str << "blendColor:  (" << blendColor.ToString() << ")\n"
			<< depth.ToString() << '\n'
			<< stencil.ToString() << '\n'
			<< cullMode.ToString() << '\n'
			<< polygon.ToString() << '\n'
			<< point.ToString();

		return str;
	}

)	// DEBUG_ONLY

}	// Graphics
}	// Engine
