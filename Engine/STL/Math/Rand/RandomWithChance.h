// Copyright � 2014-2017  Zhirnov Andrey. All rights reserved.

#pragma once

#include "Random.h"
#include "NormalDistribution.h"

namespace GX_STL
{
namespace GXMath
{

	//
	// Random Generator With Chance
	//

	template <typename RndEngine>
	struct TRandomGenerator<RndEngine>::WithChance : public Noninstancable
	{
		typedef TRandomGenerator<RndEngine>	RG;
		
		
		template <typename T, usize I>
		static bool CheckChances (const T (&chances)[I])
		{
			return CheckChances( Buffer<const T>( chances ) );
		}
		

		template <typename T>
		static bool CheckChances (Buffer<const T> chances)
		{
			typedef typename TypeTraits::RemoveConst<T>		value_t;

			value_t	summ(0);
			
			FOR( i, chances ) {
				summ += chances[i];
			}
			return Equals( summ, value_t(1) );
		}

		
		template <typename T, usize I>
		static usize Index (const T (&chances)[I])
		{
			return Index( Buffer<const T>( chances ) );
		}
		

		template <typename T>
		static usize Index (Buffer<const T> chances)
		{
			typedef typename TypeTraits::RemoveConst<T>		value_t;

			value_t f = RG::UNorm< value_t >();
		
			FOR( i, chances )
			{
				if ( f < chances[i] )
					return i;

				f -= chances[i];
			}
			return chances.Count();
		}

		
		template <typename T, usize I, typename B>
		static usize Index (const T (&chances)[I], B expected)
		{
			return Index( Buffer<const T>( chances ), expected );
		}
		

		template <typename T, typename B>
		static usize Index (Buffer<const T> chances, B expected)
		{
			typedef typename TypeTraits::RemoveConst<T>		value_t;

			value_t f = RG::NormalDistribution::UNorm< value_t >( expected );
		
			FOR( i, chances )
			{
				if ( f < chances[i] )
					return i;

				f -= chances[i];
			}
			return chances.Count();
		}
		
		/*
		template <typename T>
		static T Indexed (Buffer<const T> values, const usize expectedIndex)
		{
			typedef TRandomGenerator<RndEngine>::NormalDistribution		NormalDistribution;

			const float	idx = NormalDistribution::UNorm( float(expectedIndex) / values.Count() ) * values.Count();
			return values[ Clamp( Round<usize>( idx ), 0, values.LastIndex() ) ];
		}
		*/

		template <typename T>
		static bool Bool (T chanceOfTrue)
		{
			return UNorm<T>() < chanceOfTrue;
		}
	};


}	// GXMath
}	// GX_STL
