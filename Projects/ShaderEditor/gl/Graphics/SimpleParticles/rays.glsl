
// uniforms //
layout(std140)
uniform ShaderUB {
	float4x4		proj;
	float4x4		mv;
	float2			screenSize;
} UB;


// shader input/output data types //
#define TVertData \
TVertData_t { \
	float4	startPos; \
	float4	endPos; \
	float4	color; \
	float	size; \
}

#define TGeomData \
TGeomData_t { \
	float2	uv; \
	float4	color; \
}

#import "Core/Math/Math.glcs"


#if SHADER == SH_VERTEX

	out	TVertData	Output;

	void main ()
	{
		Output.startPos	= UB.mv * float4( ATTRIB_0.xyz, 1.0 );
		Output.endPos	= UB.mv * float4( ATTRIB_1.xyz, 1.0 );
		Output.color	= ATTRIB_2.rgba;
		Output.size		= ATTRIB_3 / Max( UB.screenSize.x, UB.screenSize.y );
	}

#endif	// SH_VERTEX
//-----------------------------------------------------------------------------


#if SHADER == SH_GEOMETRY

	layout (points) in;
	layout (triangle_strip, max_vertices = 4) out;
	
	in	TVertData	Input[];
	out	TGeomData	Output;
	

	// check is point inside oriented rectangle
	bool IsPointInside (in float2 a, in float2 b, in float2 c, in float2 m)
	{
		float2	ab		= b - a;
		float2	bc		= c - b;
		float2	am		= m - a;
		float2	bm		= m - b;

		float	ab_am	= Dot( ab, am );
		float	ab_ab	= Dot( ab, ab );
		float	bc_bm	= Dot( bc, bm );
		float	bc_bc	= Dot( bc, bc );

		return	ab_am >= 0.0f  and bc_bm >= 0.0f and
				ab_am <= ab_ab and bc_bm <= bc_bc;
	}

	void main ()
	{
		//        _ _
		//      /\ / \		    \ /
		//     /  s  /		     s
		//    /  / \/		    / \
		//   /\ /  /		 \ /
		//  \  e  /			  e
		//   \/_\/			 / \
		
		float4	start	= Input[0].startPos;
		float4	end		= Input[0].endPos;
		float2	dir		= Normalize( end.xy - start.xy );
		float2	norm	= float2( -dir.y, dir.x );

		float	size	= Input[0].size * 0.5;	// rotated rectangle size
		float	side	= size * 0.95;			// size with error
		float4	color	= Input[0].color;
		
		float2	rect_a	= start.xy + norm * size;
		float2	rect_b	= start.xy - norm * size;
		float2	rect_c	= end.xy - norm * size;

		float4	points[8]	= float4[](
			start + float4( norm + dir, 0.0, 0.0) * side,
			start + float4( norm - dir, 0.0, 0.0) * side,
			start + float4(-norm - dir, 0.0, 0.0) * side,
			start + float4(-norm + dir, 0.0, 0.0) * side,
			end   + float4( norm + dir, 0.0, 0.0) * side,
			end   + float4( norm - dir, 0.0, 0.0) * side,
			end   + float4(-norm - dir, 0.0, 0.0) * side,
			end   + float4(-norm + dir, 0.0, 0.0) * side
		);

		const float2	uv_coords[8] = float2[](
			float2(0.0, 1.0),
			float2(0.0, 0.0),
			float2(1.0, 1.0),
			float2(1.0, 0.0),
			// some invalid values for safety
			float2(2.0), float2(2.0), float2(2.0), float2(2.0)
		);

		// find external points (must be 4 points)
		for (int i = 0, j = 0; i < points.length and j < 4; ++i)
		{
			if ( not IsPointInside( rect_a, rect_b, rect_c, points[i].xy ) )
			{
				gl_Position	 = UB.proj * points[i];
				Output.uv	 = uv_coords[j];
				Output.color = color;

				EmitVertex();
				++j;
			}
		}

		EndPrimitive();
	}

#endif	// SH_GEOMETRY
//-----------------------------------------------------------------------------


#if SHADER == SH_FRAGMENT
	
	in	TGeomData	Input;

	void main()
	{
		outColor0 = Input.color * (1.0 - Distance( ToSNorm(Input.uv), float2(0.0) ));
	}

#endif	// SH_FRAGMENT
