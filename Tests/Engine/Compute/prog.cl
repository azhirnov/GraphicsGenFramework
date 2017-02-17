// 1
//# include "1.cl"
// 2
//# include "2.cl"
// 3
//# include "3.cl"
// 4
//# include "4.cl"
// Main
kernel void Main (write_only image2d_t outImage)
{
	double a = 4.81729392;
	double d = sin( a );

	write_imagef(outImage, (int2)(0,0), (float4)(convert_float(d)));
}
// end