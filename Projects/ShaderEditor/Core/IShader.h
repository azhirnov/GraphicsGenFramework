// Copyright © 2014-2017  Zhirnov Andrey. All rights reserved.
/*
	Wrapper for graphics shader.
*/

#pragma once

#include "Common.h"

namespace ShaderEditor
{

	class IShader;
	SHARED_POINTER( IShader );



	//
	// Shader interface
	//

	class IShader : public BaseObject
	{
	// methods
	protected:
		IShader (const SubSystemsRef ss) : BaseObject( ss )
		{}


	// interface
	public:
		virtual void Active () = 0;

		virtual bool SetTextures (Buffer<const TexturePtr> textures) = 0;

		virtual bool SetArg (StringCRef name, VariantRef data) = 0;

		
	// constructors
	public:
		static IShaderPtr  Create_Simple3D (const SubSystemsRef ss);
		static IShaderPtr  Create_TessLandscape (const SubSystemsRef ss);
		static IShaderPtr  Create_SimpleParticles (const SubSystemsRef ss);
	};


}	// ShaderEditor
