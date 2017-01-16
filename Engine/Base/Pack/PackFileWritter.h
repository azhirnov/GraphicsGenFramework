// Copyright © 2014-2016  Zhirnov Andrey. All rights reserved.

#pragma once

#include "PackManager.h"

namespace Engine
{
namespace Base
{

	class PackFileWritter;
	SHARED_POINTER( PackFileWritter );



	//
	// Pack File Writter
	//

	class PackFileWritter : public BaseObject
	{
	// types
	private:
		typedef PackManager::FileHeader			FileHeader;
		typedef PackManager::PackFileHeader		PackFileHeader;

		typedef Array< FileHeader >				FileHeaders_t;
		typedef Map< PackFileID, String >		FileMap_t;


	// variables
	private:
		FileHeaders_t		_fileHeaders;
		FileMap_t			_fileMap;

		ulong				_totalSize;

		static const ulong	_maxSize	= (1ull << 31);


	// methods
	public:
		explicit
		PackFileWritter (const SubSystemsRef ss);

		bool Create ();
		bool Save (StringCRef filename);
		void Close ();

		bool Add (PackFileID id, StringCRef filename);

		bool UpdatePackSize ();

		ulong GetTotalPackSize ()	const		{ return _totalSize; }
		ulong GetMaxPackSize ()	const		{ return _maxSize; }

		static PackFileWritterPtr  New (const SubSystemsRef ss);
	};
	

}	// Base
}	// Engine
