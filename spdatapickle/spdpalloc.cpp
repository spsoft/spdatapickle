/*
 * Copyright 2009 Stephen Liu
 * For license terms, see the file COPYING along with this library.
 */

#include <stdlib.h>
#include <string.h>

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

		if( field->mType > eTypeSPDPBuiltin ) {
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

int SP_DPAlloc :: deepCopy( const void * src, int srcSize, int type, void * dest, int destSize )
{
	if( NULL == src || NULL == dest ) return -1;

	const char * srcBase = (char*)src;
	char * destBase = (char*)dest;

	SP_DPMetaStruct_t * metaStruct = SP_DPMetaUtils::find( mMetaInfo, type );

	if( NULL == metaStruct ) return -1;

	if( ( metaStruct->mSize != srcSize ) || ( srcSize != destSize ) ) return -1;

	int ret = 0, i = 0;

	memset( dest, 0, destSize );

	// copy the non-ptr base type, maybe include referCount
	for( i = 0; 0 == ret && i < metaStruct->mFieldCount; i++ ) {
		SP_DPMetaField_t * field = metaStruct->mFieldList + i;

		if( field->mType < eTypeSPDPBuiltin && 0 == field->mIsPtr ) {
			memcpy( destBase + field->mOffset, srcBase + field->mOffset, field->mFieldSize );
		}
	}

	for( i = 0; 0 == ret && i < metaStruct->mFieldCount; i++ ) {
		SP_DPMetaField_t * field = metaStruct->mFieldList + i;

		// these fields had been copy, skip
		if( field->mType < eTypeSPDPBuiltin && 0 == field->mIsPtr ) continue;

		if( field->mType > eTypeSPDPBuiltin ) {
			if( field->mIsPtr ) {
				int referCount = SP_DPMetaUtils::getReferCount( src, metaStruct, field );
				SP_DPMetaStruct_t * referStruct = SP_DPMetaUtils::find( mMetaInfo, field->mType );

				char * destPtr = (char*)calloc( field->mItemSize, referCount );
				*(void**)(destBase + field->mOffset) = destPtr;

				const char * srcPtr = *(char**)( srcBase + field->mOffset );

				for( int j = 0; 0 == ret && j < referCount; j++ ) {
					ret = deepCopy( srcPtr + ( j * referStruct->mSize ), field->mItemSize,
							field->mType, destPtr + ( j * referStruct->mSize ), field->mItemSize );
				}
			} else {
				ret = deepCopy( srcBase + field->mOffset, field->mFieldSize, field->mType,
						destBase + field->mOffset, field->mFieldSize );
			}
		} else {
			ret = copyBaseType( src, srcSize, metaStruct, field, dest, destSize );
		}
	}

	return 0;
}

int SP_DPAlloc :: copyBaseType( const void * src, int srcSize, SP_DPMetaStruct_t * metaStruct,
			SP_DPMetaField_t * field, void * dest, int destSize )
{
	int ret = 0;

	const char * srcBase = (char*)src;
	const char * srcPtr = srcBase + field->mOffset;

	char * destBase = (char*)dest;
	char * destPtr = destBase + field->mOffset;

	if( field->mIsPtr ) {
		if( eTypeSPDPChar == field->mType ) {
			if( '\0' == field->mReferTo[0] ) {
				if( field->mArraySize > 0 ) {
					memcpy( destPtr, srcPtr, field->mArraySize );
				} else {
					srcPtr = *(char**)( srcBase + field->mOffset );
					if( NULL != srcPtr ) {
						*(char**)( destBase + field->mOffset ) = strdup( srcPtr );
					}
				}
			} else {
				int len = field->mArraySize;
				if( len <= 0 ) {
					len = SP_DPMetaUtils::getReferCount( src, metaStruct, field );
					destPtr = (char*)calloc( 1, len + 1 );
					*(void**)( destBase + field->mOffset ) = destPtr;

					srcPtr = *(char**)( srcBase + field->mOffset );
				}
				if( NULL != srcPtr ) memcpy( destPtr, srcPtr, len );
			}
		} else {
			int referCount = SP_DPMetaUtils::getReferCount( src, metaStruct, field );
			if( field->mArraySize <= 0 ) {
				destPtr = (char*)calloc( field->mItemSize, referCount );
				*(void**)( destBase + field->mOffset ) = destPtr;

				srcPtr = *(char**)( srcBase + field->mOffset );
			}

			if( NULL != srcPtr ) memcpy( destPtr, srcPtr, field->mItemSize * referCount );
		}
	}

	return ret;
}

