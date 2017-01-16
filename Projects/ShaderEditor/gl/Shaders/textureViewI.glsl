
// uniforms //
layout(std140)
uniform TextureViewUB {
	float2	scale;
	float2	bias;
	float2	valueScaleBias;
	int		viewMode;
} textureViewUB;


// shader input/output data types //
#define TVertData \
TVertData_t { \
	float2	texcoord; \
}


#if SHADER == SH_VERTEX

	out	TVertData	Output;

	void main()
	{
		gl_Position		= float4( ATTRIB_0.xy * textureViewUB.scale + textureViewUB.bias, 0.0, 1.0 );
		Output.texcoord	= ATTRIB_1.xy;
	}

#endif

#if SHADER == SH_FRAGMENT

	uniform isampler2D	unTexture;

	in	TVertData	Input;

	void main()
	{
		float4	tex		= float4(texture( unTexture, Input.texcoord )) / textureViewUB.valueScaleBias.x + textureViewUB.valueScaleBias.y;
		float4	color	= float4(0.0);

		switch ( textureViewUB.viewMode )
		{
			case 0 :	// Color
				color = tex;
				break;

			case 1 :	// LinearDepth
				color = float4( tex.r );	// TODO
				break;

			case 2 :	// ExpDepth
				color = float4( pow( tex.r, 10.0 ) );
				break;

			case 3 :	// Normals
				color = tex * 0.5 + 0.5;
				break;

			default :	// Unknown
				color = tex;
		}

		outColor0 = color;
	}

#endif
