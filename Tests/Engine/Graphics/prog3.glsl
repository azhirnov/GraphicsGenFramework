[HEADER]

float4 GenerateImage (in float4 rnd, in float2 texcoord);


[SOURCE]

float4 GenerateImage (in float4 rnd, in float2 texcoord)
{
	return float4( sin( texcoord.x ), cos( texcoord.y ), rnd.x + rnd.y, rnd.z + rnd.w );
}
