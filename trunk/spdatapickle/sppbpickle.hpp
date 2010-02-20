/*
 * Copyright 2009 Stephen Liu
 * For license terms, see the file COPYING along with this library.
 */

#ifndef __sppbpickle_hpp__
#define __sppbpickle_hpp__

#include "spdatapickle.hpp"

class SP_ProtoBufEncoder;
class SP_ProtoBufDecoder;

class SP_ProtoBufPickle : public SP_DataPickle
{
public:
	SP_ProtoBufPickle( SP_DPMetaInfo_t * metaInfo );
	~SP_ProtoBufPickle();

	virtual int pickle( void * structure, int size, int type, SP_XmlStringBuffer * buffer );

	virtual int unpickle( const char * data, int len, int type, void * structure, int size );

	int pickle( void * structure, int size, int type, SP_ProtoBufEncoder * buffer );

	int unpickle( SP_ProtoBufDecoder * decoder, int type, void * structure, int size );

private:

	int unpickleBaseType( void * root, SP_DPMetaStruct_t * metaStruct,
			SP_DPMetaField_t * field, void * structure );

	int unpickleBaseArray( void * node, int type, void * ptr, int size );

	int unpickleBasePtr( void * node, int type, void * ptr );

private:

	int pickleBaseType( void * structure, SP_DPMetaStruct_t * metaStruct,
			SP_DPMetaField_t * field, SP_ProtoBufEncoder * encoder );

	int pickleBaseArray( void * ptr, int size, int type,
		int fieldId, SP_ProtoBufEncoder * encoder );

	int pickleBasePtr( void * ptr, int type, int fieldId, SP_ProtoBufEncoder * encoder );
};

#endif

