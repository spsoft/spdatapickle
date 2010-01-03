/*
 * Copyright 2009 Stephen Liu
 * For license terms, see the file COPYING along with this library.
 */

#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <sys/stat.h>
#include <assert.h>

#include "dp_xyzbenchmark.hpp"

#include "spdpmetautils.hpp"
#include "spxmlpickle.hpp"
#include "spxmlrpcpickle.hpp"
#include "spjsonpickle.hpp"
#include "sppbpickle.hpp"
#include "spdpalloc.hpp"

#include "spxml/spxmlutils.hpp"
#include "spjson/spjsonutils.hpp"

void printBuffer( SP_XmlStringBuffer * buffer )
{
	const char * pos = buffer->getBuffer();
	int size = buffer->getSize();

	int isPrint = 1;
	for( int i = 0; i < size; i++ ) {
		if( ! isprint( pos[i] ) && ! isspace( pos[i] ) ) {
			isPrint = 0;
			break;
		}
	}

	if( isPrint ) {
		printf( "len %d\n%s\n\n", size, pos );
 	} else {
		printf( "<binary buffer, size %d>\n", size );
	}
}

static void test( const char * filename, int loops )
{
	FILE * fp = fopen ( filename, "r" );
	if( NULL == fp ) {
		printf( "cannot not open %s\n", filename );
		exit( -1 );
	}

	struct stat aStat;
	char * source = NULL;
	stat( filename, &aStat );
	source = ( char * ) malloc ( aStat.st_size + 1 );
	fread ( source, aStat.st_size, sizeof ( char ), fp );
	fclose ( fp );
	source[ aStat.st_size ] = '\0';

	SP_ProtoBufPickle pbPickle( gXYZBenchmarkMetaInfo );

	SP_DPAlloc alloc( gXYZBenchmarkMetaInfo );

	for( int i = 0; i < loops; i++ ) {

		XYZSpeedMessage1_t msg1;
		memset( &msg1, 0, sizeof( msg1 ) );

		int ret = pbPickle.unpickle( source, aStat.st_size, eTypeXYZSpeedMessage1, &msg1, sizeof( msg1 ) );

		if( 0 != ret ) printf( "unpickle %d\n", ret );

		alloc.free( &msg1, sizeof( msg1 ), eTypeXYZSpeedMessage1 );
	}

	free( source );
}

void initBuiltin( XYZBuiltinMix_t * mix )
{
	mix->mChar.mValue = 'a';
	mix->mInt16.mValue = 161;
	mix->mUInt16.mValue = 162;
	mix->mInt32.mValue = 321;
	mix->mUInt32.mValue = 322;
	mix->mInt64.mValue = 641;
	mix->mUInt64.mValue = 642;
	mix->mFloat.mValue = 661;
	mix->mDouble.mValue = 662;
	mix->mString.mString = strdup( "Hello World!" );

	mix->mBuffer.mSize = 3;
	{
		mix->mBuffer.mBuffer = (char*)calloc( sizeof( char ), 3 );
		for( int i = 0; i < 3; i++ ) {
				mix->mBuffer.mBuffer[i] = i;
		}
	}

	mix->mStringList.mCount = 3;
	{
		mix->mStringList.mList = (SP_DPString_t*)calloc( sizeof( SP_DPString_t ), 3 );
		for( int i = 0; i < 3; i++ ) {
			mix->mStringList.mList[i].mString = strdup( "str" );
		}
	}

	mix->mInt32List.mCount = 3;
	{
		mix->mInt32List.mList = (int*)calloc( sizeof( int ), 3 );
		for( int i = 0; i < 3; i++ ) {
			mix->mInt32List.mList[i] = i;
		}
	}

	mix->mUInt32List.mCount = 3;
	{
		mix->mUInt32List.mList = (unsigned int*)calloc( sizeof( int ), 3 );
		for( int i = 0; i < 3; i++ ) {
			mix->mUInt32List.mList[i] = i;
		}
	}

	mix->mInt64List.mCount = 3;
	{
		mix->mInt64List.mList = (int64_t*)calloc( sizeof( uint64_t ), 3 );
		for( int i = 0; i < 3; i++ ) {
			mix->mInt64List.mList[i] = i;
		}
	}

	mix->mUInt64List.mCount = 3;
	{
		mix->mUInt64List.mList = (uint64_t*)calloc( sizeof( uint64_t ), 3 );
		for( int i = 0; i < 3; i++ ) {
			mix->mUInt64List.mList[i] = i;
		}
	}
}

void testBuiltin()
{
	XYZBuiltinMix_t mix, newMix;

	initBuiltin( &mix );

	SP_DPMetaUtils::dump( gXYZBenchmarkMetaInfo );

	SP_XmlPickle      xmlPickle( gXYZBenchmarkMetaInfo );
	{
		SP_XmlStringBuffer buffer;
		xmlPickle.pickle( &mix, sizeof( mix ), eTypeXYZBuiltinMix, &buffer );
		//printf( "xml:\n\n%s\n\n", buffer.getBuffer() );

		xmlPickle.unpickle( buffer.getBuffer(), buffer.getSize(),
				eTypeXYZBuiltinMix, &newMix, sizeof( newMix ) );

		SP_XmlStringBuffer newBuffer;
		xmlPickle.pickle( &newMix, sizeof( newMix ), eTypeXYZBuiltinMix, &newBuffer );

		assert( 0 == ( strcasecmp( newBuffer.getBuffer(), buffer.getBuffer() ) ) );
	}

	SP_XmlRpcPickle   xmlRpcPickle( gXYZBenchmarkMetaInfo );
	{
		SP_XmlStringBuffer buffer;
		xmlRpcPickle.pickle( &mix, sizeof( mix ), eTypeXYZBuiltinMix, &buffer );
		//printf( "xmlrpc:\n\n%s\n\n", buffer.getBuffer() );
	}

	SP_JsonPickle     jsonPickle( gXYZBenchmarkMetaInfo );
	{
		SP_XmlStringBuffer buffer;
		jsonPickle.pickle( &mix, sizeof( mix ), eTypeXYZBuiltinMix, &buffer );
		//printf( "json:\n\n%s\n\n", buffer.getBuffer() );
	}

	SP_ProtoBufPickle pbPickle( gXYZBenchmarkMetaInfo );
	{
		SP_XmlStringBuffer buffer;
		pbPickle.pickle( &mix, sizeof( mix ), eTypeXYZBuiltinMix, &buffer );

		//printf( "protobuf:\n\n" );
		//printBuffer( &buffer );
	}

	SP_DPAlloc alloc( gXYZBenchmarkMetaInfo );
	alloc.free( &mix, sizeof( mix ), eTypeXYZBuiltinMix );
}

int main( int argc, char * argv[] )
{
	if( argc < 3 ) {
		printf( "\nUsage: %s <filename> <loops>\n\n", argv[0] );
		return -1;
	}

	const char * filename = argv[1];
	int loops = atoi( argv[2] );

	test( filename, loops );

	testBuiltin();

	return 0;
}

