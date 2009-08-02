/*
 * Copyright 2009 Stephen Liu
 * For license terms, see the file COPYING along with this library.
 */

#include <stdlib.h>

#include "spdpalloc.hpp"

#include "spdpmetainfo.hpp"
#include "spdpmetautils.hpp"

SP_DPAlloc :: SP_DPAlloc( SP_DPMetaInfo_t * metaInfo )
{
	mMetaInfo = metaInfo;
}

SP_DPAlloc :: ~SP_DPAlloc()
{
}

int SP_DPAlloc :: free( void * structure, int size, int type )
{
	if( NULL == structure ) return -1;

	char * base = (char*)structure;

	SP_DPMetaStruct_t * metaStruct = SP_DPMetaUtils::find( mMetaInfo, type );

	if( NULL == metaStruct ) return -1;

	if( metaStruct->mSize != size ) return -1;

	for( int i = 0; i < metaStruct->mFieldCount; i++ ) {
		SP_DPMetaField_t * field = metaStruct->mFieldList + i;

		// skip null pointer
		if( field->mIsPtr && ( field->mArraySize <= 0 )
				&& ( NULL == *(void**) ( base + field->mOffset ) ) ) continue;

		if( field->mType > eTypeSPDPUserDefine ) {
			if( field->mIsPtr ) {
				int referCount = SP_DPMetaUtils::getReferCount( structure, metaStruct, field );
				SP_DPMetaStruct_t * referStruct = SP_DPMetaUtils::find( mMetaInfo, field->mType );
				char * referBase = (char*)*(void**)(base + field->mOffset);

				for( int j = 0; j < referCount; j++ ) {
					free( referBase + ( j * referStruct->mSize ), field->mItemSize, field->mType );
				}

				::free( referBase );
			} else {
				free( base + field->mOffset, field->mItemSize, field->mType );
			}
		} else {
			if( field->mIsPtr ) {
				if( field->mArraySize <= 0 ) {
					::free( *(void**)( base + field->mOffset ) );
				}
			}
		}
	}

	return 0;
}

