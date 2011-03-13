/*
 * Copyright 2010 Stephen Liu
 * For license terms, see the file COPYING along with this library.
 */

#pragma once

#include <stdio.h>

class SP_DPSyntaxTree;
class SP_DPSyntaxStruct;
class SP_DPSyntaxField;

class SP_DPNameRender;

class SP_DPClassCodeRender
{
public:
	SP_DPClassCodeRender( SP_DPNameRender *nameRender );
	~SP_DPClassCodeRender();

	void generateHeader( SP_DPSyntaxTree * syntaxTree, FILE * writer );

	void generateCpp( SP_DPSyntaxTree * syntaxTree, FILE * writer );

private:
	void generateDecl( SP_DPSyntaxStruct * structure, FILE * writer );

	void generateAccessorDecl( SP_DPSyntaxField * field, FILE * writer );

	void generateFake( const char * pickleName,
			SP_DPSyntaxTree * syntaxTree,
			SP_DPSyntaxStruct * structure, FILE * writer );

	void generateImpl( const char * pickleName,
			SP_DPSyntaxStruct * structure, FILE * writer );

	void generateEval( const char * pickleName,
			SP_DPSyntaxStruct * structure, FILE * writer );

	void generateAccessorImpl( const char * className,
			SP_DPSyntaxStruct * structure,
			SP_DPSyntaxField * field, FILE * writer );

private:
	SP_DPNameRender * mNameRender;
};

