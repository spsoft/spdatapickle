/*
 * Copyright 2009 Stephen Liu
 * For license terms, see the file COPYING along with this library.
 */

#ifndef __spxmlrpcpickle_hpp__
#define __spxmlrpcpickle_hpp__

#include "spdatapickle.hpp"

class SP_XmlStringBuffer;
class SP_XmlElementNode;

class SP_XmlRpcPickle : public SP_DataPickle
{
public:
	SP_XmlRpcPickle( SP_DPMetaInfo_t * metaInfo );
	~SP_XmlRpcPickle();

	virtual int pickle( void * structure, int size, int type, SP_XmlStringBuffer * buffer );

	virtual int unpickle( const char * xml, int len, int type, void * structure, int size );

	int unpickle( SP_XmlElementNode * root, int type, void * structure, int size );

private:

	int unpickleBaseType( SP_XmlElementNode * root, SP_DPMetaStruct_t * metaStruct,
			SP_DPMetaField_t * field, void * structure );

	int unpickleBasePtr( SP_XmlElementNode * node, int type, void * ptr );

	static SP_XmlElementNode * findStructMember( SP_XmlElementNode * root, const char * name );

	static SP_XmlElementNode * findArrayMember( SP_XmlElementNode * root, int index );

private:

	virtual int realPickle( void * structure, int size, int type, SP_XmlStringBuffer * buffer );

	int pickleBaseType( void * structure, SP_DPMetaStruct_t * metaStruct,
			SP_DPMetaField_t * field, SP_XmlStringBuffer * buffer );

	int pickleBasePtr( void * ptr, int type, SP_XmlStringBuffer * buffer );

	const char * getTypeTag( int type );
};

#endif

