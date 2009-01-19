/*
 * Copyright 2009 Stephen Liu
 * For license terms, see the file COPYING along with this library.
 */

#ifndef __spdpxml_hpp__
#define __spdpxml_hpp__

class SP_DPSyntaxTree;
class SP_DPSyntaxField;
class SP_DPSyntaxStruct;

class SP_XmlNode;

class SP_DPXmlUtils
{
public:
	static int parse( const char * xmlFile, SP_DPSyntaxTree * syntaxTree );

private:
	static int parseField( SP_XmlNode * xmlNode, SP_DPSyntaxField * field );

	static int parseStruct( SP_XmlNode * xmlNode, SP_DPSyntaxStruct * structure );

private:
	SP_DPXmlUtils();
};

#endif

