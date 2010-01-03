/*
 * Copyright 2009 Stephen Liu
 * For license terms, see the file COPYING along with this library.
 */

#ifndef __spdpmetautils_hpp__
#define __spdpmetautils_hpp__

typedef struct tagSP_DPMetaInfo   SP_DPMetaInfo_t;
typedef struct tagSP_DPMetaStruct SP_DPMetaStruct_t;
typedef struct tagSP_DPMetaField  SP_DPMetaField_t;

class SP_DPMetaUtils
{
public:
	static void dump( SP_DPMetaInfo_t * metaInfo );

	static SP_DPMetaStruct_t * find( SP_DPMetaInfo_t * metaInfo, int type );

	static SP_DPMetaStruct_t * find( SP_DPMetaInfo_t * metaInfo, const char * name );

	static SP_DPMetaField_t * find( SP_DPMetaStruct_t * metaStruct, const char * name );

	static int getReferCount( const void * structure,
		SP_DPMetaStruct_t * metaStruct, SP_DPMetaField_t * field );

	static int setReferCount( void * structure,
		SP_DPMetaStruct_t * metaStruct, SP_DPMetaField_t * field, int referCount );

private:
	SP_DPMetaUtils();
};

#endif

