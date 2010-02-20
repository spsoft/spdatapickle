/*
 * Copyright 2009 Stephen Liu
 * For license terms, see the file COPYING along with this library.
 */

#ifndef __spjsonpickle_hpp__
#define __spjsonpickle_hpp__

#include "spdatapickle.hpp"

class SP_JsonStringBuffer;
class SP_JsonNode;

class SP_JsonPickle : public SP_DataPickle
{
public:
	SP_JsonPickle( SP_DPMetaInfo_t * metaInfo );
	~SP_JsonPickle();

	virtual int pickle( void * structure, int size, int type, SP_XmlStringBuffer * buffer );

	virtual int unpickle( const char * text, int len, int type, void * structure, int size );

	int pickle( void * structure, int size, int type, SP_JsonStringBuffer * buffer );

	int unpickle( SP_JsonNode * root, int type, void * structure, int size );

private:

	int unpickleBaseType( SP_JsonNode * root, SP_DPMetaStruct_t * metaStruct,
			SP_DPMetaField_t * field, void * structure );

	int unpickleBasePtr( SP_JsonNode * node, int type, void * ptr );

private:

	int pickleBaseType( void * structure, SP_DPMetaStruct_t * metaStruct,
			SP_DPMetaField_t * field, SP_JsonStringBuffer * buffer );

	int pickleBasePtr( void * ptr, int type, SP_JsonStringBuffer * buffer );
};

#endif

