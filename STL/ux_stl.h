// Copyright © 2014-2016  Zhirnov Andrey. All rights reserved.

#pragma once


// Include main directives  //
#include "Defines/Init.h"


// Defines //
#include "Defines/Defines.h"
#include "Defines/Errors.h"


// Base Types //
#include "BaseTypes/Types.h"
#include "BaseTypes/Fixed.h"
#include "BaseTypes/Float.h"
#include "BaseTypes/Half.h"
#include "BaseTypes/UFloat.h"
#include "BaseTypes/Bitfield.h"
#include "BaseTypes/TypeCast.h"


// Compile Time //
#include "CompileTime/Constants.h"
#include "CompileTime/TemplateMath.h"
#include "CompileTime/TemplateUtils.h"
#include "CompileTime/PlainOldDataType.h"
#include "CompileTime/TTypeInfo.h"
#include "CompileTime/NewTypeInfo.h"
#include "CompileTime/TypeList.h"
#include "CompileTime/TypeTraits.h"
#include "CompileTime/Signature.h"
#include "CompileTime/Fractional.h"
#include "CompileTime/StaticFloat.h"
#include "CompileTime/StaticInteger.h"
#include "CompileTime/CompileTimeDebug.h"
#include "CompileTime/AlignedType.h"


// Containers //
#include "Containers/CopyStrategy.h"
#include "Containers/Sorts.h"
#include "Containers/Array.h"
#include "Containers/BitArray.h"
#include "Containers/CircularQueue.h"
#include "Containers/Stack.h"
#include "Containers/String.h"
#include "Containers/StaticArray.h"
#include "Containers/StaticBitArray.h"
#include "Containers/Queue.h"
#include "Containers/Set.h"
#include "Containers/Map.h"
#include "Containers/Pair.h"
#include "Containers/Buffer.h"
#include "Containers/TStringRef.h"
//#include "Containers/List.h"
#include "Containers/HashSet.h"
#include "Containers/HashMap.h"


// Memory //
#include "Memory/Allocators.h"
#include "Memory/MemoryContainer.h"
#include "Memory/MemFunc.h"
#include "Memory/MemFunc2.h"


// Types //
#include "Types/Utils.h"
#include "Types/FileAddress.h"
#include "Types/UniquePtr.h"
#include "Types/Ptr.h"
#include "Types/Function.h"
#include "Types/Optional.h"
#include "Types/StringUtils.h"
#include "Types/StringParser.h"
#include "Types/BufferedPtr.h"
#include "Types/Crypt.h"
#include "Types/Union.h"
#include "Types/VariantRef.h"
#include "Types/Noncopyable.h"
#include "Types/Noninstancable.h"
#include "Types/Limit.h"
#include "Types/StaticLimit.h"
//#include "Types/TExtString.h"
//#include "Types/TVariant.h"
#include "Types/ScopeSetter.h"
#include "Types/Hash.h"


// ThreadSafe //
#include "ThreadSafe/Referenced.h"
#include "ThreadSafe/MultiThreadQueue.h"
#include "ThreadSafe/Atomic.h"
#include "ThreadSafe/MtFile.h"
#include "ThreadSafe/Singleton.h"


// Dimensions //
#include "Dimensions/ByteAndBit.h"
#include "Dimensions/RadiansAndDegrees.h"
#include "Dimensions/PhysicsValue.h"
#include "Dimensions/PhysicsValueVec.h"
#include "Dimensions/PhysicsValueMath.h"
#include "Dimensions/Percentage.h"


// Math //
#include "Math/Mathematics.h"
#include "Math/MathTypes.h"
#include "Math/FastMath.h"
#include "Math/Vec.h"
#include "Math/Matrix.h"
#include "Math/Quaternion.h"
#include "Math/TR11G11B10F.h"
#include "Math/TRGB9_E5.h"
#include "Math/Utils.h"
#include "Math/Algebra.h"
#include "Math/MathTypeCast.h"


// Math/2D //
#include "Math/2D/CoordTransform2.h"
#include "Math/2D/Rectangle.h"
#include "Math/2D/Line2.h"
#include "Math/2D/Circle.h"
#include "Math/2D/OrientedRectangle.h"


// Math/3D //
#include "Math/3D/CoordTransform3.h"
#include "Math/3D/Line3.h"
#include "Math/3D/Transform.h"
#include "Math/3D/Triangle.h"
#include "Math/3D/OrientedBox.h"
#include "Math/3D/AxisAlignedBox.h"
#include "Math/3D/PerspectiveCamera.h"
#include "Math/3D/Frustum.h"
#include "Math/3D/Plane.h"
#include "Math/3D/Sphere.h"


// Math/Color //
#include "Math/Color/Color.h"
#include "Math/Color/ColorFormats.h"
//#include "Math/Color/ColorFormatCompressed.h"


// Math/Image //
#include "Math/Image/ImageUtils.h"


// Math/Rand //
#include "Math/Rand/Random.h"
#include "Math/Rand/RandomWithChance.h"
#include "Math/Rand/Pseudorandom.h"
#include "Math/Rand/NormalDistribution.h"


// Math/Spline //
#include "Math/Spline/Spline.h"


// OS //
#include "OS/OSLowLevel.h"


// Files //
#include "Files/BaseFile.h"
#include "Files/HDDFile.h"
#include "Files/MemFile.h"
#include "Files/SubFile.h"
#include "Files/CryptFile.h"
#include "Files/LzmaFile.h"
#include "Files/ZipFile.h"


// DataBase //
#include "DataBase/SimpleDB.h"


// Java //
#include "Java/JavaEnvironment.h"
#include "Java/JavaTypeInfo.h"
#include "Java/JavaArray.h"
#include "Java/JavaString.h"
#include "Java/JavaClass.h"
#include "Java/JavaMethod.h"
#include "Java/JavaMethodType.h"


// Unicode //
#include "Unicode/Utf8StringUtils.h"


// Log //
#include "Log/Logger.h"


// Filters //
#include "Filters/GaussianFilter.h"


// Time //
#include "Time/Time.h"
#include "Time/TimeProfiler.h"


// ScriptBinding //
#include "ScriptBindings/DefaultBindings.h"
#include "ScriptBindings/ScriptBinder.h"
#include "ScriptBindings/ScriptTypes.h"
#include "ScriptBindings/ScriptEnum.h"
#include "ScriptBindings/ScriptEngine.h"

