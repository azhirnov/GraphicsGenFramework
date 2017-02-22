
// uniforms //
layout(std140)
uniform LandscapeUB {
	float4x4	mvp;
	float4x4	model;
	//float3x3	normalMat;
	float3		lightDir;
	float		maxTessLevel;
	float		heightScale;
	float		detailLevel;
	float		scrAspect;
} UB;

uniform sampler2D		unDiffuseMap;			// RGBA color
uniform sampler2D		unDisplacementMap;		// R - height, GBA - normal


// shader input/output data types //
#define TVertData \
TVertData_t { \
	float3	normal; \
	float	level; \
	float2	scrCoords; \
	float2	texcoord; \
	bool	inScreen; \
}

#define TContData \
TContData_t { \
	float3	normal; \
	float	level; \
	float2	texcoord; \
}

#define TEvalData \
TEvalData_t { \
	float3	normal; \
	float	level; \
	float2	texcoord; \
	float	height; \
}

#import "Core/Common/Cmp.glcs"
#import "Core/Math/Math.glcs"
#import "Core/Math/Color.glcs"


#if SHADER == SH_VERTEX

	out	TVertData	Output;

	bool InScreen (const float2 pos)
	{
		float	size = UB.scrAspect * 0.5;
		return ( Abs(pos.x) <= size and Abs(pos.y) <= size );
	}

	float Level (const float dist)
	{
		float d = UB.detailLevel / Abs(dist) - 2.0;
		return Clamp( d, 1.0, UB.maxTessLevel );
	}

	void main ()
	{
		//float3x3 normMat	= transpose(inverse(float3x3(UB.model)));
		float3x3 normMat	= float3x3(UB.model);

		gl_Position			= float4( ATTRIB_0.xyz, 1.0 );
		//Output.normal		= ATTRIB_1.xzy;
		Output.normal		= Normalize( (UB.model * float4( ATTRIB_1.xyz, 0.0 )).xyz );
		//Output.normal		= Normalize( float3( 0.0, 1.0, 0.0 ) );   //Normalize( normMat * ATTRIB_1.xyz );
		Output.texcoord		= ATTRIB_2.xy;

		float4	pos			= UB.mvp * gl_Position;
		Output.scrCoords	= pos.xy / pos.w;
		Output.level		= float( Level( pos.z ) );
		Output.inScreen		= InScreen( Output.scrCoords );
	}

#endif


#if SHADER == SH_TESS_CONTROL

	#define I	gl_InvocationID

	layout(vertices = 3) out;

	in	TVertData	Input[];
	out TContData	Output[];

	#define Min4( _a_, _b_, _c_, _d_ )	Min( Min( _a_, _b_ ), Min( _c_, _d_ ) )
	#define Max4( _a_, _b_, _c_, _d_ )	Max( Max( _a_, _b_ ), Max( _c_, _d_ ) )

	float4 Rect (const float2 p0, const float2 p1, const float2 p2, const float2 p3)
	{
		return  float4(	Min4( p0.x, p1.x, p2.x, p3.x ),
						Min4( p0.y, p1.y, p2.y, p3.y ),
						Max4( p0.x, p1.x, p2.x, p3.x ),
						Max4( p0.y, p1.y, p2.y, p3.y ) );
	}

	bool QuadInScreen()
	{
		float	size = UB.scrAspect;
	
		float4	screen	= float4( -size, -size, size, size );
		float4	rect 	= Rect(  Input[0].scrCoords, Input[1].scrCoords,
								 Input[2].scrCoords, Input[3].scrCoords );
		return	( rect[0] < screen[2] and rect[2] > screen[0]   and
				  rect[1] < screen[3] and rect[3] > screen[1] ) or
				( screen[2] < rect[0] and screen[0] > rect[2]   and
				  screen[3] < rect[1] and screen[1] > rect[3] );
	}

	void main()
	{
		if ( I == 0 )
		{
			/*bool	in_screen = Any( bool4( Input[0].inScreen, Input[1].inScreen,
											Input[2].inScreen, Input[3].inScreen ) );*/
			float	max_level = Max( Max( Input[0].level, Input[1].level ),
									 Max( Input[2].level, Input[3].level ) );
			float	k = /*( in_screen or QuadInScreen() ) ?*/ 1.0 /*: 0.0*/;
		
			gl_TessLevelInner[0] = max_level * k;
			gl_TessLevelOuter[0] = Max( Input[0].level, Input[3].level ) * k;
			gl_TessLevelOuter[1] = Max( Input[0].level, Input[1].level ) * k;
			gl_TessLevelOuter[2] = Max( Input[1].level, Input[2].level ) * k;
		}
	
		gl_out[I].gl_Position	= gl_in[I].gl_Position;
		Output[I].normal		= Input[I].normal;
		Output[I].texcoord		= Input[I].texcoord;
		Output[I].level			= Input[I].level;
	}

#endif


#if SHADER == SH_TESS_EVALUATION

	layout(triangles, equal_spacing, ccw) in;

	in	TContData	Input[];
	out	TEvalData	Output;

	#define Interpolate( _arr, _field ) \
		( gl_TessCoord.x * _arr[0] _field + \
		  gl_TessCoord.y * _arr[1] _field + \
		  gl_TessCoord.z * _arr[2] _field )

	void main()
	{
		float4	pos		= Interpolate( gl_in, .gl_Position );
		float3	norm	= Interpolate( Input, .normal );
		//float3 norm	= Normalize( Input[0].normal + Input[1].normal + Input[2].normal );

		Output.texcoord	= Interpolate( Input, .texcoord );
		float4	hn		= texture( unDisplacementMap, Output.texcoord );

		Output.level	= Interpolate( Input, .level ) / UB.maxTessLevel;
		Output.normal	= norm; //hn.gba;

		bool	in_border	= AnyLess( gl_TessCoord, float3(0.001) );

		//if ( not in_border )
			pos.xyz	+= hn.r * norm * UB.heightScale;

		gl_Position  = float4( UB.mvp * pos );
	}

#endif


#if SHADER == SH_FRAGMENT

	in	TEvalData	Input;

	void main()
	{
		const float	ambient = 0.2;

		float	light = Clamp( dot( Input.normal, UB.lightDir ), ambient, 1.0 );

		//outColor0	= float4( HSVtoRGB(float3( Input.level / UB.maxTessLevel, 1.0, 1.0 )), 1.0 );
		//outColor0   = float4( -( texture( unDisplacementMap, Input.texcoord ).gba ), 1.0 );
		//outColor0	= float4( ToUNorm( Input.normal ), 1.0 );
		outColor0 = texture( unDiffuseMap, Input.texcoord );
		outColor0	*= light;

		//outColor0.rgb	= HSVtoRGB(float3( Input.level, 1.0, 1.0 ));
		//outColor0.a		= texture( unDiffuseMap, Input.texcoord ).a;
	}

#endif
