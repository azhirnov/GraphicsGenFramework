// Copyright © 2014-2016  Zhirnov Andrey. All rights reserved.

#pragma once

#include "Enums.h"

namespace Engine
{
namespace Graphics
{


	//
	// Sampler State
	//

	struct SamplerState : public CompileTime::PODStruct
	{
	// types
	public:
		typedef Vec< EWrapMode::type, 3 >		WrapMode;


	// variables
	public:
		WrapMode			wrapMode;
		EFilter::type		filter;
		ECompareFunc::type	compareRefToTexture;


	// methods
	public:
		SamplerState (GX_DEFCTOR) :
			wrapMode( EWrapMode::Clamp ), filter( EFilter::MinMagMipLinear ),
			compareRefToTexture( ECompareFunc::None )
		{}

		SamplerState (EWrapMode::type wrapMode, EFilter::type filter,
					  ECompareFunc::type compareRefToTexture = ECompareFunc::None) :
			wrapMode( wrapMode ), filter( filter ), compareRefToTexture( compareRefToTexture )
		{}


		bool operator == (const SamplerState &right) const
		{
			return	All( wrapMode == right.wrapMode ) &
					All( filter == right.filter ) &
					( compareRefToTexture == right.compareRefToTexture );
		}


		bool operator >  (const SamplerState &right) const
		{
			return
				wrapMode.x != right.wrapMode.x ?					wrapMode.x > right.wrapMode.x :
				wrapMode.y != right.wrapMode.y ?					wrapMode.y > right.wrapMode.y :
				wrapMode.z != right.wrapMode.z ?					wrapMode.z > right.wrapMode.z :
				filter != right.filter ?							filter > right.filter :
				compareRefToTexture > right.compareRefToTexture;
		}


		bool operator <  (const SamplerState &right) const
		{
			return
				wrapMode.x != right.wrapMode.x ?					wrapMode.x < right.wrapMode.x :
				wrapMode.y != right.wrapMode.y ?					wrapMode.y < right.wrapMode.y :
				wrapMode.z != right.wrapMode.z ?					wrapMode.z < right.wrapMode.z :
				filter != right.filter ?							filter < right.filter :
				compareRefToTexture < right.compareRefToTexture;
		}


		DEBUG_ONLY(
		String ToString () const
		{
			String	str("SamplerState {\n");

			str << "	wrapMode:				";

			FOR( i, wrapMode )
			{
				if ( i > 0 )	str << ", ";
				str << EWrapMode::ToString( wrapMode[i] );
			}

			str << "\n";
			str << "	filter:					" << EFilter::ToString( filter ) << "\n";
			str << "	compareRefToTexture:	" << ECompareFunc::ToString( compareRefToTexture ) << "\n";
			str << "\n}";

			return str;
		})
	};


}	// Graphics
}	// Engine
