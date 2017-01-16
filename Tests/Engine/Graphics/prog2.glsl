
float RandomizeAttrib (float x)
{
	return fract( x * 824529.0 );
}

float2 RandomizeAttrib (float2 x)
{
	return fract( x * 3674524.0 );
}

float3 RandomizeAttrib (float3 x)
{
	return fract( x * 73245.0 );
}

float4 RandomizeAttrib (float4 x)
{
	return fract( x * 654236.0 );
}
