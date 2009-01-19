/*
 * Copyright 2009 Stephen Liu
 * For license terms, see the file COPYING along with this library.
 */

#ifndef __spdpcode_hpp__
#define __spdpcode_hpp__

#include <stdio.h>

class SP_DPSyntaxTree;
class SP_DPSyntaxStruct;
class SP_DPSyntaxField;

class SP_DPNameRender;

class SP_DPCodeRender
{
public:
	SP_DPCodeRender( SP_DPNameRender * nameRender );
	~SP_DPCodeRender();

	void generateHeader( SP_DPSyntaxTree * syntaxTree, FILE * writer );

	void generateMetadata( SP_DPSyntaxTree * syntaxTree, FILE * writer );

private:

	void generateStruct( SP_DPSyntaxStruct * structure, FILE * writer );

	void generateField( SP_DPSyntaxField * field, FILE * writer );

	void generateMetaEnum( SP_DPSyntaxTree * syntaxTree, FILE * writer );

	void generateMetaField( SP_DPSyntaxStruct * structure, FILE * writer );

	void generateMetaInfo( SP_DPSyntaxTree * syntaxTree, FILE * writer );

private:
	SP_DPNameRender * mNameRender;
};

#endif

