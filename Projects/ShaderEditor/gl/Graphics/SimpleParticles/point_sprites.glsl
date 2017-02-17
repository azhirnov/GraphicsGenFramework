
// uniforms //
layout(std140)
uniform ShaderUB {
	float4x4		proj;
	float4x4		mv;
	float4x4		mvp;
	float2			screenSize;
} UB;


// shader input/output data types //
#define TVertData \
TVertData_t { \
	float4	color; \
}
	
#import "Core/Math/Math.glcs"


#if SHADER == SH_VERTEX

	out	TVertData	Output;

	void main ()
	{
		gl_Position		= UB.mvp * float4( ATTRIB_0.xyz, 1.0 );
		Output.color	= ATTRIB_2.rgba;

		float	size	= Clamp( - ATTRIB_3.x / (UB.mv * float4( ATTRIB_0.xyz, 1.0 )).z, 0.0, ATTRIB_3.x*4.0 );
		//float	size	= ATTRIB_3.x;

		gl_PointSize	= size;
	}

#endif	// SH_VERTEX
//-----------------------------------------------------------------------------


#if SHADER == SH_FRAGMENT

	in	TVertData	Input;

	void main()
	{
		outColor0 = Input.color; // * (1.0 - Distance( ToSNorm(gl_PointCoord), float2(0.0) ));
	}

#endif	// SH_FRAGMENT
