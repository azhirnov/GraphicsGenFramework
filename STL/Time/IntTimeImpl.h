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
	private:
		T		_time;


	// methods
	public:
		Time (GX_DEFCTOR) : _time(0) {}

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
			return FromNanoSeconds( (T) time.NanoSeconds() );
		}

		Self &	FromSeconds (const T &value)			{ _time = value * T(1000000000);	return *this; }
		Self &	FromMilliSeconds (const T &value)		{ _time = value * T(1000000);		return *this; }
		Self &	FromMicroSeconds (const T &value)		{ _time = value * T(1000);			return *this; }
		Self &	FromNanoSeconds (const T &value)		{ _time = value;					return *this; }
		
		T		Seconds ()						const	{ return (T) GXMath::RoundToUInt( _time * 1.0e-9 ); }
		T		MilliSeconds ()					const	{ return (T) GXMath::RoundToUInt( _time * 1.0e-6 ); }
		T		MicroSeconds ()					const	{ return (T) GXMath::RoundToUInt( _time * 1.0e-3 ); }
		T		NanoSeconds ()					const	{ return _time; }

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

		String	ToString ()						const	{ return String().FormatI( NanoSeconds(), 10 ) << " ns"; }

		// seconds = value * 10 ^ power
		static int	GetSecondsPowerOf10 ()				{ return 9; }
	};


	template <>
	struct Hash< Time<T> > : private Hash<T>
	{
		typedef Time<T>				key_t;
		typedef Hash<T>				base_t;
		typedef base_t::result_t	result_t;

		result_t operator () (const key_t &x) const
		{
			return base_t::operator ()( x.NanoSeconds() );
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
			_start = GetCurrentTIme();
		}

		Time_t GetTimeDelta () const
		{
			return GetCurrentTIme() - _start;
		}

		Time_t GetStartTIme () const
		{
			return _start;
		}

		Time_t GetCurrentTIme () const
		{
			return Time_t().FromTime( _timer.GetTimeMicroSec() );
		}
	};

#endif	// _TIME_PROFILER_IMPL_
	
#undef T
