// Copyright © 2014-2016  Zhirnov Andrey. All rights reserved.

#pragma once

#include "TemplateUtils.h"

namespace GX_STL
{
namespace CompileTime
{


	//
	// IntegerMinLimit, IntegerMaxLimit
	//

	namespace _ctime_hidden_
	{
		template <typename T>
		struct Int_IsSigned
		{
			static const bool	value = ( T(1) > T(-1) );
		};

	}	// _ctime_hidden_

	template <typename T>
	static constexpr T IntegerMinLimit = _ctime_hidden_::Int_IsSigned<T>::value ? T( T(1) << (SizeOf<T>::bits-1) ) : T(0);
	
	template <typename T>
	static constexpr T IntegerMaxLimit = _ctime_hidden_::Int_IsSigned<T>::value ? T( T(-1) & T(~IntegerMinLimit<T>) ) : T(-1);


	
	//
	// Near Integer Type
	//
	
	struct NearInt
	{
	private:
		template <usize SizeValue>
		struct _FromSize {
			typedef typename CompileTime::SwitchType< (SizeValue <= sizeof(byte)), byte,
						typename CompileTime::SwitchType< (SizeValue <= sizeof(short)), short,
							typename CompileTime::SwitchType< (SizeValue <= sizeof(int)), int,
								ilong > > >		type;
		};
		
		template <isize Value>
		struct _FromValue {
			typedef typename CompileTime::SwitchType< (Value >= IntegerMinLimit<byte> and Value <= IntegerMaxLimit<byte>), byte,
						typename CompileTime::SwitchType< (Value >= IntegerMinLimit<short> and Value <= IntegerMaxLimit<short>), short,
							typename CompileTime::SwitchType< (Value >= IntegerMinLimit<int> and Value <= IntegerMaxLimit<int>), int, ilong >
						> >		type;
		};

	public:
		template <usize SizeValue>
		using FromSize = typename _FromSize< SizeValue >::type;

		template <usize Bits>
		using FromBits = typename _FromSize< ((Bits+7) >> 3) >::type;

		template <typename T>
		using FromType = typename _FromSize< sizeof(T) >::type;
	};

	struct NearUInt
	{
	private:
		template <usize SizeValue>
		struct _FromSize {
			typedef typename CompileTime::SwitchType< (SizeValue <= sizeof(ubyte)), ubyte,
						typename CompileTime::SwitchType< (SizeValue <= sizeof(ushort)), ushort,
							typename CompileTime::SwitchType< (SizeValue <= sizeof(uint)), uint,
								ulong > > >		type;
		};

		template <usize Value>
		struct _FromValue {
			typedef typename CompileTime::SwitchType< (Value <= IntegerMaxLimit<ubyte>), ubyte,
						typename CompileTime::SwitchType< (Value <= IntegerMaxLimit<ushort>), ushort,
							typename CompileTime::SwitchType< (Value <= IntegerMaxLimit<uint>), uint, ulong >
						> >						type;
		};

	public:
		template <usize SizeValue>
		using FromSize = typename _FromSize< SizeValue >::type;

		template <usize Bits>
		using FromBits = typename _FromSize< ((Bits+7) >> 3) >::type;

		template <typename T>
		using FromType = typename _FromSize< sizeof(T) >::type;
	};



	//
	// Larger Integer Type
	//
	
	template <typename T>
	using LargerInt = typename NearInt::FromSize< Clamp< uint, (sizeof(T)<<1), sizeof(byte), sizeof(ilong) > >;

	template <typename T>
	using LargerUInt = typename NearUInt::FromSize< Clamp< uint, (sizeof(T)<<1), sizeof(ubyte), sizeof(ulong) > >;



	//
	// Lesser Integer Type
	//
	
	template <typename T>
	using LesserInt = typename NearInt::FromSize< Clamp< uint, (sizeof(T)>>1), sizeof(byte), sizeof(ilong) > >;

	template <typename T>
	using LesserUInt = typename NearUInt::FromSize< Clamp< uint, (sizeof(T)>>1), sizeof(ubyte), sizeof(ulong) > >;



	//
	// Near Float Type
	//

	struct NearFloat
	{
		template <usize SizeValue>
		using FromSize = typename CompileTime::SwitchType< (SizeValue <= sizeof(float)), float,
							typename CompileTime::SwitchType< (SizeValue <= sizeof(double)), double,
								long double > >;

		template <typename T>
		using FromType = typename FromSize< sizeof(T) >;
	};


}	// CompileTime
}	// GX_STL
