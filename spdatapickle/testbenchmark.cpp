/*
 * Copyright 2009 Stephen Liu
 * For license terms, see the file COPYING along with this library.
 */

#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <sys/stat.h>

#include "dp_xyzbenchmark.hpp"

#include "spdpmetautils.hpp"
#include "spxmlpickle.hpp"
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

int main( int argc, char * argv[] )
{
	if( argc < 3 ) {
		printf( "\nUsage: %s <filename> <loops>\n\n", argv[0] );
		return -1;
	}

	const char * filename = argv[1];
	int loops = atoi( argv[2] );

	test( filename, loops );

	return 0;
}

