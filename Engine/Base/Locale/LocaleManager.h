// Copyright © 2014-2016  Zhirnov Andrey. All rights reserved.

#pragma once

#include "Engine/Base/Resources/IResourceLoader.h"
#include "Engine/Base/Locale/LocaleStringID.h"

namespace Engine
{
namespace Base
{

	//
	// Locale Manager
	//

	class LocaleManager : public IResourceLoader
	{
	// types
	public:
		typedef Array< uint3 >	char_intervals_t;		// first char, first wchar, count

		//
		// Language Info
		//
		struct LangInfo
		{
		// variables
			String				name,			// example: "English"
								iso3Name;		// example: "eng"
			char_intervals_t	charIntervals;

		// methods
			LangInfo () {}
		};

	private:
		typedef Map< LocaleStringID, /*ext_string*/String >	str_map_t;


	// variables
	private:
		str_map_t	_strings;


	// methods
	public:
		explicit
		LocaleManager (const SubSystemsRef ss);

		~LocaleManager ();
		

		// IResourceLoader //
		bool Load (OUT ResourcePtr &resource, PackFileID fileID, const RFilePtr &file) override;

		EResource::type GetType () const override	{ return EResource::Locale; }
	};
	

}	// Base
}	// Engine