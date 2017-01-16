// Copyright © 2017  Zhirnov Andrey. All rights reserved.

#pragma once

#include "../Core/Common.h"

namespace ShaderEditor
{

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

		virtual bool SetData (StringCRef name, VariantRef data) = 0;
	};

	SHARED_POINTER( IShader );


}	// ShaderEditor
