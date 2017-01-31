
// uniforms //
layout(std140)
uniform ShaderUB {
	float4x4		mvp;
	float3		lightDir;
} UB;

uniform sampler2D	unDiffuseTexture;


// shader input/output data types //
#define TVertData \
TVertData_t { \
	float3	normal; \
	float2	texcoord; \
}


#if SHADER == SH_VERTEX

	out	TVertData	Output;

	void main ()
	{
		gl_Position		= UB.mvp * float4( ATTRIB_0.xyz, 1.0 );
		Output.normal	= ATTRIB_1.xyz;
		Output.texcoord	= ATTRIB_2.xy;
	}

#endif


#if SHADER == SH_FRAGMENT

	in	TVertData	Input;

	void main()
	{
		const float	ambient = 0.2;

		float	light = clamp( dot( Input.normal, UB.lightDir ), ambient, 1.0 );

		outColor0 = texture( unDiffuseTexture, Input.texcoord );
		outColor0 *= light;
	}

#endif
