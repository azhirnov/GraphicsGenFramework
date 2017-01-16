// Copyright © 2014-2016  Zhirnov Andrey. All rights reserved.
/*
	MultiThreadQueue is optimized for the large number of elements.

	MultiThreadShortQueue is optimized for fast transfer of a small
	number of elements to the other thread.
*/

#pragma once

#include "../OS/OSLowLevel.h"

namespace GX_STL
{
namespace GXTypes
{

	//
	// Multi thread queue
	//

	template <typename QueueType>
	struct MultiThreadQueue
	{
	// types
	public:
		typedef QueueType							queue_t;
		typedef typename QueueType::value_t			value_t;
		typedef MultiThreadQueue< QueueType >		Self;


	// variables
	protected:
		queue_t		_currentQueue,
					_pendingQueue;

		Mutex		_lockCurrentQueue,
					_lockPendingQueue;


	// methods
	public:
		MultiThreadQueue ()
		{}
		

		MultiThreadQueue (const Self &other) :
			_currentQueue(_currentQueue), _pendingQueue(_pendingQueue)
		{}


		MultiThreadQueue (Self &&other) :
			_currentQueue( RVREF( other._currentQueue ) ),
			_pendingQueue( RVREF( other._pendingQueue ) )
		{}


		~MultiThreadQueue ()
		{
			ClearAll();
		}


		void ClearAll ()
		{
			SCOPELOCK_UNIQUE( _lockPendingQueue );
			SCOPELOCK_UNIQUE( _lockCurrentQueue );

			_currentQueue.Clear();
			_pendingQueue.Clear();
		}


		void ClearCurrent ()
		{
			SCOPELOCK( _lockCurrentQueue );
			_currentQueue.Clear();
		}


		void ClearPending ()
		{
			SCOPELOCK( _lockPendingQueue );
			_pendingQueue.Clear();
		}


		void ReserveCurrent (usize size)
		{
			SCOPELOCK( _lockCurrentQueue );
			_currentQueue.Reserve( size );
		}


		void ReservePending (usize size)
		{
			SCOPELOCK( _lockPendingQueue );
			_pendingQueue.Reserve( size );
		}


		usize GetPendingQueueCount ()
		{
			SCOPELOCK( _lockPendingQueue );
			return _pendingQueue.Count();
		}


		usize GetCurrentQueueCount ()
		{
			SCOPELOCK( _lockCurrentQueue );
			return _currentQueue.Count();
		}
		

		void Flush ()
		{
			SCOPELOCK_UNIQUE( _lockPendingQueue );

			if ( not _pendingQueue.Empty() )
			{
				SCOPELOCK_UNIQUE( _lockCurrentQueue );

				_currentQueue.AppendBack( _pendingQueue );
				_pendingQueue.Clear();
			}
		}

		
		usize Push (const value_t &x)
		{
			SCOPELOCK( _lockPendingQueue );
			_pendingQueue.PushBack( x );
			return _pendingQueue.Count();
		}


		usize Push (value_t &&x)
		{
			SCOPELOCK( _lockPendingQueue );
			_pendingQueue.PushBack( RVREF( x ) );
			return _pendingQueue.Count();
		}

		
		usize Append (const QueueType &q)
		{
			SCOPELOCK( _lockPendingQueue );
			_pendingQueue.AppendBack( q );
			return _pendingQueue.Count();
		}


		template <typename Op>
		bool Process (Op &op)
		{
			value_t	x;
			{
				SCOPELOCK( _lockCurrentQueue );

				if ( _currentQueue.Empty() )
					return false;
				
				x = _currentQueue.Front();
				_currentQueue.PopFront();
			}

			op( x );
			return true;
		}

		
		template <typename Op>
		usize ProcessAll (Op &op)
		{
			value_t	x;
			usize		counter = 0;

			while ( true )
			{
				{
					SCOPELOCK( _lockCurrentQueue );

					if ( _currentQueue.Empty() )
						break;
				
					x = _currentQueue.Front();
					_currentQueue.PopFront();

				}
				++counter;
				op( x );
			}
			return counter;
		}
	};


	
	//
	// Multi thread short queue
	//

	template <typename QueueType>
	struct MultiThreadShortQueue : public MultiThreadQueue< QueueType >
	{
	// types
	private:
		typedef MultiThreadQueue< QueueType >		parent_t;
		typedef QueueType							queue_t;
		typedef typename QueueType::value_t			value_t;
		typedef MultiThreadShortQueue< QueueType >	Self;


	// methods
	private:
		void _TryFlush ()
		{
			// Warning: _lockPendingQueue must be locked!

			if ( parent_t::_pendingQueue.Empty() )
				return;

			if ( parent_t::_lockCurrentQueue.TryLock() )
			{
				parent_t::_currentQueue.AppendBack( parent_t::_pendingQueue );
				parent_t::_lockCurrentQueue.Unlock();

				parent_t::_pendingQueue.Clear();
			}
		}


	public:
		usize Push (const value_t &x)
		{
			SCOPELOCK( parent_t::_lockPendingQueue );
			parent_t::_pendingQueue.PushBack( x );
			_TryFlush();
			return parent_t::_pendingQueue.Count();
		}
		

		usize Push (value_t &&x)
		{
			SCOPELOCK( parent_t::_lockPendingQueue );
			parent_t::_pendingQueue.PushBack( RVREF( x ) );
			_TryFlush();
			return parent_t::_pendingQueue.Count();
		}


		void Append (const QueueType &q)
		{
			SCOPELOCK( parent_t::_lockPendingQueue );
			parent_t::_pendingQueue.AppendBack( q );
			_TryFlush();
		}


		void TryFlush ()
		{
			SCOPELOCK( parent_t::_lockPendingQueue );
			_TryFlush();
		}


		template <typename Op>
		usize ProcessAll (Op &op)
		{
			SCOPELOCK( parent_t::_lockCurrentQueue );
			usize	counter = 0;

			while ( not parent_t::_currentQueue.Empty() )
			{
				op( parent_t::_currentQueue.Front() );
				parent_t::_currentQueue.PopFront();
				++counter;
			}
			return counter;
		}
	};


}	// GXTypes
}	// GX_STL
