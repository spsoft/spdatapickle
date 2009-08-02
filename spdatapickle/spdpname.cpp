/*
 * Copyright 2009 Stephen Liu
 * For license terms, see the file COPYING along with this library.
 */

#include <string.h>
#include <stdio.h>
#include <ctype.h>

#include "spdpname.hpp"
#include "spdpsyntax.hpp"

SP_DPNameRender :: SP_DPNameRender( const char * prefix )
{
	strncpy( mPrefix, prefix, sizeof( mPrefix ) );
}

SP_DPNameRender :: ~SP_DPNameRender()
{
}

const char * SP_DPNameRender :: getFileName( const char * filename, char * name, int size )
{
	snprintf( name, size, "dp_%s%s", mPrefix, filename );
	toLower( name );

	return name;
}

const char * SP_DPNameRender :: getTypeRawName( const char * type, char * name, int size )
{
	typedef struct tagType2Name {
		const char * mType, * mName;
	} Type2Name_t;

	static Type2Name_t type2name [] = {
		{ "char",   "char" },

		{ "int16",  "short" },
		{ "uint16", "unsigned short" },

		{ "int32",  "int" },
		{ "uint32", "unsigned int" },

		{ "int64",  "long long" },
		{ "uint64", "unsigned long long" },

		{ "float",  "float" },
		{ "double", "double" },

		{ NULL, NULL }
	};

	if( '*' == *type ) type++;

	const char * realname = NULL;
	int isBaseType = 0;

	for( int i = 0; ; ++i ) {
		Type2Name_t * iter = &(type2name[i]);

		if( NULL == iter->mType ) break;

		if( 0 == strcmp( type, iter->mType ) ) {
			isBaseType = 1;
			realname = iter->mName;
			break;
		}
	}

	char structName[ 128 ] = { 0 };
	if( 0 == isBaseType ) {
		char tmp[ 128 ] = { 0 };
		snprintf( structName, sizeof( structName ), "%s_t",
				getStructBaseName( type, tmp, sizeof( tmp ) ) );
		realname = structName;
	}

	strncpy( name, realname, size - 1 );
	name[ size - 1 ] = '\0';

	return name;
}

const char * SP_DPNameRender :: getTypeName( const char * type, char * name, int size )
{
	char tmp[ 128 ] = { 0 };
	getTypeRawName( type, tmp, sizeof( tmp ) );

	if( '*' == *type ) {
		snprintf( name, size, "%s *", tmp );
	} else {
		strncpy( name, tmp, size - 1 );
		name[ size - 1 ] = '\0';
	}

	return name;
}

const char * SP_DPNameRender :: getTypeFullName( SP_DPSyntaxField * field, char * name, int size )
{
	if( field->getArraySize() > 0 ) {
		char tmp[ 128 ] = { 0 };
		getTypeName( field->getType(), tmp, sizeof( tmp ) );
		snprintf( name, size, "%s[%d]", tmp, field->getArraySize() );
	} else {
		getTypeName( field->getType(), name, size );
	}

	return name;
}

const char * SP_DPNameRender :: getTypeEnum( const char * type, char * name, int size )
{
	typedef struct tagType2Name {
		const char * mType, * mName;
	} Type2Name_t;

	static Type2Name_t type2name [] = {
		{ "char",   "eTypeSPDPChar" },

		{ "int16",  "eTypeSPDPInt16" },
		{ "uint16", "eTypeSPDPUint16" },

		{ "int32",  "eTypeSPDPInt32" },
		{ "uint32", "eTypeSPDPUint32" },

		{ "int64",  "eTypeSPDPInt64" },
		{ "uint64", "eTypeSPDPUint64" },

		{ "float",  "eTypeSPDPFloat" },
		{ "double", "eTypeSPDPDouble" },

		{ NULL, NULL }
	};

	int isPtrType = ( '*' == *type );
	int isBaseType = 0;

	const char * realname = type;
	if( isPtrType ) realname++;

	for( int i = 0; ; ++i ) {
		Type2Name_t * iter = &(type2name[i]);

		if( NULL == iter->mType ) break;

		if( 0 == strcmp( realname, iter->mType ) ) {
			isBaseType = 1;
			realname = iter->mName;
			break;
		}
	}

	char structName[ 128 ] = { 0 };
	if( 0 == isBaseType ) {
		char tmp[ 128 ] = { 0 };
		snprintf( structName, sizeof( structName ), "eType%s",
				getStructBaseName( realname, tmp, sizeof( tmp ) ) );
		realname = structName;
	}

	snprintf( name, size, "%s", realname );

	return name;
}

const char * SP_DPNameRender :: getFieldName( const char * name, char * fieldName, int size )
{
	snprintf( fieldName, size, "m%c%s", toupper( *name ), name + 1 );

	return fieldName;
}

const char * SP_DPNameRender :: getStructBaseName( const char * name, char * structName, int size )
{
	snprintf( structName, size, "%s%c%s", mPrefix, toupper( *name ), name + 1 );

	return structName;
}

char * SP_DPNameRender :: toLower ( register char *s )
{
	register char *ret = s;

	for ( ; *s != '\0'; ++s ) *s = tolower( *s );

	return ret;
}

char * SP_DPNameRender :: toUpper( register char *s )
{
	register char * ret = s;

	for( ; *s != '\0'; s++ ) *s = toupper( *s );

	return ret;
}

