/*
 * Copyright 2009 Stephen Liu
 * For license terms, see the file COPYING along with this library.
 */

#ifndef __spdpalloc_hpp__
#define __spdpalloc_hpp__

typedef struct tagSP_DPMetaInfo SP_DPMetaInfo_t;

class SP_DPAlloc
{
public:
	SP_DPAlloc( SP_DPMetaInfo_t * metaInfo );
	~SP_DPAlloc();

	int free( void * structure, int size, int type );

private:
	SP_DPMetaInfo_t * mMetaInfo;
};

#endif

