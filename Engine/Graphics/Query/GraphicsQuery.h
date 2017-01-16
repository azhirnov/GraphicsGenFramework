// Copyright © 2017  Zhirnov Andrey. All rights reserved.

#pragma once

#include "Engine/Graphics/Types/Common.h"

namespace Engine
{
namespace Graphics
{

	class GraphicsQuery;
	SHARED_POINTER( GraphicsQuery );



	//
	// Graphics Query
	//

	class GraphicsQuery : public BaseObject
	{
	// variables
	private:
		QueryID			_id;
		EQuery::type	_target;
		uint			_index;
		ulong			_result;
		bool			_resultAvailable;
		bool			_resultCached;


	// methods
	public:
		explicit
		GraphicsQuery (const SubSystemsRef ss);
		~GraphicsQuery ();

		bool Create ();

		void Begin (EQuery::type target, uint index);
		void End ();

		bool IsResultAvailable ();

		ulong GetResult ();

		EQuery::type	Target ()	const	{ return _target; }
		uint			Index ()	const	{ return _index; }
		QueryID const&	QueryId ()	const	{ return _id; }

		static GraphicsQueryPtr  New (const SubSystemsRef ss);

	private:
		void _Delete ();

	// api specific functions
	private:
		bool _IsResultAvailable () const;
		ulong _SyncResult () const;
		//ulong _AsyncResult () const;
	};


}	// Graphics
}	// Engine
