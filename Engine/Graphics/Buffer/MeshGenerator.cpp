// Copyright © 2014-2017  Zhirnov Andrey. All rights reserved.

#include "MeshGenerator.h"

namespace Engine
{
namespace Graphics
{
	
/*
=================================================
	constructor
=================================================
*/
	MeshGenerator::GridVertex::GridVertex (const float3 &position, const float3 &normal, const float2 &texcoord)
	{
		this->position	= position;
		this->normal	= TypeCast::SNormToInt<short>( normal );
		this->texcoord	= TypeCast::UNormToInt<ushort>( texcoord );
	}
	
/*
=================================================
	GetAttribs
=================================================
*/
	void MeshGenerator::GridVertex::GetAttribs (OUT VertexAttribsDesc &desc)
	{
		desc.Add( VertexAttribsDesc::AttribIndex(0), EAttribute::Float3,		OffsetOf( &GridVertex::position ) );
		desc.Add( VertexAttribsDesc::AttribIndex(1), EAttribute::Short3_Norm,	OffsetOf( &GridVertex::normal ) );
		desc.Add( VertexAttribsDesc::AttribIndex(2), EAttribute::UShort2_Norm,	OffsetOf( &GridVertex::texcoord ) );
	}

/*
=================================================
	CreateQuad
=================================================
*/
	MeshGenerator::Mesh  MeshGenerator::CreateQuad (const SubSystemsRef ss, const RectF &position, const RectF &texcoord)
	{
		const float	quad_verts[] = {
			position.left,  position.top,		texcoord.left,  texcoord.top,
			position.left,  position.bottom,	texcoord.left,  texcoord.bottom,
			position.right, position.top,		texcoord.right, texcoord.top,
			position.right, position.bottom,	texcoord.right, texcoord.bottom
		};

		VertexAttribsDesc	desc;
		desc.Add( VertexAttribsDesc::AttribIndex(0), EAttribute::Float2, BytesU(0) );
		desc.Add( VertexAttribsDesc::AttribIndex(1), EAttribute::Float2, SizeOf<float2>() );
		
		Mesh	mesh( New<VertexBuffer>( ss ) );

		CHECK( mesh.vertexBuffer->Create() );
		CHECK( mesh.vertexBuffer->SetData( BinaryCBuffer::From( quad_verts ), EBufferUsage::Static ) );

		mesh.vertexBuffer->SetAttribs( desc, BytesU::SizeOf<float4>() );
		mesh.vertexBuffer->SetPrimitive( EPrimitive::TriangleStrip );

		return mesh;
	}
	
/*
=================================================
	CreateCube
=================================================
*/
	MeshGenerator::Mesh  MeshGenerator::CreateCube (const SubSystemsRef ss, const float3 &scale, const float3 &bias)
	{
		const float3	s( 1.0f );
		
		/*
			   /4--------/2			0 - max
			  / |       / |			5 - min
			 /  |      /  |
			6---------0   |			first strip:  0,1,2,3,4,5,6,7
			|  /5- - -|- -3			second strip: 4,2,6,0,7,1,5,3
			| /       |  /
			|/        | /
			7---------1/
		*/

		float3	cube_verts[] = {
			float3( +s.x, +s.y, +s.z ),	// 0
			float3( +s.x, +s.y, -s.z ),	// 1
			float3( +s.x, -s.y, +s.z ),	// 2
			float3( +s.x, -s.y, -s.z ),	// 3
			float3( -s.x, -s.y, +s.z ),	// 4
			float3( -s.x, -s.y, -s.z ),	// 5
			float3( -s.x, +s.y, +s.z ),	// 6
			float3( -s.x, +s.y, -s.z ),	// 7
		};

		const ushort	indices[] = {
			0,1,2,		2,1,3,		2,3,4,		4,3,5,		4,5,6,		6,5,7,
			4,6,2,		2,6,0,		6,7,0,		0,7,1,		7,5,1,		1,5,3
		};

		for (usize i = 0; i < CountOf(cube_verts); ++i)
		{
			cube_verts[i] = cube_verts[i] * scale + bias;
		}

		VertexAttribsDesc	desc;
		desc.Add( VertexAttribsDesc::AttribIndex(0), EAttribute::Float3, BytesU(0) );
		
		Mesh	mesh( New<VertexBuffer>( ss ), New<IndexBuffer>( ss ) );

		CHECK( mesh.vertexBuffer->Create() );
		CHECK( mesh.vertexBuffer->SetData( BinaryCBuffer::From( cube_verts ), EBufferUsage::Static ) );

		mesh.vertexBuffer->SetAttribs( desc, BytesU::SizeOf<float3>() );
		mesh.vertexBuffer->SetPrimitive( EPrimitive::Triangle );

		CHECK( mesh.indexBuffer->Create() );
		CHECK( mesh.indexBuffer->SetData( BinaryCBuffer::From( indices ), EBufferUsage::Static ) );

		mesh.indexBuffer->SetIndexType( EIndex::UShort );

		return mesh;
	}

/*
=================================================
	CreatePlane
=================================================
*/
	MeshGenerator::Mesh  MeshGenerator::CreatePlane (const SubSystemsRef ss, uint numVertices, const float3 &scale,
													 const float3 &bias, uint patchSize, bool addBorder)
	{
		CHECK_ERR( patchSize == 3 or patchSize == 4 or patchSize == 16 );
		CHECK_ERR( numVertices > 0 );

		Array<GridVertex>	vertices;
		Array<uint>			indices;
		uint				count		= numVertices + 1;
		uint				max_size	= numVertices;
		float const			uv_scale	= 1.0f / numVertices;
		float3 const		pos_scale	= scale * float3( uv_scale, uv_scale, 1.0f );

		if ( addBorder )
		{
			numVertices	+= 2;
			count		+= 2;
		}

		vertices.Reserve( count * count );
		indices.Reserve( numVertices * numVertices * 4 );

		for (uint h = 0; h < count; ++h)
		{
			for (uint w = 0; w < count; ++w)
			{
				float3	pos;

				if ( addBorder )
				{
					pos = Clamp( int2(w, h)-1, int2(0), int2(max_size) ).To<float2>().xyo();

					if ( w == 0 or h == 0 or w == numVertices or h == numVertices )
						pos.z = -1.0f;
				}
				else
					pos = int2(w, h).To<float2>().xyo();

				vertices.PushBack( GridVertex(
					pos * pos_scale + bias,
					float3( 0.0f, 0.0f, 1.0f ),
					pos.xy() * uv_scale
				) );

				if ( w != 0 and h != 0 )
				{
					if ( patchSize == 4 )
					{
						indices.PushBack( h*count		+ w );
						indices.PushBack( h*count		+ w-1 );
						indices.PushBack( (h-1)*count	+ w-1 );
						indices.PushBack( (h-1)*count	+ w );
					}
					else
					if ( patchSize == 3 )
					{
						indices.PushBack( h*count		+ w );
						indices.PushBack( h*count		+ w-1 );
						indices.PushBack( (h-1)*count	+ w-1 );

						indices.PushBack( (h-1)*count	+ w-1 );
						indices.PushBack( (h-1)*count	+ w );
						indices.PushBack( h*count		+ w );
					}
					else
					if ( patchSize == 16 and w % 3 == 0 and h % 3 == 0  )
					{
						for (uint i = 0; i < 4; ++i) {
							for (uint j = 0; j < 4; ++j) {
								indices.PushBack( (h+i-3)*count + w+j-3 );
							}
						}
					}
				}
			}
		}

		VertexAttribsDesc	attr;
		GridVertex::GetAttribs( attr );
		
		Mesh	mesh( New<VertexBuffer>( ss ), New<IndexBuffer>( ss ) );

		CHECK( mesh.vertexBuffer->Create() );
		CHECK( mesh.vertexBuffer->SetData( BinaryCBuffer::From( vertices ), EBufferUsage::Static ) );

		mesh.vertexBuffer->SetAttribs( attr, BytesU::SizeOf<GridVertex>() );
		mesh.vertexBuffer->SetPrimitive( EPrimitive::Triangle );

		CHECK( mesh.indexBuffer->Create() );
		CHECK( mesh.indexBuffer->SetData( BinaryCBuffer::From( indices ), EBufferUsage::Static ) );

		mesh.indexBuffer->SetIndexType( EIndex::UInt );

		return mesh;
	}
	
/*
=================================================
	CreateSphere
=================================================
*/
	MeshGenerator::Mesh  MeshGenerator::CreateSphere (const SubSystemsRef ss, uint numVertices, float radius, uint patchSize)
	{
		CHECK_ERR( patchSize == 3 or patchSize == 4 );
		CHECK_ERR( numVertices > 0 );
		
		Array<GridVertex>	vertices;
		Array<uint>			indices;

		uint const			count = numVertices + 1;
		
		vertices.Reserve( numVertices * numVertices );
		indices.Reserve( numVertices * numVertices * 4 );

		for (uint z = 0; z < count; ++z)
		{
			for (uint i = 0; i < count; ++i)
			{
				float2	t = SinCos( Radians<float>::Pi() * float(z) / float(numVertices) );
				float2	p = SinCos( Radians<float>::Pi() * 2.0f * float(i) / float(numVertices) ).yx() * t.x;
				float3	n = float3( p, t.y ).Normalized();
				
				vertices.PushBack( GridVertex(
					n * radius,
					n,
					uint2( i, z ).To<float2>() / float(numVertices)
				) );

				if ( z == 0 or i == 0 )
					continue;

				const uint	i1 = i - 1;
				const uint	z1 = z - 1;
				
				if ( patchSize == 4 )
				{
					indices.PushBack( i  + z  * count );
					indices.PushBack( i1 + z  * count );
					indices.PushBack( i1 + z1 * count );
					indices.PushBack( i  + z1 * count );
				}
				else
				if ( patchSize == 3 )
				{
					indices.PushBack( i  + z  * count );
					indices.PushBack( i1 + z  * count );
					indices.PushBack( i1 + z1 * count );
						
					indices.PushBack( i1 + z1 * count );
					indices.PushBack( i  + z1 * count );
					indices.PushBack( i  + z  * count );
				}
			}
		}
		
		VertexAttribsDesc	attr;
		GridVertex::GetAttribs( attr );
		
		Mesh	mesh( New<VertexBuffer>( ss ), New<IndexBuffer>( ss ) );

		CHECK( mesh.vertexBuffer->Create() );
		CHECK( mesh.vertexBuffer->SetData( BinaryCBuffer::From( vertices ), EBufferUsage::Static ) );

		mesh.vertexBuffer->SetAttribs( attr, BytesU::SizeOf<GridVertex>() );
		mesh.vertexBuffer->SetPrimitive( EPrimitive::Triangle );

		CHECK( mesh.indexBuffer->Create() );
		CHECK( mesh.indexBuffer->SetData( BinaryCBuffer::From( indices ), EBufferUsage::Static ) );

		mesh.indexBuffer->SetIndexType( EIndex::UInt );

		return mesh;
	}
	
/*
=================================================
	CreateCylinder
=================================================
*/
	MeshGenerator::Mesh  MeshGenerator::CreateCylinder (const SubSystemsRef ss, uint numVertices, float radius, float length, uint patchSize)
	{
		CHECK_ERR( patchSize == 3 or patchSize == 4 );
		CHECK_ERR( numVertices > 0 );
		
		Array<GridVertex>	vertices;
		Array<uint>			indices;

		float const			circle_length	= Pi<float> * 2.0f * radius;
		uint const			circle_vertices	= RoundToUInt( float(numVertices) * circle_length / length );

		vertices.Reserve( numVertices * circle_vertices );
		indices.Reserve( numVertices * circle_vertices * 4 );

		for (uint z = 0; z < numVertices; ++z)
		{
			// circle
			for (uint i = 0; i < circle_vertices; ++i)
			{
				float2	p = SinCos( Radians<float>::Pi() * 2.0f * float(i) / float(circle_vertices) ).yx();

				vertices.PushBack( GridVertex(
					float3( radius * p.x, radius * p.y, length * float(z) / numVertices ),
					float3( p.x, p.y, 0.0f ),
					float2( float(i) / circle_vertices, float(z) / numVertices )
				) );

				if ( z == 0 or i == 0 )
					continue;
				
				if ( patchSize == 4 )
				{
					indices.PushBack( i   + z     * circle_vertices );
					indices.PushBack( i-1 + z     * circle_vertices );
					indices.PushBack( i-1 + (z-1) * circle_vertices );
					indices.PushBack( i   + (z-1) * circle_vertices );
				}
				else
				if ( patchSize == 3 )
				{
					indices.PushBack( i   + z     * circle_vertices );
					indices.PushBack( i-1 + z     * circle_vertices );
					indices.PushBack( i-1 + (z-1) * circle_vertices );
						
					indices.PushBack( i-1 + (z-1) * circle_vertices );
					indices.PushBack( i   + (z-1) * circle_vertices );
					indices.PushBack( i   + z     * circle_vertices );
				}
			}
		}

		VertexAttribsDesc	attr;
		GridVertex::GetAttribs( attr );
		
		Mesh	mesh( New<VertexBuffer>( ss ), New<IndexBuffer>( ss ) );

		CHECK( mesh.vertexBuffer->Create() );
		CHECK( mesh.vertexBuffer->SetData( BinaryCBuffer::From( vertices ), EBufferUsage::Static ) );

		mesh.vertexBuffer->SetAttribs( attr, BytesU::SizeOf<GridVertex>() );
		mesh.vertexBuffer->SetPrimitive( EPrimitive::Triangle );

		CHECK( mesh.indexBuffer->Create() );
		CHECK( mesh.indexBuffer->SetData( BinaryCBuffer::From( indices ), EBufferUsage::Static ) );

		mesh.indexBuffer->SetIndexType( EIndex::UInt );

		return mesh;
	}


}	// Graphics
}	// Engine
