/*
 * Copyright 2009 Stephen Liu
 * For license terms, see the file COPYING along with this library.
 */

#include <string.h>
#include <stdlib.h>
#include <assert.h>

#include "sppbpickle.hpp"
#include "spdpmetainfo.hpp"
#include "spdpmetautils.hpp"

#include "spxml/spxmlutils.hpp"

#include "spjson/sppbcodec.hpp"

SP_ProtoBufPickle :: SP_ProtoBufPickle( SP_DPMetaInfo_t * metaInfo )
	: SP_DataPickle( metaInfo )
{
}

SP_ProtoBufPickle :: ~SP_ProtoBufPickle()
{
}

int SP_ProtoBufPickle :: pickle( void * structure, int size, int type, SP_XmlStringBuffer * buffer )
{
	SP_ProtoBufEncoder encoder;

	int ret = pickle( structure, size, type, &encoder );

	if( NULL != encoder.getBuffer() ) buffer->append( encoder.getBuffer(), encoder.getSize() );

	return ret;
}

int SP_ProtoBufPickle :: pickle( void * structure, int size, int type, SP_ProtoBufEncoder * encoder )
{
	if( NULL == structure ) return -1;

	char * base = (char*)structure;

	SP_DPMetaStruct_t * metaStruct = SP_DPMetaUtils::find( mMetaInfo, type );

	if( NULL == metaStruct ) return -1;

	if( metaStruct->mSize != size ) return -1;

	for( int i = 0; i < metaStruct->mFieldCount; i++ ) {
		SP_DPMetaField_t * field = metaStruct->mFieldList + i;

		// skip referred field
		if( field->mIsReferred ) continue;

		// skip null pointer
		if( field->mIsPtr && ( field->mArraySize <= 0 )
				&& ( NULL == *(void**) ( base + field->mOffset ) ) ) continue;

		if( field->mType > eTypeSPDPBuiltin ) {
			SP_ProtoBufEncoder subEncoder;

			if( field->mIsPtr ) {
				int referCount = SP_DPMetaUtils::getReferCount( structure, metaStruct, field );
				SP_DPMetaStruct_t * referStruct = SP_DPMetaUtils::find( mMetaInfo, field->mType );
				char * referBase = *(char**)(base + field->mOffset);

				for( int j = 0; j < referCount; j++ ) {
					pickle( referBase + ( j * referStruct->mSize ), field->mItemSize, field->mType, &subEncoder );
					encoder->addBinary( field->mId, subEncoder.getBuffer(), subEncoder.getSize() );
					subEncoder.reset();
				}
			} else {
				pickle( base + field->mOffset, field->mItemSize, field->mType, &subEncoder );
				encoder->addBinary( field->mId, subEncoder.getBuffer(), subEncoder.getSize() );
			}
		} else {
			pickleBaseType( structure, metaStruct, field, encoder );
		}
	}

	return 0;
}

int SP_ProtoBufPickle :: pickleBaseType( void * structure, SP_DPMetaStruct_t * metaStruct,
		SP_DPMetaField_t * field, SP_ProtoBufEncoder * encoder )
{
	int ret = 0;

	char * base = (char*)structure;
	void * ptr = base + field->mOffset;

	if( field->mIsPtr ) {
		if( field->mArraySize <= 0 ) ptr = *(void**)ptr;

		if( eTypeSPDPChar == field->mType ) {
			if( '\0' == field->mReferTo[0] ) {
				encoder->addBinary( field->mId, (char*)ptr, strlen( (char*)ptr ) );
			} else {
				int referCount = SP_DPMetaUtils::getReferCount( structure, metaStruct, field );
				encoder->addBinary( field->mId, (char*)ptr, referCount );
			}
		} else {
			int referCount = SP_DPMetaUtils::getReferCount( structure, metaStruct, field );
			char * referBase = (char*)ptr;

			pickleBaseArray( referBase, referCount * field->mItemSize, field->mType, field->mId, encoder );
		}
	} else {
		pickleBasePtr( ptr, field->mType, field->mId, encoder );
	}

	return ret;
}

int SP_ProtoBufPickle :: pickleBaseArray( void * ptr, int size, int type,
		int fieldId, SP_ProtoBufEncoder * encoder )
{
	int ret = 0;

	switch( type ) {
		case eTypeSPDPInt16:
			encoder->addPacked( fieldId, (uint16_t*)ptr, size / sizeof( uint16_t ) );
			break;
		case eTypeSPDPUint16:
			encoder->addPacked( fieldId, (uint16_t*)ptr, size / sizeof( uint16_t ) );
			break;
		case eTypeSPDPInt32:
			encoder->addPacked( fieldId, (uint32_t*)ptr, size / sizeof( uint32_t ) );
			break;
		case eTypeSPDPUint32:
			encoder->addPacked( fieldId, (uint32_t*)ptr, size / sizeof( uint32_t ) );
			break;
		case eTypeSPDPInt64:
			encoder->addPacked( fieldId, (uint64_t*)ptr, size / sizeof( uint64_t ) );
			break;
		case eTypeSPDPUint64:
			encoder->addPacked( fieldId, (uint64_t*)ptr, size / sizeof( uint64_t ) );
			break;
		case eTypeSPDPFloat:
			encoder->addPacked( fieldId, (float*)ptr, size / sizeof( float ) );
			break;
		case eTypeSPDPDouble:
			encoder->addPacked( fieldId, (double*)ptr, size / sizeof( double ) );
			break;
		default:
			ret = -1;
			break;
	}

	return ret;
}

int SP_ProtoBufPickle :: pickleBasePtr( void * ptr, int type,
		int fieldId, SP_ProtoBufEncoder * encoder )
{
	int ret = 0;

	switch( type ) {
		case eTypeSPDPChar:
			encoder->addVarint( fieldId, *(char*)ptr );
			break;
		case eTypeSPDPInt16:
			encoder->addVarint( fieldId, *(int16_t*)ptr );
			break;
		case eTypeSPDPUint16:
			encoder->addVarint( fieldId, *(uint16_t*)ptr );
			break;
		case eTypeSPDPInt32:
			encoder->addVarint( fieldId, *(int32_t*)ptr );
			break;
		case eTypeSPDPUint32:
			encoder->addVarint( fieldId, *(uint32_t*)ptr );
			break;
		case eTypeSPDPInt64:
			encoder->addVarint( fieldId, *(int64_t*)ptr );
			break;
		case eTypeSPDPUint64:
			encoder->addVarint( fieldId, *(uint64_t*)ptr );
			break;
		case eTypeSPDPFloat:
			encoder->addFloat( fieldId, *(float*)ptr );
			break;
		case eTypeSPDPDouble:
			encoder->addDouble( fieldId, *(double*)ptr );
			break;
		default:
			ret = -1;
			break;
	}

	return ret;
}

int SP_ProtoBufPickle :: unpickle( const char * data, int len, int type, void * structure, int size )
{
	SP_ProtoBufDecoder decoder;
	decoder.copyFrom( data, len );

	return unpickle( &decoder, type, structure, size );
}

int SP_ProtoBufPickle :: unpickle( SP_ProtoBufDecoder * decoder, int type, void * structure, int size )
{
	int ret = 0, i = 0;

	char * base = (char*)structure;

	SP_DPMetaStruct_t * metaStruct = SP_DPMetaUtils::find( mMetaInfo, type );

	if( NULL == metaStruct || metaStruct->mSize != size ) return -1;

	// unpickle the non-ptr base type, maybe include referCount
	for( i = 0; 0 == ret && i < metaStruct->mFieldCount; i++ ) {
		SP_DPMetaField_t * field = metaStruct->mFieldList + i;

		// skip referred field
		if( field->mIsReferred ) continue;

		if( field->mType < eTypeSPDPBuiltin && 0 == field->mIsPtr ) {
			SP_ProtoBufDecoder::KeyValPair_t pair;
			bool findRet = decoder->find( field->mId, &pair );

			if( ! findRet ) {
				if( field->mIsRequired ) ret = -1;
				continue;
			}

			unpickleBaseType( &pair, metaStruct, field, structure );
		}
	}

	for( i = 0; 0 == ret && i < metaStruct->mFieldCount; i++ ) {
		SP_DPMetaField_t * field = metaStruct->mFieldList + i;

		// skip referred field
		if( field->mIsReferred ) continue;

		// these fields had been unpickled, skip
		if( field->mType < eTypeSPDPBuiltin && 0 == field->mIsPtr ) continue;

		SP_ProtoBufDecoder::KeyValPair_t pair;
		bool findRet = decoder->find( field->mId, &pair );

		if( ! findRet ) {
			if( field->mIsRequired && ( 0 == field->mIsPtr || field->mArraySize > 0 ) ) {
				ret = -1;
			}

			SP_DPMetaUtils::setReferCount( structure, metaStruct, field, 0 );
			continue;
		}

		if( field->mType > eTypeSPDPBuiltin ) {
			if( field->mIsPtr ) {
				int referCount = pair.mRepeatedCount;
				SP_DPMetaUtils::setReferCount( structure, metaStruct, field, referCount );

				SP_DPMetaStruct_t * referStruct = SP_DPMetaUtils::find( mMetaInfo, field->mType );
				char * referBase = (char*)calloc( field->mItemSize, referCount );
				*(void**)(base + field->mOffset) = referBase;

				for( int j = 0; 0 == ret && j < referCount; j++ ) {
					findRet = decoder->find( field->mId, &pair, j );

					if( findRet ) {
						SP_ProtoBufDecoder subDecoder;
						subDecoder.attach( pair.mBinary.mBuffer, pair.mBinary.mLen );
						ret = unpickle( &subDecoder, field->mType,
								referBase + ( j * referStruct->mSize ), field->mItemSize );
					} else {
						ret = -1;
					}
				}
			} else {
				SP_ProtoBufDecoder subDecoder;
				subDecoder.attach( pair.mBinary.mBuffer, pair.mBinary.mLen );
				ret = unpickle( &subDecoder, field->mType, base + field->mOffset, field->mItemSize );
			}
		} else {
			ret = unpickleBaseType( &pair, metaStruct, field, structure );
		}
	}

	return ret;
}

int SP_ProtoBufPickle :: unpickleBaseType( void * node, SP_DPMetaStruct_t * metaStruct,
		SP_DPMetaField_t * field, void * structure )
{
	int ret = 0;

	SP_ProtoBufDecoder::KeyValPair_t * pair = (SP_ProtoBufDecoder::KeyValPair_t*)node;

	char * base = (char*)structure;
	void * ptr = base + field->mOffset;

	if( field->mIsPtr ) {
		const char * buffer = pair->mBinary.mBuffer;
		int len = pair->mBinary.mLen;

		if( eTypeSPDPChar == field->mType ) {
			if( field->mArraySize > 0 ) {
				len = len > field->mArraySize ? field->mArraySize : len;
				strncpy( (char*)ptr, buffer, len );
				((char*)ptr)[ field->mArraySize - 1 ] = '\0';
			} else {
				ptr = SP_ProtoBufCodecUtils::dup( buffer, len );
				*(void**)(base + field->mOffset ) = ptr;
			}

			SP_DPMetaUtils::setReferCount( structure, metaStruct, field, len );
		} else {
			int referCount = len;
			if( field->mArraySize <= 0 ) {
				ptr = calloc( field->mItemSize, referCount );
				*(void**)(base + field->mOffset ) = ptr;
			} else {
				referCount = field->mArraySize;
			}

			referCount = unpickleBaseArray( pair, field->mType, ptr, field->mItemSize * referCount );

			SP_DPMetaUtils::setReferCount( structure, metaStruct, field, referCount );
		}
	} else {
		unpickleBasePtr( node, field->mType, ptr );
	}

	return ret;
}

int SP_ProtoBufPickle :: unpickleBaseArray( void * node, int type, void * ptr, int size )
{
	int count = 0;

	SP_ProtoBufDecoder::KeyValPair_t * pair = (SP_ProtoBufDecoder::KeyValPair_t*)node;

	const char * buffer = pair->mBinary.mBuffer;
	int len = pair->mBinary.mLen;

	switch( type ) {
		case eTypeSPDPInt16:
			count = SP_ProtoBufCodecUtils::getPacked( buffer, len, (uint16_t*)ptr, size / sizeof( uint16_t ) );
			break;
		case eTypeSPDPUint16:
			count = SP_ProtoBufCodecUtils::getPacked( buffer, len, (uint16_t*)ptr, size / sizeof( uint16_t ) );
			break;
		case eTypeSPDPInt32:
			count = SP_ProtoBufCodecUtils::getPacked( buffer, len, (uint32_t*)ptr, size / sizeof( uint32_t ) );
			break;
		case eTypeSPDPUint32:
			count = SP_ProtoBufCodecUtils::getPacked( buffer, len, (uint32_t*)ptr, size / sizeof( uint32_t ) );
			break;
		case eTypeSPDPInt64:
			count = SP_ProtoBufCodecUtils::getPacked( buffer, len, (uint64_t*)ptr, size / sizeof( uint64_t ) );
			break;
		case eTypeSPDPUint64:
			count = SP_ProtoBufCodecUtils::getPacked( buffer, len, (uint64_t*)ptr, size / sizeof( uint64_t ) );
			break;
		case eTypeSPDPFloat:
			count = SP_ProtoBufCodecUtils::getPacked( buffer, len, (float*)ptr, size / sizeof( float ) );
			break;
		case eTypeSPDPDouble:
			count = SP_ProtoBufCodecUtils::getPacked( buffer, len, (double*)ptr, size / sizeof( double ) );
			break;
		default:
			count = -1;
			break;
	}

	return count;
}

int SP_ProtoBufPickle :: unpickleBasePtr( void * node, int type, void * ptr )
{
	int ret = 0;

	SP_ProtoBufDecoder::KeyValPair_t * pair = (SP_ProtoBufDecoder::KeyValPair_t*)node;

	switch( type ) {
		case eTypeSPDPChar:
			*(char*)ptr = (char)pair->mVarint.s;
			break;
		case eTypeSPDPInt16:
			*(int16_t*)ptr = (int16_t)pair->mVarint.s;
			break;
		case eTypeSPDPUint16:
			*(uint16_t*)ptr = (uint16_t)pair->mVarint.u;
			break;
		case eTypeSPDPInt32:
			if( SP_ProtoBufDecoder::eWire32Bit == pair->mWireType ) {
				*(int32_t*)ptr = pair->m32Bit.s;
			} else {
				*(int32_t*)ptr = (int32_t)pair->mVarint.s;
			}
			break;
		case eTypeSPDPUint32:
			if( SP_ProtoBufDecoder::eWire32Bit == pair->mWireType ) {
				*(uint32_t*)ptr = pair->m32Bit.u;
			} else {
				*(uint32_t*)ptr = (uint32_t)pair->mVarint.u;
			}
			break;
		case eTypeSPDPInt64:
			if( SP_ProtoBufDecoder::eWire64Bit == pair->mWireType ) {
				*(int64_t*)ptr = pair->m64Bit.s;
			} else {
				*(int64_t*)ptr = pair->mVarint.s;
			}
			break;
		case eTypeSPDPUint64:
			if( SP_ProtoBufDecoder::eWire64Bit == pair->mWireType ) {
				*(uint64_t*)ptr = pair->m64Bit.u;
			} else {
				*(uint64_t*)ptr = pair->mVarint.u;
			}
			break;
		case eTypeSPDPFloat:
			*(float*)ptr = pair->m32Bit.f;
			break;
		case eTypeSPDPDouble:
			*(double*)ptr = pair->m64Bit.d;
			break;
		default:
			ret = -1;
			break;
	}

	return ret;
}

