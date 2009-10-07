/*
 * Copyright 2009 Stephen Liu
 * For license terms, see the file COPYING along with this library.
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "spdpmetautils.hpp"
#include "spdpmetainfo.hpp"

void SP_DPMetaUtils :: dump( SP_DPMetaInfo_t * metaInfo )
{
	char tmp[ 128 ] = { 0 };

	printf( "prefix %s, name %s\n", metaInfo->mPrefix, metaInfo->mFileName );
	printf( "struct.count %d\n", metaInfo->mStructCount );

	for( int i = 0; i < metaInfo->mStructCount; i++ ) {
		printf( "\n" );

		SP_DPMetaStruct_t * st = metaInfo->mStructList + i;
		printf( "struct name %s, id %d, size %d, field.count %d\n",
				st->mName, st->mId, st->mSize, st->mFieldCount );

		for( int j = 0; j < st->mFieldCount; j++ ) {
			SP_DPMetaField_t * field = st->mFieldList + j;

			snprintf( tmp, sizeof( tmp ), "%s,", field->mName );

			//printf( "field name %-15s offset %d, field.size %d, item.size %d, type %d, isptr %d, array.size %d, referto [%s]\n",
				//tmp, field->mOffset, field->mFieldSize, field->mItemSize, field->mType, field->mIsPtr,
				//field->mArraySize, field->mReferTo );

			printf( "field name %-15s id %d, offset %d, type %d, isptr %d, isreq %d, array.size %d, referto [%s], is.referred %d\n",
				tmp, field->mId, field->mOffset, field->mFieldSize, field->mIsPtr, field->mIsRequired,
				field->mArraySize, field->mReferTo, field->mIsReferred );
		}
	}

	printf( "\n" );
}

SP_DPMetaStruct_t * SP_DPMetaUtils :: find( SP_DPMetaInfo_t * metaInfo, int type )
{
	SP_DPMetaStruct_t * ret = NULL;

	for( int i = 0; i < metaInfo->mStructCount; i++ ) {
		SP_DPMetaStruct_t * st = metaInfo->mStructList + i;

		if( st->mId == type ) {
			ret = st;
			break;
		}
	}

	return ret;
}

SP_DPMetaStruct_t * SP_DPMetaUtils :: find( SP_DPMetaInfo_t * metaInfo, const char * name )
{
	SP_DPMetaStruct_t * ret = NULL;

	for( int i = 0; i < metaInfo->mStructCount; i++ ) {
		SP_DPMetaStruct_t * st = metaInfo->mStructList + i;

		if( 0 == strcmp( st->mName, name ) ) {
			ret = st;
			break;
		}
	}

	return ret;
}

SP_DPMetaField_t * SP_DPMetaUtils :: find( SP_DPMetaStruct_t * metaStruct, const char * name )
{
	SP_DPMetaField_t * ret = NULL;

	for( int i = 0; i < metaStruct->mFieldCount; i++ ) {
		SP_DPMetaField_t * field = metaStruct->mFieldList + i;

		if( 0 == strcmp( field->mName, name ) ) {
			ret = field;
			break;
		}
	}

	return ret;
}

int SP_DPMetaUtils :: getReferCount( void * structure,
		SP_DPMetaStruct_t * metaStruct, SP_DPMetaField_t * field )
{
	int ret = -1;

	SP_DPMetaField_t * referTo = SP_DPMetaUtils::find( metaStruct, field->mReferTo );

	if( NULL == referTo ) return -1;

	char * base = (char*)structure;
	void * ptr = base + referTo->mOffset;

	switch( referTo->mType ) {
		case eTypeSPDPChar:
			ret = *(char*)ptr;
			break;
		case eTypeSPDPInt16:
			ret = *(short*)ptr;
			break;
		case eTypeSPDPUint16:
			ret = *(unsigned short*)ptr;
			break;
		case eTypeSPDPInt32:
			ret = *(int*)ptr;
			break;
		default:
			ret = -1;
			break;
	}

	return ret;
}

int SP_DPMetaUtils :: setReferCount( void * structure, SP_DPMetaStruct_t * metaStruct,
		SP_DPMetaField_t * field, int referCount )
{
	int ret = 0;

	SP_DPMetaField_t * referTo = SP_DPMetaUtils::find( metaStruct, field->mReferTo );

	if( NULL == referTo ) return -1;

	char * base = (char*)structure;
	void * ptr = base + referTo->mOffset;

	switch( referTo->mType ) {
		case eTypeSPDPChar:
			*(char*)ptr = referCount;
			break;
		case eTypeSPDPInt16:
			*(short*)ptr = referCount;
			break;
		case eTypeSPDPUint16:
			*(unsigned short*)ptr = referCount;
			break;
		case eTypeSPDPInt32:
			*(int*)ptr = referCount;
			break;
		case eTypeSPDPUint32:
			*(unsigned int*)ptr = referCount;
			break;
		case eTypeSPDPInt64:
			*(long long*)ptr = referCount;
			break;
		case eTypeSPDPUint64:
			*(unsigned long long*)ptr = referCount;
			break;
		default:
			ret = -1;
			break;
	}

	return ret;
}

