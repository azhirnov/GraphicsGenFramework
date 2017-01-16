// Copyright © 2017  Zhirnov Andrey. All rights reserved.

#include "GLSL.h"


const uint2	unMaxSubCellSize = uint2(6);

const uint2	unCellSize = uint2(10);


float2 Hash22 (float2 p)
{
    float2 q = float2( Dot( p, float2(127.1f, 311.7f) ), 
						Dot( p, float2(269.5f, 183.3f) ) );
    
	return Fract( Sin(q) * 43758.5453f );
}


bool InRect (uint4 rect, uint2 pos)
{
	return	AllGreaterEqual( pos, rect.xy() ) and
			AllLess( pos, rect.zw() );
}


uint2 SubCellSize (uint2 global, uint2 local)
{
    float2	h = Hash22(float2(global));
    uint2	s = uint2( Round( float2(unMaxSubCellSize) * h ) );
    
    return Clamp( s, uint2(1), unCellSize - local );
}


void GLSLMain (uint2 globalCoord)
{
	const uint maxRows = 16;

	uint4	rowCache[ maxRows * 2 ];

	uint2	size		= Min( unCellSize, uint2(maxRows) );
	uint2	global		= globalCoord * size;
	uint2	local		= uint2(0);
	uint	cache_write	= 0;	// current row
	uint	cache_read	= 1;	// y-1 row

	for (local.y = 0; local.y < size.y; ++local.y)
	{
		cache_read	= cache_write;
		cache_write	= (cache_write+1) & 1;

		for (local.x = 0; local.x < size.x; ++local.x)
		{
			uint4	rect;

			if ( local.x > 0 )
			{
				rect = rowCache[ local.x-1 + cache_write * maxRows ];

				if ( InRect( rect, local ) )
				{
					rowCache[ local.x + cache_write * maxRows ] = rect;
					//imageStore( outGridDataImage, int2(global + local), rect );
					continue;
				}
			}
			
			if ( local.y > 0 )
			{
				rect = rowCache[ local.x + cache_read * maxRows ];

				if ( InRect( rect, local ) )
				{
					rowCache[ local.x + cache_write * maxRows ] = rect;
					//imageStore( outGridDataImage, int2(global + local), rect );
					continue;
				}
			}
			
			rect = uint4( local, local + SubCellSize( global + local, local ) );

			rowCache[ local.x + cache_write * maxRows ] = rect;
			//imageStore( outGridDataImage, int2(global + local), rect );
		}
	}
}


extern void TestShader_Temp ()
{
	GLSLMain( uint2(0,0) );
}
