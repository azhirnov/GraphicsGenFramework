
#import "prog1.glsl"

// shader input/output data types //
#define TVertData \
TVertData_t { \
	float4	position; \
	float2	texcoord; \
}


#if SHADER == SH_VERTEX

	#include "prog2.glsl"
	
	out	TVertData	Output;

	void main ()
	{
		gl_Position = PositionTransform( float4( ATTRIB_0, 0.0, 1.0 ) );

		Output.position = RandomizeAttrib( gl_Position );
		Output.texcoord = RandomizeAttrib( ATTRIB_1 );
	}

#endif

#if SHADER == SH_FRAGMENT

	#import "prog3.glsl"
	#import "prog1.glsl"
	#include "prog2.glsl"
	
	in	TVertData	Input;

	void main ()
	{
		float4	pos = float4( 0.0 );
		//float4	pos = PositionTransform( Input.position );	// error

		outColor0 = GenerateImage( pos, Input.texcoord );
	}

#endif
