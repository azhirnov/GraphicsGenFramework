
[HEADER]

#if SHADER == SH_VERTEX

	float4 PositionTransform (in float4 pos);

#endif


[SOURCE]

float4 PositionTransform (in float4 pos)
{
	return pos + float4( 0.0001 );
}
