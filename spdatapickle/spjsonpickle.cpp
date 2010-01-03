/*
 * Copyright 2009 Stephen Liu
 * For license terms, see the file COPYING along with this library.
 */

#include <string.h>
#include <stdlib.h>
#include <assert.h>

#include "spjsonpickle.hpp"
#include "spdpmetainfo.hpp"
#include "spdpmetautils.hpp"
#include "spdpbase64.hpp"

#include "spxml/spxmlutils.hpp"

#include "spjson/spjsonutils.hpp"
#include "spjson/spjsoncodec.hpp"
#include "spjson/spjsondomparser.hpp"
#include "spjson/spjsonhandle.hpp"
#include "spjson/spjsonnode.hpp"

SP_JsonPickle :: SP_JsonPickle( SP_DPMetaInfo_t * metaInfo )
	: SP_DataPickle( metaInfo )
{
}

SP_JsonPickle :: ~SP_JsonPickle()
{
}

int SP_JsonPickle :: pickle( void * structure, int size, int type, SP_XmlStringBuffer * buffer )
{
	SP_JsonStringBuffer tmp;

	int ret = pickle( structure, size, type, &tmp );

	if( NULL != tmp.getBuffer() ) buffer->append( tmp.getBuffer(), tmp.getSize() );

	return ret;
}

int SP_JsonPickle :: pickle( void * structure, int size, int type, SP_JsonStringBuffer * buffer )
{
	if( NULL == structure ) return -1;

	char * base = (char*)structure;

	SP_DPMetaStruct_t * metaStruct = SP_DPMetaUtils::find( mMetaInfo, type );

	if( NULL == metaStruct ) return -1;

	if( metaStruct->mSize != size ) return -1;

	buffer->append( "{" );

	int isFirst = 1;

	for( int i = 0; i < metaStruct->mFieldCount; i++ ) {
		SP_DPMetaField_t * field = metaStruct->mFieldList + i;

		// skip null pointer
		if( field->mIsPtr && ( field->mArraySize <= 0 )
				&& ( NULL == *(void**) ( base + field->mOffset ) ) ) continue;

		if( isFirst ) {
			isFirst = 0;
		} else {
			buffer->append( ",\n" );
		}

		buffer->append( '"' );
		buffer->append( field->mName );
		buffer->append( "\": " );

		if( field->mType > eTypeSPDPBuiltin ) {
			buffer->append( "\n" );
			if( field->mIsPtr ) {

				buffer->append( "[" );

				int referCount = SP_DPMetaUtils::getReferCount( structure, metaStruct, field );
				SP_DPMetaStruct_t * referStruct = SP_DPMetaUtils::find( mMetaInfo, field->mType );
				char * referBase = *(char**)(base + field->mOffset);

				for( int j = 0; j < referCount; j++ ) {
					if( j > 0 ) buffer->append( "," );
					pickle( referBase + ( j * referStruct->mSize ), field->mItemSize, field->mType, buffer );
				}

				buffer->append( "]\n" );
			} else {
				pickle( base + field->mOffset, field->mItemSize, field->mType, buffer );
			}
		} else {
			pickleBaseType( structure, metaStruct, field, buffer );
		}
	}

	buffer->append( "}\n" );

	return 0;
}

int SP_JsonPickle :: pickleBaseType( void * structure, SP_DPMetaStruct_t * metaStruct,
		SP_DPMetaField_t * field, SP_JsonStringBuffer * buffer )
{
	int ret = 0;

	char * base = (char*)structure;
	void * ptr = base + field->mOffset;

	if( field->mIsPtr ) {
		if( field->mArraySize <= 0 ) ptr = *(void**)ptr;

		if( eTypeSPDPChar == field->mType ) {
			buffer->append( '"' );

			if( '\0' == field->mReferTo[0] ) {
				SP_JsonCodec::encode( (char*)ptr, buffer );
			} else {
				int referCount = SP_DPMetaUtils::getReferCount( structure, metaStruct, field );
				SP_DPBase64EncodedBuffer b64buffer( ptr, referCount );
				buffer->append( b64buffer.getBuffer(), b64buffer.getLength() );
			}
			buffer->append( '"' );
		} else {
			int referCount = SP_DPMetaUtils::getReferCount( structure, metaStruct, field );
			char * referBase = (char*)ptr;

			buffer->append( "[" );

			for( int i = 0; i < referCount; i++ ) {
				if( i > 0 ) buffer->append( ',' );
				pickleBasePtr( referBase + ( i * field->mItemSize ), field->mType, buffer );
			}

			buffer->append( "]" );
		}
	} else {
		pickleBasePtr( ptr, field->mType, buffer );
	}

	return ret;
}

int SP_JsonPickle :: pickleBasePtr( void * ptr, int type, SP_JsonStringBuffer * buffer )
{
	int ret = 0;

	char tmp[ 128 ] = { 0 };

	switch( type ) {
		case eTypeSPDPChar:
			snprintf( tmp, sizeof( tmp ), "%d", *(char*)ptr );
			break;
		case eTypeSPDPInt16:
			snprintf( tmp, sizeof( tmp ), "%d", *(short*)ptr );
			break;
		case eTypeSPDPUint16:
			snprintf( tmp, sizeof( tmp ), "%u", *(unsigned short*)ptr );
			break;
		case eTypeSPDPInt32:
			snprintf( tmp, sizeof( tmp ), "%d", *(int*)ptr );
			break;
		case eTypeSPDPUint32:
			snprintf( tmp, sizeof( tmp ), "%u", *(unsigned int*)ptr );
			break;
		case eTypeSPDPInt64:
			snprintf( tmp, sizeof( tmp ), "%lld", *(int64_t*)ptr );
			break;
		case eTypeSPDPUint64:
			snprintf( tmp, sizeof( tmp ), "%llu", *(uint64_t*)ptr );
			break;
		case eTypeSPDPFloat:
			snprintf( tmp, sizeof( tmp ), "%f", *(float*)ptr );
			break;
		case eTypeSPDPDouble:
			snprintf( tmp, sizeof( tmp ), "%f", *(double*)ptr );
			break;
		default:
			ret = -1;
			break;
	}

	if( 0 == ret ) buffer->append( tmp );

	return ret;
}

int SP_JsonPickle :: unpickle( const char * text, int len, int type, void * structure, int size )
{
	SP_JsonDomParser parser;
	parser.append( text, len );

	if( NULL != parser.getError() ) {
		return -1;
	}

	if( NULL == parser.getValue() ) {
		return -1;
	}

	return unpickle( parser.getValue(), type, structure, size );
}

int SP_JsonPickle :: unpickle( SP_JsonNode * root, int type, void * structure, int size )
{
	int ret = 0, i = 0;

	char * base = (char*)structure;

	SP_DPMetaStruct_t * metaStruct = SP_DPMetaUtils::find( mMetaInfo, type );

	if( NULL == metaStruct || NULL == root ) return -1;

	if( metaStruct->mSize != size ) return -1;

	if( NULL == root ) return -1;

	SP_JsonHandle rootHandle( root );

	// unpickle the non-ptr base type, maybe include referCount
	for( i = 0; 0 == ret && i < metaStruct->mFieldCount; i++ ) {
		SP_DPMetaField_t * field = metaStruct->mFieldList + i;

		SP_JsonHandle fieldHandle = rootHandle.getChild( field->mName );

		if( field->mType < eTypeSPDPBuiltin && 0 == field->mIsPtr ) {
			if( NULL == fieldHandle.toNode() ) {
				if( field->mIsRequired ) ret = -1;
				continue;
			}

			unpickleBaseType( fieldHandle.toNode(), metaStruct, field, structure );
		}
	}

	for( i = 0; 0 == ret && i < metaStruct->mFieldCount; i++ ) {
		SP_DPMetaField_t * field = metaStruct->mFieldList + i;

		// these fields had been unpickled, skip
		if( field->mType < eTypeSPDPBuiltin && 0 == field->mIsPtr ) continue;

		SP_JsonHandle fieldHandle = rootHandle.getChild( field->mName );

		if( NULL == fieldHandle.toNode() ) {
			if( field->mIsRequired && ( 0 == field->mIsPtr || field->mArraySize > 0 ) ) {
				ret = -1;
			}

			SP_DPMetaUtils::setReferCount( structure, metaStruct, field, 0 );
			continue;
		}

		if( field->mType > eTypeSPDPBuiltin ) {
			if( field->mIsPtr ) {
				int referCount = SP_DPMetaUtils::getReferCount( structure, metaStruct, field );
				SP_DPMetaStruct_t * referStruct = SP_DPMetaUtils::find( mMetaInfo, field->mType );
				char * referBase = (char*)calloc( field->mItemSize, referCount );
				*(void**)(base + field->mOffset) = referBase;

				for( int j = 0; 0 == ret && j < referCount; j++ ) {
					SP_JsonHandle itemHandle = fieldHandle.getChild( j );

					ret = unpickle( itemHandle.toNode(), field->mType,
							referBase + ( j * referStruct->mSize ), field->mItemSize );
				}
			} else {
				ret = unpickle( fieldHandle.toNode(), field->mType, base + field->mOffset, field->mItemSize );
			}
		} else {
			ret = unpickleBaseType( fieldHandle.toNode(), metaStruct, field, structure );
		}
	}

	return ret;
}

int SP_JsonPickle :: unpickleBaseType( SP_JsonNode * root, SP_DPMetaStruct_t * metaStruct,
		SP_DPMetaField_t * field, void * structure )
{
	int ret = 0;

	char * base = (char*)structure;
	void * ptr = base + field->mOffset;

	if( field->mIsPtr ) {
		if( eTypeSPDPChar == field->mType ) {
			SP_JsonHandle nodeHandle( root );
			const char * cdata = nodeHandle.toString()->getValue();

			if( '\0' == field->mReferTo[0] ) {
				if( field->mArraySize > 0 ) {
					strncpy( (char*)ptr, cdata, field->mArraySize );
					((char*)ptr)[ field->mArraySize - 1 ] = '\0';
				} else {
					*(char**)(base + field->mOffset ) = strdup( cdata );
				}
			} else {
				SP_DPBase64DecodedBuffer b64buffer( cdata, strlen( cdata ) );

				int len = field->mArraySize;
				if( len <= 0 ) {
					len = b64buffer.getLength();
					ptr = calloc( 1, len + 1 );
					*(void**)(base + field->mOffset ) = ptr;
				}
				len = len > (int)b64buffer.getLength() ? (int)b64buffer.getLength() : len;
				memcpy( ptr, b64buffer.getBuffer(), len );
			}
		} else {
			int referCount = SP_DPMetaUtils::getReferCount( structure, metaStruct, field );
			if( field->mArraySize <= 0 ) {
				ptr = calloc( field->mItemSize, referCount );
				*(void**)(base + field->mOffset ) = ptr;
			}

			char * referBase = (char*)ptr;

			SP_JsonHandle rootHandle( root );

			for( int i = 0; i < referCount; i++ ) {
				SP_JsonHandle itemHandle = rootHandle.getChild( i );
				unpickleBasePtr( itemHandle.toNode(), field->mType, referBase + ( i * field->mItemSize ) );
			}
		}
	} else {
		unpickleBasePtr( root, field->mType, ptr );
	}

	return ret;
}

int SP_JsonPickle :: unpickleBasePtr( SP_JsonNode * node, int type, void * ptr )
{
	int ret = 0;

	SP_JsonHandle nodeHandle( node );

	switch( type ) {
		case eTypeSPDPChar:
			*(char*)ptr = (char)nodeHandle.toInt()->getValue();
			break;
		case eTypeSPDPInt16:
			*(short*)ptr = (short)nodeHandle.toInt()->getValue();
			break;
		case eTypeSPDPUint16:
			*(unsigned short*)ptr = (unsigned short)nodeHandle.toInt()->getValue();
			break;
		case eTypeSPDPInt32:
			*(int*)ptr = (int)nodeHandle.toInt()->getValue();
			break;
		case eTypeSPDPUint32:
			*(unsigned int*)ptr = (unsigned int)nodeHandle.toInt()->getValue();
			break;
		case eTypeSPDPInt64:
			*(int64_t*)ptr = nodeHandle.toInt()->getValue();
			break;
		case eTypeSPDPUint64:
			*(uint64_t*)ptr = nodeHandle.toInt()->getValue();
			break;
		case eTypeSPDPFloat:
			*(float*)ptr = (float)nodeHandle.toDouble()->getValue();
			break;
		case eTypeSPDPDouble:
			*(double*)ptr = nodeHandle.toDouble()->getValue();
			break;
		default:
			ret = -1;
			break;
	}

	return ret;
}

