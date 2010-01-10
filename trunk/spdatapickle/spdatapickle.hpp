/*
 * Copyright 2009 Stephen Liu
 * For license terms, see the file COPYING along with this library.
 */

#ifndef __spdatapickle_hpp__
#define __spdatapickle_hpp__

class SP_XmlStringBuffer;

typedef struct tagSP_DPMetaInfo   SP_DPMetaInfo_t;
typedef struct tagSP_DPMetaField  SP_DPMetaField_t;
typedef struct tagSP_DPMetaStruct SP_DPMetaStruct_t;

class SP_DataPickle
{
public:
	enum { eXml = 0, eXmlRpc = 1, eJson = 2, eProtoBuf = 3 };

public:
	SP_DataPickle( SP_DPMetaInfo_t * metaInfo );
	virtual ~SP_DataPickle();

	virtual int pickle( void * structure, int size, int type, SP_XmlStringBuffer * buffer ) = 0;

	virtual int unpickle( const char * text, int len, int type, void * structure, int size ) = 0;

protected:

	SP_DPMetaInfo_t * mMetaInfo;
};

#endif

