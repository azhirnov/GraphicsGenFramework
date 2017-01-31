// Copyright © 2014-2017  Zhirnov Andrey. All rights reserved.

#pragma once

#include "MemoryBuffer.h"

namespace Engine
{
namespace Graphics
{

	//
	// Mesh Generator
	//

	class MeshGenerator
	{
	// types
	public:
		struct GridVertex
		{
		// variables
			float3	position;
			short3	normal;
			ushort2	texcoord;

		// methods
			GridVertex () {}

			GridVertex (const float3 &position, const float3 &normal, const float2 &texcoord);
			
			static void GetAttribs (OUT VertexAttribsDesc &desc);
		};

		struct Mesh
		{
		// variables
			VertexBufferPtr		vertexBuffer;
			IndexBufferPtr		indexBuffer;

		// methods
			Mesh (GX_DEFCTOR)
			{}

			explicit
			Mesh (const VertexBufferPtr &vb, const IndexBufferPtr &ib = null) :
				vertexBuffer(vb), indexBuffer(ib)
			{}

			bool IsCreated () const
			{
				return vertexBuffer.IsNotNull();
			}

			void Draw ()
			{
				if ( indexBuffer )
					vertexBuffer->DrawIndexed( indexBuffer );
				else
					vertexBuffer->Draw();
			}

			VertexAttribsPtr GetVertexAttribs () const
			{
				return vertexBuffer ? vertexBuffer->Attribs() : null;
			}

			VertexAttribsState const& GetAttribsState () const
			{
				return GetVertexAttribs()->GetAttribsState();
			}
		};


	// methods
	public:

		static Mesh  CreateQuad (const SubSystemsRef ss, const RectF &position, const RectF &texcoord);
		
		// [-1;+1]
		static Mesh  CreateCube (const SubSystemsRef ss, const float3 &scale, const float3 &bias);

		// for tessellation //
		// [0;+1]
		static Mesh  CreatePlane (const SubSystemsRef ss, uint vertices, const float3 &scale,
								  const float3 &bias, uint patchSize, bool addBorder);
		
		// [-1;+1]
		static Mesh  CreateSphere (const SubSystemsRef ss, uint vertices, float radius, uint patchSize);
		
		// [-1;+1]
		static Mesh  CreateCylinder (const SubSystemsRef ss, uint vertices, float radius, float length, uint patchSize);
	};


}	// Graphics
}	// Engine
