/*
 * Copyright 2009 Stephen Liu
 * For license terms, see the file COPYING along with this library.
 */

#ifndef __spxmlpickle_hpp__
#define __spxmlpickle_hpp__

class SP_XmlStringBuffer;
class SP_XmlElementNode;

typedef struct tagSP_DPMetaInfo   SP_DPMetaInfo_t;
typedef struct tagSP_DPMetaField  SP_DPMetaField_t;
typedef struct tagSP_DPMetaStruct SP_DPMetaStruct_t;

class SP_XmlPickle
{
public:
	SP_XmlPickle( SP_DPMetaInfo_t * metaInfo );
	~SP_XmlPickle();

	int pickle( void * structure, int size, int type, SP_XmlStringBuffer * buffer );

	int unpickle( const char * xml, int len, int type, void * structure, int size );

private:

	int unpickle( SP_XmlElementNode * root, int type, void * structure, int size );

	int unpickleBaseType( SP_XmlElementNode * root, SP_DPMetaStruct_t * metaStruct,
			SP_DPMetaField_t * field, void * structure );

	int unpickleBasePtr( SP_XmlElementNode * node, int type, void * ptr );

private:

	int pickleBaseType( void * structure, SP_DPMetaStruct_t * metaStruct,
			SP_DPMetaField_t * field, SP_XmlStringBuffer * buffer );

	int pickleBasePtr( void * ptr, int type, SP_XmlStringBuffer * buffer );

	const char * getTypeTag( int type );

private:
	SP_DPMetaInfo_t * mMetaInfo;
};

#endif

