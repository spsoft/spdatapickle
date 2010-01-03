/*
 * Copyright 2009 Stephen Liu
 * For license terms, see the file COPYING along with this library.
 */

#include <string.h>
#include <stdlib.h>
#include <assert.h>

#include "spxmlpickle.hpp"
#include "spdpmetainfo.hpp"
#include "spdpmetautils.hpp"
#include "spdpbase64.hpp"

#include "spxml/spxmlutils.hpp"
#include "spxml/spxmlcodec.hpp"
#include "spxml/spdomparser.hpp"
#include "spxml/spxmlhandle.hpp"
#include "spxml/spxmlnode.hpp"

#include "spjson/spjsonport.hpp"

SP_XmlPickle :: SP_XmlPickle( SP_DPMetaInfo_t * metaInfo )
	: SP_DataPickle( metaInfo )
{
}

SP_XmlPickle :: ~SP_XmlPickle()
{
}

int SP_XmlPickle :: pickle( void * structure, int size, int type, SP_XmlStringBuffer * buffer )
{
	SP_DPMetaStruct_t * metaStruct = SP_DPMetaUtils::find( mMetaInfo, type );

	if( NULL == metaStruct ) return -1;

	int ret = 0;

	buffer->append( '<' );
	buffer->append( metaStruct->mName );
	buffer->append( ">\n" );

	ret = realPickle( structure, size, type, buffer );

	buffer->append( "</" );
	buffer->append( metaStruct->mName );
	buffer->append( ">\n" );

	return ret;
}

int SP_XmlPickle :: realPickle( void * structure, int size, int type, SP_XmlStringBuffer * buffer )
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

		buffer->append( '<' );
		buffer->append( field->mName );
		buffer->append( ">" );

		if( field->mType > eTypeSPDPBuiltin ) {
			buffer->append( "\n" );
			if( field->mIsPtr ) {
				int referCount = SP_DPMetaUtils::getReferCount( structure, metaStruct, field );
				SP_DPMetaStruct_t * referStruct = SP_DPMetaUtils::find( mMetaInfo, field->mType );
				char * referBase = *(char**)(base + field->mOffset);

				for( int j = 0; j < referCount; j++ ) {
					buffer->append( '<' );
					buffer->append( referStruct->mName );
					buffer->append( ">\n" );

					realPickle( referBase + ( j * referStruct->mSize ), field->mItemSize, field->mType, buffer );

					buffer->append( "</" );
					buffer->append( referStruct->mName );
					buffer->append( ">\n" );
				}
			} else {
				realPickle( base + field->mOffset, field->mItemSize, field->mType, buffer );
			}
		} else {
			pickleBaseType( structure, metaStruct, field, buffer );
		}

		buffer->append( "</" );
		buffer->append( field->mName );
		buffer->append( ">\n" );
	}

	return 0;
}

int SP_XmlPickle :: pickleBaseType( void * structure, SP_DPMetaStruct_t * metaStruct,
		SP_DPMetaField_t * field, SP_XmlStringBuffer * buffer )
{
	int ret = 0;

	char * base = (char*)structure;
	void * ptr = base + field->mOffset;

	if( field->mIsPtr ) {
		if( field->mArraySize <= 0 ) ptr = *(void**)ptr;

		if( eTypeSPDPChar == field->mType ) {
			if( '\0' == field->mReferTo[0] ) {
				SP_XmlStringCodec::encode( "iso-8859-1", (char*)ptr, buffer );
			} else {
				int referCount = SP_DPMetaUtils::getReferCount( structure, metaStruct, field );
				SP_DPBase64EncodedBuffer b64buffer( ptr, referCount );
				buffer->append( b64buffer.getBuffer(), b64buffer.getLength() );
			}
		} else {
			int referCount = SP_DPMetaUtils::getReferCount( structure, metaStruct, field );
			const char * tag = getTypeTag( field->mType );
			char * referBase = (char*)ptr;

			for( int i = 0; i < referCount; i++ ) {
				buffer->append( '<' );
				buffer->append( tag );
				buffer->append( '>' );

				pickleBasePtr( referBase + ( i * field->mItemSize ), field->mType, buffer );

				buffer->append( "</" );
				buffer->append( tag );
				buffer->append( '>' );
			}
		}
	} else {
		pickleBasePtr( ptr, field->mType, buffer );
	}

	return ret;
}

int SP_XmlPickle :: pickleBasePtr( void * ptr, int type, SP_XmlStringBuffer * buffer )
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

const char * SP_XmlPickle :: getTypeTag( int type )
{
	const char * ret = NULL;

	switch( type ) {
		case eTypeSPDPChar:
			ret = "i8";
			break;
		case eTypeSPDPInt16:
			ret = "i16";
			break;
		case eTypeSPDPUint16:
			ret = "u16";
			break;
		case eTypeSPDPInt32:
			ret = "i32";
			break;
		case eTypeSPDPUint32:
			ret = "u32";
			break;
		case eTypeSPDPInt64:
			ret = "i64";
			break;
		case eTypeSPDPUint64:
			ret = "u64";
			break;
		case eTypeSPDPFloat:
			ret = "f";
			break;
		case eTypeSPDPDouble:
			ret = "d";
			break;
		default:
			break;
	}

	return ret;
}

int SP_XmlPickle :: unpickle( const char * xml, int len, int type, void * structure, int size )
{
	SP_XmlDomParser parser;
	parser.append( xml, len );

	if( NULL != parser.getError() ) {
		return -1;
	}

	SP_XmlElementNode * root = parser.getDocument()->getRootElement();

	if( NULL == root ) {
		return -1;
	}

	SP_DPMetaStruct_t * metaStruct = SP_DPMetaUtils::find( mMetaInfo, type );

	if( 0 != strcmp( root->getName(), metaStruct->mName ) ) {
		return -1;
	}

	return unpickle( root, type, structure, size );
}

int SP_XmlPickle :: unpickle( SP_XmlElementNode * root, int type, void * structure, int size )
{
	int ret = 0, i = 0;

	char * base = (char*)structure;

	SP_DPMetaStruct_t * metaStruct = SP_DPMetaUtils::find( mMetaInfo, type );

	if( NULL == metaStruct || NULL == root ) return -1;

	if( metaStruct->mSize != size ) return -1;

	SP_XmlHandle rootHandle( root );

	// unpickle the non-ptr base type, maybe include referCount
	for( i = 0; 0 == ret && i < metaStruct->mFieldCount; i++ ) {
		SP_DPMetaField_t * field = metaStruct->mFieldList + i;

		SP_XmlHandle fieldHandle = rootHandle.getChild( field->mName );

		if( field->mType < eTypeSPDPBuiltin && 0 == field->mIsPtr ) {
			if( NULL == fieldHandle.toNode() ) {
				if( field->mIsRequired ) ret = -1;
				continue;
			}

			unpickleBaseType( fieldHandle.toElement(), metaStruct, field, structure );
		}
	}

	for( i = 0; 0 == ret && i < metaStruct->mFieldCount; i++ ) {
		SP_DPMetaField_t * field = metaStruct->mFieldList + i;

		// these fields had been unpickled, skip
		if( field->mType < eTypeSPDPBuiltin && 0 == field->mIsPtr ) continue;

		SP_XmlHandle fieldHandle = rootHandle.getChild( field->mName );

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
					SP_XmlHandle itemHandle = fieldHandle.getChild( referStruct->mName, j );

					ret = unpickle( itemHandle.toElement(), field->mType,
							referBase + ( j * referStruct->mSize ), field->mItemSize );
				}
			} else {
				ret = unpickle( fieldHandle.toElement(), field->mType,
						base + field->mOffset, field->mItemSize );
			}
		} else {
			ret = unpickleBaseType( fieldHandle.toElement(), metaStruct, field, structure );
		}
	}

	return ret;
}

int SP_XmlPickle :: unpickleBaseType( SP_XmlElementNode * root, SP_DPMetaStruct_t * metaStruct,
		SP_DPMetaField_t * field, void * structure )
{
	int ret = 0;

	char * base = (char*)structure;
	void * ptr = base + field->mOffset;

	if( field->mIsPtr ) {
		if( eTypeSPDPChar == field->mType ) {
			SP_XmlHandle nodeHandle( root );
			SP_XmlHandle cdataHandle = nodeHandle.getChild(0);
			const char * cdata = cdataHandle.toCData()->getText();

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

			const char * tag = getTypeTag( field->mType );
			char * referBase = (char*)ptr;

			SP_XmlHandle rootHandle( root );

			for( int i = 0; i < referCount; i++ ) {
				SP_XmlHandle itemHandle = rootHandle.getChild( tag, i );
				unpickleBasePtr( itemHandle.toElement(), field->mType, referBase + ( i * field->mItemSize ) );
			}
		}
	} else {
		unpickleBasePtr( root, field->mType, ptr );
	}

	return ret;
}

int SP_XmlPickle :: unpickleBasePtr( SP_XmlElementNode * node, int type, void * ptr )
{
	int ret = 0;

	SP_XmlHandle nodeHandle( node );
	SP_XmlHandle cdataHandle = nodeHandle.getChild(0);

	if( cdataHandle.toCData() == NULL ) return -1;

	const char * text = cdataHandle.toCData()->getText();

	switch( type ) {
		case eTypeSPDPChar:
			*(char*)ptr = atoi( text );
			break;
		case eTypeSPDPInt16:
			*(short*)ptr = atoi( text );
			break;
		case eTypeSPDPUint16:
			*(unsigned short*)ptr = atoi( text );
			break;
		case eTypeSPDPInt32:
			*(int*)ptr = atoi( text );
			break;
		case eTypeSPDPUint32:
			*(unsigned int*)ptr = strtoul( text, NULL, 10 );
			break;
		case eTypeSPDPInt64:
			sscanf( text, "%lld", (int64_t*)ptr );
			//*(int64_t*)ptr = _strtoll( text, NULL, 10 );
			break;
		case eTypeSPDPUint64:
			sscanf( text, "%llu", (uint64_t*)ptr );
			//*(uint64_t*)ptr = strtoull( text, NULL, 10 );
			break;
		case eTypeSPDPFloat:
			sscanf( text, "%f", (float*)ptr );
			//*(float*)ptr = strtof( text, NULL );
			break;
		case eTypeSPDPDouble:
			sscanf( text, "%lf", (double*)ptr );
			//*(float*)ptr = strtod( text, NULL );
			break;
		default:
			ret = -1;
			break;
	}

	return ret;
}

