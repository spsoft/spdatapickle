/*
 * Copyright 2009 Stephen Liu
 * For license terms, see the file COPYING along with this library.
 */

#ifndef __spdpsyntax_hpp__
#define __spdpsyntax_hpp__

#include <vector>

class SP_DPSyntaxNode
{
public:
	SP_DPSyntaxNode();
	virtual ~SP_DPSyntaxNode();

	void setName( const char * name );
	const char * getName() const;

	const char * getInitalName() const;

private:
	char mName[ 128 ];
	char mInitalName[ 128 ];
};

class SP_DPSyntaxField : public SP_DPSyntaxNode
{
public:
	SP_DPSyntaxField();
	~SP_DPSyntaxField();

	void setId( int id );
	int getId();

	void setType( const char * type );
	const char * getType() const;

	int isPtr() const;

	void setArraySize( int arraySize );
	int getArraySize() const;

	void setReferTo( const char * referTo );
	const char * getReferTo() const;

	void setRequired( int isRequired );
	int  isRequired();

	void setReferred( int isReferred );
	int  isReferred();

private:
	int mId;
	char mType[ 128 ];
	int mArraySize;
	char mReferTo[ 128 ];
	int mIsRequired;
	int mIsReferred;
};

typedef std::vector<SP_DPSyntaxField> SP_DPSyntaxFieldVector;

class SP_DPSyntaxStruct : public SP_DPSyntaxNode
{
public:
	SP_DPSyntaxStruct();
	~SP_DPSyntaxStruct();

	SP_DPSyntaxFieldVector * getFieldList();

	SP_DPSyntaxField * findField( const char * name );

private:
	SP_DPSyntaxFieldVector mFieldList;
};

typedef std::vector<SP_DPSyntaxStruct> SP_DPSyntaxStructVector;

class SP_DPSyntaxTree : public SP_DPSyntaxNode
{
public:
	SP_DPSyntaxTree();
	~SP_DPSyntaxTree();

	int isBuiltin();

	void setDefineFile( const char * defineFile );
	const char * getDefineFile() const;

	void setPrefix( const char * prefix );
	const char * getPrefix() const;

	SP_DPSyntaxStructVector * getStructList();

private:
	char mDefineFile[ 128 ];
	char mPrefix[ 128 ];
	SP_DPSyntaxStructVector mStructList;
};

#endif

