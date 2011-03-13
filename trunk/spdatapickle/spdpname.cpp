/*
 * Copyright 2009 Stephen Liu
 * For license terms, see the file COPYING along with this library.
 */

#include <string.h>
#include <stdio.h>
#include <ctype.h>

#include "spdpname.hpp"
#include "spdpsyntax.hpp"

#include "spjson/spjsonport.hpp"

SP_DPNameRender :: SP_DPNameRender( const char * prefix )
{
	strncpy( mPrefix, prefix, sizeof( mPrefix ) );
}

SP_DPNameRender :: ~SP_DPNameRender()
{
}

const char * SP_DPNameRender :: getFileName( const char * filename, char * name, int size )
{
	snprintf( name, size, "dp_%s%sstruct", mPrefix, filename );
	toLower( name );

	return name;
}

const char * SP_DPNameRender :: getClassFileName( const char * filename, char * name, int size )
{
	snprintf( name, size, "dp_%s%sclass", mPrefix, filename );
	toLower( name );

	return name;
}

int SP_DPNameRender :: isBuiltinType( const char * type )
{
	static char * builtinTypes [] = {
		"SP_DPChar",
		"SP_DPInt16",
		"SP_DPUInt16",
		"SP_DPInt32",
		"SP_DPUInt32",
		"SP_DPInt64",
		"SP_DPUInt64",
		"SP_DPFloat",
		"SP_DPDouble",
		"SP_DPString",
		"SP_DPBuffer",
		"SP_DPStringList",
		"SP_DPInt32List",
		"SP_DPUInt32List",
		"SP_DPInt64List",
		"SP_DPUInt64List",
		NULL
	};

	for( int i = 0; ; i++ ) {
		if( NULL == builtinTypes[i] ) break;

		if( 0 == strcmp( type, builtinTypes[i] ) ) {
			return 1;
		}
	}

	return 0;
}

int SP_DPNameRender :: isBaseType( const char * type )
{
	static char * baseTypes [] = {
		"char",
		"int16",
		"uint16",
		"int32",
		"uint32",
		"int64",
		"uint64",
		"float",
		"double",
		NULL
	};

	const char * realname = ( '*' == *type ? type + 1 : type );

	for( int i = 0; ; i++ ) {
		if( NULL == baseTypes[i] ) break;

		if( 0 == strcmp( realname, baseTypes[i] ) ) {
			return 1;
		}
	}

	return 0;
}

const char * SP_DPNameRender :: getTypeRawName( const char * type, char * name, int size )
{
	typedef struct tagType2Name {
		const char * mType, * mName;
	} Type2Name_t;

	static Type2Name_t type2name [] = {
		{ "char",   "char" },

		{ "int16",  "int16_t" },
		{ "uint16", "uint16_t" },

		{ "int32",  "int32_t" },
		{ "uint32", "uint32_t" },

		{ "int64",  "int64_t" },
		{ "uint64", "uint64_t" },

		{ "float",  "float" },
		{ "double", "double" },

		{ "SP_DPChar",   "SP_DPChar_t" },
		{ "SP_DPInt16",  "SP_DPInt16_t" },
		{ "SP_DPUInt16", "SP_DPUInt16_t" },
		{ "SP_DPInt32",  "SP_DPInt32_t" },
		{ "SP_DPUInt32", "SP_DPUInt32_t" },
		{ "SP_DPInt64",  "SP_DPInt64_t" },
		{ "SP_DPUInt64", "SP_DPUInt64_t" },
		{ "SP_DPFloat",  "SP_DPFloat_t" },
		{ "SP_DPDouble", "SP_DPDouble_t" },
		{ "SP_DPString", "SP_DPString_t" },
		{ "SP_DPBuffer",     "SP_DPBuffer_t" },
		{ "SP_DPStringList", "SP_DPStringList_t" },
		{ "SP_DPInt32List",  "SP_DPInt32List_t" },
		{ "SP_DPUInt32List", "SP_DPUInt32List_t" },
		{ "SP_DPInt64List",  "SP_DPInt64List_t" },
		{ "SP_DPUInt64List", "SP_DPUInt64List_t" },

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

		{ "SP_DPChar",   "eTypeSP_DPChar" },
		{ "SP_DPInt16",  "eTypeSP_DPInt16" },
		{ "SP_DPUInt16", "eTypeSP_DPUInt16" },
		{ "SP_DPInt32",  "eTypeSP_DPInt32" },
		{ "SP_DPUInt32", "eTypeSP_DPUInt32" },
		{ "SP_DPInt64",  "eTypeSP_DPInt64" },
		{ "SP_DPUInt64", "eTypeSP_DPUInt64" },
		{ "SP_DPFloat",  "eTypeSP_DPFloat" },
		{ "SP_DPDouble", "eTypeSP_DPDouble" },
		{ "SP_DPString", "eTypeSP_DPString" },
		{ "SP_DPBuffer",     "eTypeSP_DPBuffer" },
		{ "SP_DPStringList", "eTypeSP_DPStringList" },
		{ "SP_DPInt32List",  "eTypeSP_DPInt32List" },
		{ "SP_DPUInt32List", "eTypeSP_DPUInt32List" },
		{ "SP_DPInt64List",  "eTypeSP_DPInt64List" },
		{ "SP_DPUInt64List", "eTypeSP_DPUInt64List" },

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

const char * SP_DPNameRender :: getParamName( const char * name, char * paramName, int size )
{
	snprintf( paramName, size, "%c%s", tolower( *name ), name + 1 );

	return paramName;
}

const char * SP_DPNameRender :: getStructBaseName( const char * type, char * structName, int size )
{
	static const char * builtinTypes [] = {
		"SP_DPChar",
		"SP_DPInt16",
		"SP_DPUInt16",
		"SP_DPInt32",
		"SP_DPUInt32",
		"SP_DPInt64",
		"SP_DPUInt64",
		"SP_DPFloat",
		"SP_DPDouble",
		"SP_DPString",
		"SP_DPBuffer",
		"SP_DPStringList",
		"SP_DPInt32List",
		"SP_DPUInt32List",
		"SP_DPInt64List",
		"SP_DPUInt64List",
		NULL
	};

	if( '*' == *type ) type++;

	int isBuiltinType = 0;

	for( int i = 0; ; ++i ) {
		const char * iter = builtinTypes[i];

		if( NULL == iter ) break;

		if( 0 == strcmp( type, iter ) ) {
			isBuiltinType = 1;
			break;
		}
	}

	if( isBuiltinType ) {
		snprintf( structName, size, "%s", type );
	} else {
		snprintf( structName, size, "%s%c%s", mPrefix, toupper( *type ), type + 1 );
	}

	return structName;
}

const char * SP_DPNameRender :: getStructFakeName( const char * name, char * structName, int size )
{
	snprintf( structName, size, "%s%c%s_Fake", mPrefix, toupper( *name ), name + 1 );

	return structName;
}

const char * SP_DPNameRender :: getMetaInfoName( const char * name, char * metaName, int size )
{
	snprintf( metaName, size, "g%s%c%sMetaInfo", mPrefix, toupper( *name ), name + 1 );

	return metaName;
}

const char * SP_DPNameRender :: getPickleName( const char * name, char * pickleName, int size )
{
	snprintf( pickleName, size, "%s%c%sPickle", mPrefix, toupper( *name ), name + 1 );

	return pickleName;
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

