/*
	Plane classes
*/

//------------------------------------------------------------------------------//
//									HEADER										//

#include "math.cl"
#include "line.cl"


typedef struct _Plane
{
	real3	normal;
	real3	point;

} Plane;


void Plane_Create (OUT Plane *result, real3 p0, real3 p1, real3 p2);
void Plane_Create2D (OUT Plane *result, real z);
bool Plane_RayIntersect (const Plane *plane, const Line3 *line, OUT real3 *point);



//------------------------------------------------------------------------------//
//									SOURCE										//

/*
=================================================
	Plane_Create
=================================================
*/
void Plane_Create (OUT Plane *result, real3 p0, real3 p1, real3 p2)
{
	result->normal	= normalize( cross( p1 - p0, p2 - p0 ) );
	result->point	= p0;
}

/*
=================================================
	Plane_Create2D
=================================================
*/
void Plane_Create2D (OUT Plane *result, real z)
{
	result->normal	= (real3)( 0.0, 1.0, 0.0 );
	result->point	= (real3)( 0.0, z, 0.0 );
}

/*
=================================================
	Plane_RayIntersect
=================================================
*/
bool Plane_RayIntersect (const Plane *plane, const Line3 *line, OUT real3 *point)
{
	real3	u	= Line3_Direction( line );
	real3	n	= plane->normal;
	real3	w	= line->begin - plane->point;

	real	nu	= dot( n, u );
	real	nw	= dot( n, w );

	// is parallel?
	if ( IsZero( nu ) )
		return false;

	*point = (-nw / nu) * u + line->begin;
	return true;
}
