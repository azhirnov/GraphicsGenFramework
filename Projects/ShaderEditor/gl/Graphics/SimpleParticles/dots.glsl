
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
		gl_Position		= UB.mv * float4( ATTRIB_0.xyz, 1.0 );
		Output.color	= ATTRIB_2.rgba;
		Output.size		= ATTRIB_3 * 4.0 / Max( UB.screenSize.x, UB.screenSize.y );
	}

#endif	// SH_VERTEX
//-----------------------------------------------------------------------------


#if SHADER == SH_GEOMETRY

	layout (points) in;
	layout (triangle_strip, max_vertices = 4) out;
	
	in	TVertData	Input[];
	out	TGeomData	Output;

	void main ()
	{
		// Billboard
		// from http://www.geeks3d.com/20140815/particle-billboarding-with-the-geometry-shader-glsl/

		float4	pos		= gl_in[0].gl_Position;
		float	size	= Input[0].size;


		// a: left-bottom
		float2	va	= pos.xy + float2(-0.5, -0.5) * size;
		gl_Position	= UB.proj * float4(va, pos.zw);
		Output.uv	= float2(0.0, 0.0);
		Output.color= Input[0].color;
		EmitVertex();

		// b: left-top
		float2	vb	= pos.xy + float2(-0.5, 0.5) * size;
		gl_Position	= UB.proj * float4(vb, pos.zw);
		Output.uv	= float2(0.0, 1.0);
		Output.color= Input[0].color;
		EmitVertex();

		// d: right-bottom
		float2	vd	= pos.xy + float2(0.5, -0.5) * size;
		gl_Position	= UB.proj * float4(vd, pos.zw);
		Output.uv	= float2(1.0, 0.0);
		Output.color= Input[0].color;
		EmitVertex();

		// c: right-top
		float2	vc	= pos.xy + float2(0.5, 0.5) * size;
		gl_Position	= UB.proj * float4(vc, pos.zw);
		Output.uv	= float2(1.0, 1.0);
		Output.color= Input[0].color;
		EmitVertex();

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
