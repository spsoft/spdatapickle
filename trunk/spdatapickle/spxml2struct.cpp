/*
 * Copyright 2009 Stephen Liu
 * For license terms, see the file COPYING along with this library.
 */

#include <stdio.h>

#include "spdpxml.hpp"
#include "spdpsyntax.hpp"
#include "spdpcode.hpp"
#include "spdpname.hpp"

void usage( const char * program )
{
	printf( "Usage: %s <xml file>\n", program );
}

void printSyntaxTree( SP_DPSyntaxTree * syntaxTree )
{
	printf( "prefix = %s\n", syntaxTree->getPrefix() );
	printf( "name = %s\n", syntaxTree->getName() );

	SP_DPSyntaxStructVector * slist = syntaxTree->getStructList();
	SP_DPSyntaxStructVector::iterator sit = slist->begin();
	for( ; slist->end() != sit; ++sit ) {
		printf( "\nstruct = %s\n\n", sit->getName() );

		SP_DPSyntaxFieldVector * flist = sit->getFieldList();
		SP_DPSyntaxFieldVector::iterator fit = flist->begin();

		for( ; flist->end() != fit; ++fit ) {
			printf( "field { %s, %s, %d, \"%s\" }\n", fit->getName(), fit->getType(),
					fit->getArraySize(), fit->getReferTo() );

			printf( "\n" );
		}
	}
}

int main( int argc, char * argv[] )
{
	if( argc < 2 ) {
		usage( argv[0] );
		return -1;
	}

	const char * xmlFile = argv[1];

	SP_DPSyntaxTree syntaxTree;
	SP_DPXmlUtils::parse( xmlFile, &syntaxTree );

	SP_DPNameRender nameRender( syntaxTree.getPrefix() );
	SP_DPCodeRender codeRender( &nameRender );

	char filename[ 128 ] = { 0 }, tmp[ 128 ] = { 0 };
	nameRender.getFileName( syntaxTree.getName(), tmp, sizeof( tmp ) );

	// header file
	{
		snprintf( filename, sizeof( filename ), "%s.hpp", tmp );
		FILE * fp = fopen( filename, "w" );
		codeRender.generateHeader( &syntaxTree, fp );
		fclose( fp );

		printf( "spxml2struct: Build %s ... done\n", filename );
	}

	// cpp file
	{
		snprintf( filename, sizeof( filename ), "%s.cpp", tmp );
		FILE * fp = fopen( filename, "w" );
		codeRender.generateMetadata( &syntaxTree, fp );
		fclose( fp );

		printf( "spxml2struct: Build %s ... done\n", filename );
	}

	//printSyntaxTree( &syntaxTree );

	return 0;
}

