// Copyright © 2014-2016  Zhirnov Andrey. All rights reserved.

#if defined( _TIME_IMPL_ )

	template <>
	struct Time< T >
	{
	// types
	public:
		typedef T			value_t;
		typedef Time< T >	Self;


	// variables
	protected:
		T	_time;


	// methods
	public:
		Time (GX_DEFCTOR) : _time(0.0) {}

		explicit
		Time (const T &value) : _time(value) {}
		
		template <typename B>
		Time (const Time<B> &value) : _time(0)
		{
			FromTime( value );
		}

		template <typename B>
		Self &	FromTime (const B &time)
		{
			if ( time.GetSecondsPowerOf10() <= 0 )		return FromSeconds( (T) time.Seconds() );
			if ( time.GetSecondsPowerOf10() <= 3 )		return FromMilliSeconds( (T) time.MilliSeconds() );
			if ( time.GetSecondsPowerOf10() <= 6 )		return FromMicroSeconds( (T) time.MicroSeconds() );

			return FromNanoSeconds( (T) time.NanoSeconds() );
		}

		Self &	FromSeconds (const T &value)			{ _time = value;				return *this; }
		Self &	FromMilliSeconds (const T &value)		{ _time = value * T(1.0e-3);	return *this; }
		Self &	FromMicroSeconds (const T &value)		{ _time = value * T(1.0e-6);	return *this; }
		Self &	FromNanoSeconds (const T &value)		{ _time = value * T(1.0e-9);	return *this; }

		T		Seconds ()						const	{ return _time; }
		T		MilliSeconds ()					const	{ return _time * T(1.0e+3); }
		T		MicroSeconds ()					const	{ return _time * T(1.0e+6); }
		T		NanoSeconds ()					const	{ return _time * T(1.0e+9); }

		Self &	operator += (const Self &right)			{ _time += right._time;  return *this; }
		Self &	operator -= (const Self &right)			{ _time -= right._time;  return *this; }

		Self	operator +  (const Self &right)	const	{ return ( Self(*this) += right ); }
		Self	operator -  (const Self &right)	const	{ return ( Self(*this) -= right ); }

		bool	operator == (const Self &right)	const	{ return _time == right._time; }
		bool	operator != (const Self &right)	const	{ return _time != right._time; }
		bool	operator >  (const Self &right)	const	{ return _time >  right._time; }
		bool	operator <  (const Self &right)	const	{ return _time <  right._time; }
		bool	operator >= (const Self &right)	const	{ return _time >= right._time; }
		bool	operator <= (const Self &right)	const	{ return _time <= right._time; }

		template <typename B>
		B		To ()							const	{ return B().FromTime( *this ); }

		String	ToString ()						const	{ return String().FormatF( Seconds() ) << " s"; }

		// seconds = value * 10 ^ power
		static int	GetSecondsPowerOf10 ()				{ return 0; }
	};


	template <>
	struct Hash< Time<T> > : private Hash<T>
	{
		typedef Time<T>				key_t;
		typedef Hash<T>				base_t;
		typedef base_t::result_t	result_t;

		result_t operator () (const key_t &x) const
		{
			return base_t::operator ()( x.Seconds() );
		}
	};
	
#endif	// _TIME_IMPL_


#if defined( _TIME_PROFILER_IMPL_ )

	template <>
	struct TimeProfiler< T >
	{
	// types
	public:
		typedef T					value_t;
		typedef Time< T >			Time_t;
		typedef TimeProfiler< T >	Self;


	// variables
	private:
		PerfomanceTimer	_timer;
		Time_t			_start;


	// methods
	public:
		explicit
		TimeProfiler (bool start = false)
		{
			if ( start ) {
				Start();
			}
		}

		void Start ()
		{
			_start = GetCurrentTime();
		}

		Time_t GetTimeDelta () const
		{
			return GetCurrentTime() - _start;
		}

		Time_t GetStartTime () const
		{
			return _start;
		}

		Time_t GetCurrentTime () const
		{
			return Time_t().FromTime( _timer.GetTime() );
		}
	};

#endif	// _TIME_PROFILER_IMPL_

#undef T
