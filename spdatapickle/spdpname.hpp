/*
 * Copyright 2009 Stephen Liu
 * For license terms, see the file COPYING along with this library.
 */

#ifndef __spdpname_hpp__
#define __spdpname_hpp__

class SP_DPSyntaxTree;
class SP_DPSyntaxField;

class SP_DPNameRender {
public:
	SP_DPNameRender( const char * prefix );
	virtual ~SP_DPNameRender();

	virtual const char * getFileName( const char * filename, char * name, int size );

	virtual const char * getTypeFullName( SP_DPSyntaxField * field, char * name, int size );

	virtual const char * getTypeName( const char * type, char * name, int size );

	virtual const char * getTypeRawName( const char * type, char * name, int size );

	virtual const char * getTypeEnum( const char * type, char * name, int size );

	virtual const char * getFieldName( const char * name, char * fieldName, int size );

	virtual const char * getStructBaseName( const char * name, char * structName, int size );

public:

	static char * toLower ( register char *s );

	static char * toUpper( register char *s );

private:
	char mPrefix[ 128 ];
};

#endif

