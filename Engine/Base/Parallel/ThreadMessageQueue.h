// Copyright © 2014-2017  Zhirnov Andrey. All rights reserved.

#pragma once

#include "ParallelOp.h"

namespace Engine
{
namespace Base
{
	
	//
	// Thread Message Queue
	//

	template <typename MTQ>
	struct TThreadMessageQueue
	{
	// types
	private:
		typedef MTQ							mt_queue_t;
		typedef TThreadMessageQueue< MTQ >	Self;

		struct _ProcessMessage
		{
			_ProcessMessage ()
			{}

			void operator () (const ParallelOp &op)
			{
				op.Process();
			}
		};


	// variables
	private:
		mt_queue_t			_messageQueue;
		
		static const usize _MAX_PENDING_QUEUE_COUNT	= 1024;


	// methods
	public:
		TThreadMessageQueue ()
		{
			_messageQueue.ReserveCurrent( 256 );
			_messageQueue.ReservePending( 128 );
		}


		TThreadMessageQueue (const Self &other) : _messageQueue( other._messageQueue )
		{}


		TThreadMessageQueue (Self &&other) : _messageQueue( RVREF( other._messageQueue ) )
		{}


		~TThreadMessageQueue ()
		{}


		usize Push (const ParallelOp &op)
		{
			const usize	res = _messageQueue.Push( op );

			if ( res > _MAX_PENDING_QUEUE_COUNT )
			{
				WARNING( "pending queue is too big!" );
				Flush();
				return 0;
			}
			return res;
		}


		usize Push (ParallelOp &&op)
		{
			const usize	res = _messageQueue.Push( RVREF( op ) );

			if ( res > _MAX_PENDING_QUEUE_COUNT )
			{
				WARNING( "pending queue is too big!" );
				Flush();
				return 0;
			}
			return res;
		}


		void Flush ()
		{
			return _messageQueue.Flush();
		}


		bool Process ()
		{
			_ProcessMessage	op;
			return _messageQueue.Process( op );
		}


		usize ProcessAll ()
		{
			_ProcessMessage	op;
			return _messageQueue.ProcessAll( op );
		}


		mt_queue_t &	GetQueue ()
		{
			return _messageQueue;
		}
	};


	typedef TThreadMessageQueue< MtQueue< CircularQueue< ParallelOp > > >			ThreadMessageQueue;
	typedef TThreadMessageQueue< MtShortQueue< CircularQueue< ParallelOp > > >		ThreadMessageQueue2;


}	// Base
}	// Engine
