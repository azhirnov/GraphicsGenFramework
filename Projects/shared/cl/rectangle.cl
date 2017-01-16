/*
	Rectangle classes
*/

//------------------------------------------------------------------------------//
//									HEADER										//

#include "math.cl"


typedef struct _RectF
{
	real2	min;	// left, bottom
	real2	max;	// right, top

} RectF;


RectF RectF_Create (real left, real bottom, real right, real top);
RectF RectF_FromPoints (real2 point0, real2 point1);
bool RectF_ContainsPoint (const RectF rect, real2 point);
bool RectF_RectIntersect (const RectF a, const RectF b);



//------------------------------------------------------------------------------//
//									SOURCE										//


RectF RectF_Create (real left, real bottom, real right, real top)
{
	RectF	rect;
	rect.min = (real2)( min( left, right ), min( bottom, top ) );
	rect.max = (real2)( max( left, right ), max( bottom, top ) );
	return rect;
}


RectF RectF_FromPoints (real2 point0, real2 point1)
{
	return RectF_Create( point0.x, point0.y, point1.x, point1.y );
}


bool RectF_ContainsPoint (const RectF rect, real2 point)
{
	return all( point >= rect.min ) and all( point <= rect.max );
}


bool RectF_RectIntersect (const RectF a, const RectF b)
{
	return	!!( ( all( a.min < b.max ) and all( a.max > b.min ) ) or
				( all( b.max < a.min ) and all( b.min > a.max ) ) );
}
