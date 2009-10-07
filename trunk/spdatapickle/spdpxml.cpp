/*
 * Copyright 2009 Stephen Liu
 * For license terms, see the file COPYING along with this library.
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <string.h>
#include <errno.h>

#include "spdpxml.hpp"

#include "spdpsyntax.hpp"

#include "spxml/spdomparser.hpp"
#include "spxml/spdomiterator.hpp"
#include "spxml/spxmlnode.hpp"
#include "spxml/spxmlhandle.hpp"

int SP_DPXmlUtils :: parse( const char * xmlFile, SP_DPSyntaxTree * syntaxTree )
{
	struct stat fileStat;

	if( 0 != stat( xmlFile, &fileStat ) ) {
		fprintf( stderr, "Invalid xml file: stat errno %d, %s\n", errno, strerror( errno ) );
		return -1;
	}

	FILE * fp = fopen( xmlFile, "r" );
	if( NULL == fp ) {
		fprintf( stderr, "Invalid xml file: fopen errno %d, %s\n", errno, strerror( errno ) );
		return -1;
	}

	char * fileContent = (char*)malloc( fileStat.st_size + 1 );
	memset( fileContent, 0, fileStat.st_size + 1 );
	fread( fileContent, fileStat.st_size, sizeof( char ), fp );

	fclose( fp );

	SP_XmlDomParser parser;
	parser.append( fileContent, fileStat.st_size );

	free( fileContent );

	if( NULL != parser.getError() ) {
		fprintf( stderr, "Invalid xml file: %s\n", parser.getError() );
		return -1;
	}

	if( NULL == parser.getDocument()->getRootElement() ) {
		fprintf( stderr, "Invalid xml file: empty document\n" );
		return -1;
	}

	syntaxTree->setDefineFile( xmlFile );

	SP_XmlElementNode * root = parser.getDocument()->getRootElement();

	if( 0 != strcmp( "metainfo", root->getName() ) ) {
		fprintf( stderr, "Invalid xml file: root is %s\n", root->getName() );
		return -1;
	}

	const char * prefix = root->getAttrValue( "prefix" );
	const char * name = root->getAttrValue( "filename" );

	if( NULL == prefix || NULL == name ) {
		fprintf( stderr, "Invalid xml file: prefix|name miss\n" );
		return -1;
	}

	syntaxTree->setPrefix( prefix );
	syntaxTree->setName( name );

	int ret = 0;

	const SP_XmlNodeList * children = root->getChildren();
	for( int i = 0; i < children->getLength(); i++ ) {
		SP_DPSyntaxStruct structure;
		if( 0 == parseStruct( children->get(i), &structure ) ) {
			syntaxTree->getStructList()->push_back( structure );
		} else {
			fprintf( stderr, "Invalid xml file: struct element\n" );
			ret = -1;
			break;
		}
	}

	return ret;
}

int SP_DPXmlUtils :: parseField( SP_XmlNode * xmlNode, SP_DPSyntaxField * field )
{
	if( SP_XmlNode::eELEMENT != xmlNode->getType() ) return -1;

	SP_XmlElementNode * element = (SP_XmlElementNode*)xmlNode;

	if( 0 != strcmp( "field", element->getName() ) ) return -1;

	const char * name = element->getAttrValue( "name" );
	if( NULL == name ) return -1;

	field->setName( name );

	const char * id = element->getAttrValue( "id" );
	const char * type = element->getAttrValue( "type" );
	const char * arraySize = element->getAttrValue( "arraysize" );
	const char * referTo = element->getAttrValue( "referto" );
	const char * required = element->getAttrValue( "required" );

	if( NULL != id ) field->setId( atoi( id ) );
	if( NULL != type ) field->setType( type );
	if( NULL != arraySize ) field->setArraySize( atoi( arraySize ) );
	if( NULL != referTo ) field->setReferTo( referTo );
	if( NULL != required ) field->setRequired( atoi( required ) );

	return NULL != type ? 0 : -1;
}

int SP_DPXmlUtils :: parseStruct( SP_XmlNode * xmlNode, SP_DPSyntaxStruct * structure )
{
	if( SP_XmlNode::eELEMENT != xmlNode->getType() ) return -1;

	SP_XmlElementNode * element = (SP_XmlElementNode*)xmlNode;

	if( 0 != strcmp( "struct", element->getName() ) ) return -1;

	const char * name = element->getAttrValue( "name" );
	if( NULL == name ) return -1;

	structure->setName( name );

	const SP_XmlNodeList * children = element->getChildren();

	int ret = 0;

	for( int i = 0; i < children->getLength(); i++ ) {
		SP_DPSyntaxField field;
		if( 0 == parseField( children->get(i), &field ) ) {
			structure->getFieldList()->push_back( field );
		} else {
			fprintf( stderr, "Invalid xml file: field element\n" );
			ret = -1;
			break;
		}
	}

	SP_DPSyntaxFieldVector * fieldList = structure->getFieldList();
	SP_DPSyntaxFieldVector::iterator fit = fieldList->begin();

	for( ; fieldList->end() != fit; ++fit ) {
		if( '\0' != *( fit->getReferTo() ) ) {
			SP_DPSyntaxFieldVector::iterator it = fieldList->begin();
			for( ; fieldList->end() != it; ++it ) {
				if( 0 == strcmp( it->getName(), fit->getReferTo() ) ) {
					it->setReferred( 1 );
					break;
				}
			}
		}
	}

	return ret;
}

