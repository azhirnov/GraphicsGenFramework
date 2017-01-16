// Copyright © 2014-2016  Zhirnov Andrey. All rights reserved.

#include "STL/ux_stl.h"

using namespace GX_STL;
using namespace GX_STL::GXTypes;
using namespace GX_STL::GXMath;


extern void Test_Types_FileAddress ()
{
	const String filename = "../11/../22/33\\44\\55.ab";

	const String ext = FileAddressUtils::GetExtension( filename );			ASSERT( ext == "ab" );
	const String name = FileAddressUtils::GetName( filename );				ASSERT( name == "55" );
	const String path = FileAddressUtils::GetPath( filename );				ASSERT( path == "../11/../22/33\\44" );
	const String name_ext = FileAddressUtils::GetNameAndExt( filename );	ASSERT( name_ext == "55.ab" );

	Array<StringCRef> path_arr;
	FileAddressUtils::DividePath( filename, path_arr );
	ASSERT( path_arr.Count() == 7 );
	ASSERT( path_arr[0] == ".." );
	ASSERT( path_arr[1] == "11" );
	ASSERT( path_arr[2] == ".." );
	ASSERT( path_arr[3] == "22" );
	ASSERT( path_arr[4] == "33" );
	ASSERT( path_arr[5] == "44" );
	ASSERT( path_arr[6] == "55.ab" );

	String	without_name = filename;
	FileAddressUtils::RemoveName( without_name );
	ASSERT( without_name == "../11/../22/33\\44" );
	
	String	without_ext = filename;
	FileAddressUtils::RemoveExtension( without_ext );
	ASSERT( without_ext == "../11/../22/33\\44\\55" );

	String formated = filename;
	FileAddressUtils::FormatPath( formated );
	ASSERT( formated == "../22/33/44/55.ab" );
	
	formated = "aa/bb/../../../aa/bb/cc/dd";
	FileAddressUtils::FormatPath( formated );
	ASSERT( formated == "../aa/bb/cc/dd" );

	String left = filename;
	FileAddressUtils::RemoveDirectoriesFromLeft( left, 2 );
	ASSERT( left == "../11" );

	String right = filename;
	FileAddressUtils::RemoveDirectoriesFromRight( right, 2 );
	ASSERT( right == "../11/../22/33" );

	String dir_path = "path";
	FileAddressUtils::AddDirectoryToPath( dir_path, "dir" );
	ASSERT( dir_path == "path/dir" );
	
	dir_path = "/path/";
	FileAddressUtils::AddDirectoryToPath( dir_path, "/dir1/" );
	FileAddressUtils::AddDirectoryToPath( dir_path, "/dir2/" );
	ASSERT( dir_path == "/path/dir1/dir2/" );
	
	dir_path = "path";
	FileAddressUtils::AddBaseDirectoryToPath( dir_path, "dir" );
	ASSERT( dir_path == "dir/path" );
	
	dir_path = "/path/";
	FileAddressUtils::AddBaseDirectoryToPath( dir_path, "/dir1/" );
	FileAddressUtils::AddBaseDirectoryToPath( dir_path, "/dir2/" );
	ASSERT( dir_path == "/dir2/dir1/path/" );

	String name1 = name;
	FileAddressUtils::AddExtensionToName( name1, ".ext" );		ASSERT( name1 == name + ".ext" );
	
	name1 = name + ".";
	FileAddressUtils::AddExtensionToName( name1, ".ext" );		ASSERT( name1 == name + ".ext" );
	
	name1 = name;
	FileAddressUtils::AddExtensionToName( name1, "ext" );		ASSERT( name1 == name + ".ext" );
	
	name1 = name + ".";
	FileAddressUtils::AddExtensionToName( name1, "ext" );		ASSERT( name1 == name + ".ext" );
}
