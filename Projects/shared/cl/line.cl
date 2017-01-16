/*
	2D and 3D Line classes
*/

//------------------------------------------------------------------------------//
//									HEADER										//

#include "rectangle.cl"


typedef struct _Line2
{
	real2	begin;
	real2	end;

} Line2;

void  Line2_Create (OUT Line2 *line, real2 begin, real2 end);
void  Line2_GetBoundingRect (const Line2 *line, OUT RectF *rect);
bool  Line2_ContainsPoint (const Line2 *line, real2 point);
bool  Line2_RayIntersection (const Line2 *a, const Line2 *b, OUT real2 *crossPoint);
bool  Line2_LineIntersection (const Line2 *a, const Line2 *b, OUT real2 *crossPoint);
bool  Line2_RayRectIntersection (const Line2 *line, const RectF *rect, OUT Line2 *result);
bool  Line2_RectIntersection (const Line2 *line, const RectF *rect, OUT Line2 *result);
void  Line2_GetEquation (const Line2 *line, OUT real3 *abc);
void  Line2_GetEquationY (const Line2 *line, OUT real2 *kb);
real2 Line2_Direction (const Line2 *line);
real  Line2_Length (const Line2 *line);
void  Line2_RayPerpendicular (const Line2 *line, real2 point, OUT Line2 *result);
bool  Line2_Perpendicular (const Line2 *line, real2 point, OUT Line2 *result);
real  Line2_MinDistance (const Line2 *line, real2 point);


typedef struct _Line3
{
	real3	begin;
	real3	end;

} Line3;

void  Line3_Create (OUT Line3 *line, real3 begin, real3 end);
real3 Line3_Direction (const Line3 *line);
void  Line3_ToLine2 (const Line3 *line, OUT Line2 *result);
real  Line3_MinDistance (const Line3 *line, real3 point);



//------------------------------------------------------------------------------//
//									SOURCE										//


/*
=================================================
	Line2_Create
=================================================
*/
void Line2_Create (OUT Line2 *line, real2 begin, real2 end)
{
	line->begin	= begin;
	line->end	= end;
}

/*
=================================================
	Line2_GetBoundingRect
=================================================
*/
void Line2_GetBoundingRect (const Line2 *line, OUT RectF *rect)
{
	*rect = RectF_FromPoints( line->begin, line->end );
}

/*
=================================================
	Line2_GetEquation
=================================================
*/
void Line2_GetEquation (const Line2 *line, OUT real3 *abc)
{
	// Ax + By + C = 0
	abc->x = line->begin.y - line->end.y;
	abc->y = line->end.x - line->begin.x;
	abc->z = Cross2( line->begin, line->end );
}

/*
=================================================
	Line2_GetEquationY
=================================================
*/
void Line2_GetEquationY (const Line2 *line, OUT real2 *kb)
{
	real3	abc;
	Line2_GetEquation( line, OUT &abc );

	*kb = IsZero( abc.s1 ) ? (real2)(0.0) : -abc.s02 / abc.s1;
}

/*
=================================================
	Line2_ContainsPoint
=================================================
*/
bool Line2_ContainsPoint (const Line2 *line, real2 point)
{
	RectF	rect;
	Line2_GetBoundingRect( line, OUT &rect );

	if ( not RectF_ContainsPoint( rect, point ) )
		return false;

	real3	abc;
	Line2_GetEquation( line, OUT &abc );
	
	return IsZero( dot( point, abc.s01 ) + abc.s2 );
}

/*
=================================================
	Line2_RayIntersection
=================================================
*/
bool Line2_RayIntersection (const Line2 *a, const Line2 *b, OUT real2 *crossPoint)
{
	real2	av = a->begin - a->end;
	real2	bv = b->begin - b->end;

	real	c  = Cross2( av, bv );

	if ( IsZero( c ) )
		return false;

	real	ca = Cross2( av, av );
	real	cb = Cross2( bv, bv );

	*crossPoint = (real2)( ca * bv.x - cb * av.x,
						   ca * bv.y - cb * av.y ) / c;
	return true;
}

/*
=================================================
	Line2_LineIntersection
=================================================
*/
bool Line2_LineIntersection (const Line2 *a, const Line2 *b, OUT real2 *crossPoint)
{
	if ( not Line2_RayIntersection( a, b, OUT crossPoint ) )
		return false;

	RectF	rect_a;
	RectF	rect_b;

	Line2_GetBoundingRect( a, OUT &rect_a );
	Line2_GetBoundingRect( b, OUT &rect_b );

	return RectF_ContainsPoint( rect_a, *crossPoint ) and RectF_ContainsPoint( rect_b, *crossPoint );
}

/*
=================================================
	Line2_RayRectIntersection
----
	returns line that contains intersection points
=================================================
*/
bool Line2_RayRectIntersection (const Line2 *line, const RectF *rect, OUT Line2 *result)
{
	RectF	line_brect;
	Line2_GetBoundingRect( line, OUT &line_brect );

	if ( not RectF_RectIntersect( *rect, line_brect ) )
		return false;

	real2	cross_points[4] = {};
	int		i = 0;

	Line2	line_b;
	real2	tmp_point;

	// left
	Line2_Create( OUT &line_b, (real2)( rect->min.x, rect->min.y ), (real2)( rect->min.x, rect->max.y ) );

	if ( Line2_RayIntersection( line, &line_b, OUT &tmp_point ) and RectF_ContainsPoint( *rect, tmp_point ) )
	{
		cross_points[i++] = tmp_point;
	}
	
	// right
	Line2_Create( OUT &line_b, (real2)( rect->max.x, rect->min.y ), (real2)( rect->max.x, rect->max.y ) );

	if ( Line2_RayIntersection( line, &line_b, OUT &tmp_point ) and RectF_ContainsPoint( *rect, tmp_point ) )
	{
		cross_points[i++] = tmp_point;
	}
	
	// bottom
	Line2_Create( OUT &line_b, (real2)( rect->min.x, rect->min.y ), (real2)( rect->max.x, rect->min.y ) );

	if ( Line2_RayIntersection( line, &line_b, OUT &tmp_point ) and RectF_ContainsPoint( *rect, tmp_point ) )
	{
		cross_points[i++] = tmp_point;
	}
	
	// top
	Line2_Create( OUT &line_b, (real2)( rect->min.x, rect->max.y ), (real2)( rect->max.x, rect->max.y ) );

	if ( Line2_RayIntersection( line, &line_b, OUT &tmp_point ) and RectF_ContainsPoint( *rect, tmp_point ) )
	{
		cross_points[i++] = tmp_point;
	}
	
	Line2_Create( OUT result, cross_points[0], cross_points[0] );

	// may happens 4 intersections when ray match to rectangle diagonal
	for (; i > 1; --i)
	{
		if ( not Equals2( cross_points[i-1], result->end ) )
			break;
	}

	return i > 0;
}

/*
=================================================
	Line2_RectIntersection
----
	returns line that contains intersection points
=================================================
*/
bool Line2_RectIntersection (const Line2 *line, const RectF *rect, OUT Line2 *result)
{
	if ( not Line2_RayRectIntersection( line, rect, OUT result ) )
		return false;

	int		count = 2;

	RectF	line_brect;
	Line2_GetBoundingRect( line, OUT &line_brect );

	// remove points if they outside of the source line
	if ( not RectF_ContainsPoint( line_brect, result->begin ) )
	{
		result->end = result->begin;
		--count;
	}
	
	if ( not RectF_ContainsPoint( line_brect, result->end ) )
	{
		result->begin = result->end;
		--count;
	}

	return count > 0;
}

/*
=================================================
	Line2_Direction
=================================================
*/
real2 Line2_Direction (const Line2 *line)
{
	return normalize( line->end - line->begin );
}

/*
=================================================
	Line2_Length
=================================================
*/
real Line2_Length (const Line2 *line)
{
	return distance( line->begin, line->end );
}

/*
=================================================
	Line2_RayPerpendicular
=================================================
*/
void Line2_RayPerpendicular (const Line2 *line, real2 point, OUT Line2 *result)
{
	real2	vec		= line->end - line->begin;
	real2	vec_sq	= vec * vec;
	real2	base;

	base.x = ( line->begin.x * vec_sq.y + point.x * vec_sq.x + vec.x * vec.y * (point.y - line->begin.y) ) / (vec_sq.x + vec_sq.y);
	base.y = vec.x * (point.x - base.x) / vec.y + point.y;

	result->begin = point;
	result->end   = base;
}

/*
=================================================
	Line2_Perpendicular
=================================================
*/
bool Line2_Perpendicular (const Line2 *line, real2 point, OUT Line2 *result)
{
	Line2_RayPerpendicular( line, point, OUT result );

	return Line2_ContainsPoint( line, result->end );
}

/*
=================================================
	Line2_Perpendicular
=================================================
*/
real  Line2_MinDistance (const Line2 *line, real2 point)
{
	real3	abc;
	Line2_GetEquation( line, &abc );

	real	d = ( abc.s0 * point.x + abc.s1 * point.y + abc.s2 ) / sqrt( dot( abc.s01, abc.s01 ) );
	
	return min( d, min( distance( point, line->begin ), distance( point, line->end ) ) );
}



/*
=================================================
	Line3_Create
=================================================
*/
void Line3_Create (OUT Line3 *line, real3 begin, real3 end)
{
	line->begin	= begin;
	line->end	= end;
}

/*
=================================================
	Line3_Direction
=================================================
*/
real3 Line3_Direction (const Line3 *line)
{
	return normalize( line->end - line->begin );
}

/*
=================================================
	Line3_ToLine2
=================================================
*/
void  Line3_ToLine2 (const Line3 *line, OUT Line2 *result)
{
	Line2_Create( result, line->begin.xy, line->end.xy );
}

/*
=================================================
	Line3_MinDistance
=================================================
*/
real  Line3_MinDistance (const Line3 *line, real3 point)
{
	real3	v = line->end - line->begin;
	real3	p = point - line->begin;

	real	a = Cross2( v.zy, p.zy );
	real	b = Cross2( v.xz, p.xz );
	real	c = Cross2( v.yx, p.yx );

	real	d = sqrt( (a*a + b*b + c*c) / dot( v, v ) );

	return min( d, min( distance( point, line->begin ), distance( point, line->end ) ) );
}
