
// uniforms //
layout(std140)
uniform TerrainUB {
	real4x4		mvp;
	real3		lightDir;
	real		gridScale;
	real		maxTessLevel;
	real		heightScale;
	real		detailLevel;
	real		scrAspect;
} terrainUB;

uniform sampler2D		unHeightMap;
uniform sampler2D		unNormalMap;
uniform sampler2D		unColoredLine;


// shader input/output data types //
#define TVertData \
TVertData_t { \
	real3	normal; \
	real2	scrCoords; \
	float2	texcoord; \
	float	level; \
	bool	inScreen; \
}

#define TContData \
TContData_t { \
	real3	normal; \
	float2	texcoord; \
	float	level; \
}

#define TEvalData \
TEvalData_t { \
	float2	texcoord; \
	real	height; \
}


#if SHADER == SH_VERTEX

	out	TVertData	Output;

	bool InScreen (in real2 pos)
	{
		real	size = terrainUB.scrAspect * 1.2;
		return ( abs(pos.x) <= size and abs(pos.y) <= size );
	}

	real Level (real dist)
	{
		real d = terrainUB.detailLevel * terrainUB.gridScale * 0.05 / abs(dist) - 2.0;
		return clamp( d, 1.0, terrainUB.maxTessLevel );
	}

	void main ()
	{
		#if defined(ATTRIB_1) && defined(ATTRIB_2)
			gl_Position		= float4( ATTRIB_0.xyz * terrainUB.gridScale, 1.0 );
			Output.normal	= ATTRIB_1.xyz;
			Output.texcoord	= ATTRIB_2.xy;
		#else
			gl_Position		= float4( (ATTRIB_0.xy * 2.0 - 1.0) * terrainUB.gridScale, 0.0, 1.0 );
			Output.normal	= real3( 0.0, 0.0, 1.0 );
			Output.texcoord	= ATTRIB_0.xy;
		#endif

		real4	pos			= terrainUB.mvp * real4( gl_Position.xyz +
							  texture( unHeightMap, Output.texcoord ).r *
							  Output.normal * terrainUB.heightScale, 1.0 );
		Output.scrCoords	= pos.xy / pos.w;
		Output.level		= float( Level( pos.z ) );
		Output.inScreen		= InScreen( Output.scrCoords );
	}

#endif


#if SHADER == SH_TESS_CONTROL

	#define I	gl_InvocationID

	layout(vertices = 4) out;

	in	TVertData	Input[];
	out TContData	Output[];

	#define Min4( _a_, _b_, _c_, _d_ )	min( min( _a_, _b_ ), min( _c_, _d_ ) )
	#define Max4( _a_, _b_, _c_, _d_ )	max( max( _a_, _b_ ), max( _c_, _d_ ) )

	real4 Rect (in real2 p0, in real2 p1, in real2 p2, in real2 p3)
	{
		return  real4(	Min4( p0.x, p1.x, p2.x, p3.x ),
						Min4( p0.y, p1.y, p2.y, p3.y ),
						Max4( p0.x, p1.x, p2.x, p3.x ),
						Max4( p0.y, p1.y, p2.y, p3.y ) );
	}

	bool QuadInScreen()
	{
		real	size = terrainUB.scrAspect * 1.2;
	
		real4	screen	= real4( -size, -size, size, size );
		real4	rect 	= Rect(  Input[0].scrCoords, Input[1].scrCoords,
								 Input[2].scrCoords, Input[3].scrCoords );
		return	( rect[0] < screen[2] and rect[2] > screen[0] and
				  rect[1] < screen[3] and rect[3] > screen[1] ) or
				( screen[2] < rect[0] and screen[0] > rect[2] and
				  screen[3] < rect[1] and screen[1] > rect[3] );
	}

	void main()
	{
		if ( I == 0 )
		{
			bool	in_screen = any( bool4( Input[0].inScreen, Input[1].inScreen,
											Input[2].inScreen, Input[3].inScreen ) );
			float	max_level = max( max( Input[0].level, Input[1].level ),
									 max( Input[2].level, Input[3].level ) );
			float	k = ( in_screen || QuadInScreen() ) ? 1.0 : 0.0;
		
			gl_TessLevelInner[0] = max_level * k;
			gl_TessLevelInner[1] = max_level * k;
			gl_TessLevelOuter[0] = max( Input[0].level, Input[3].level ) * k;
			gl_TessLevelOuter[1] = max( Input[0].level, Input[1].level ) * k;
			gl_TessLevelOuter[2] = max( Input[1].level, Input[2].level ) * k;
			gl_TessLevelOuter[3] = max( Input[2].level, Input[3].level ) * k;
		}
	
		gl_out[I].gl_Position	= gl_in[I].gl_Position;
		Output[I].normal		= Input[I].normal;
		Output[I].texcoord		= Input[I].texcoord;
		Output[I].level			= Input[I].level;
	}

#endif


#if SHADER == SH_TESS_EVALUATION

	layout(quads, equal_spacing, ccw) in;

	in	TContData	Input[];
	out	TEvalData	Output;

	#define Interpolate( _arr, _field ) \
		( mix(	mix( _arr[0] _field, _arr[1] _field, gl_TessCoord.x ), \
				mix( _arr[3] _field, _arr[2] _field, gl_TessCoord.x ), \
				gl_TessCoord.y ) )

	real PCF (in float2 vTexcoord)
	{
		/*
		real	height = 0.0;
		height += textureOffset( unHeightMap, vTexcoord, int2(-1,-1) ).r;
		height += textureOffset( unHeightMap, vTexcoord, int2(-1, 0) ).r;
		height += textureOffset( unHeightMap, vTexcoord, int2(-1, 1) ).r;
		height += textureOffset( unHeightMap, vTexcoord, int2( 0,-1) ).r;
		height += textureOffset( unHeightMap, vTexcoord, int2( 0, 0) ).r * 2.0;
		height += textureOffset( unHeightMap, vTexcoord, int2( 0, 1) ).r;
		height += textureOffset( unHeightMap, vTexcoord, int2( 1,-1) ).r;
		height += textureOffset( unHeightMap, vTexcoord, int2( 1, 0) ).r;
		height += textureOffset( unHeightMap, vTexcoord, int2( 1, 1) ).r;
		return ( height * 0.1 );
		/*/
		return texture( unHeightMap, vTexcoord ).r;
		//*/
	}

	void main()
	{
		real4	pos		= Interpolate( gl_in, .gl_Position );
		Output.texcoord	= Interpolate( Input, .texcoord );
		//Output.level	= Interpolate( Input, .level );
		real3	norm	= Interpolate( Input, .normal );
		Output.height	= PCF( Output.texcoord );
	
		pos.xyz += Output.height * norm * terrainUB.heightScale;
		gl_Position = float4( terrainUB.mvp * pos );
	}

#endif


#if SHADER == SH_FRAGMENT

	in	TEvalData	Input;

	void main()
	{
		real3	normal = texture( unNormalMap, Input.texcoord ).rgb;

		float	light = float( clamp( dot( normal, terrainUB.lightDir ), 0.2, 1.0 ) * 0.5 + 0.5 );

		outColor0 = texture( unColoredLine, float2( Input.height * 0.5 + 0.5, 0.5 ) );
		outColor0 *= 0.0001;
		outColor0 += 1.0;

		outColor0 *= light;
		
		//outColor0.rgb += normal * 0.5 + 0.5;
	}

#endif
