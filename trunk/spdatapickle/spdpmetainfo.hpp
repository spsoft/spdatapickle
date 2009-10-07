/*
 * Copyright 2009 Stephen Liu
 * For license terms, see the file COPYING along with this library.
 */

#ifndef __spdpmetainfo_hpp__
#define __spdpmetainfo_hpp__

#ifdef __cplusplus
extern "C" {
#endif

enum {
	eTypeSPDPUnknown = 0,

	eTypeSPDPChar,

	eTypeSPDPInt16,
	eTypeSPDPUint16,

	eTypeSPDPInt32,
	eTypeSPDPUint32,

	eTypeSPDPInt64,
	eTypeSPDPUint64,

	eTypeSPDPFloat,
	eTypeSPDPDouble,

	eTypeSPDPUserDefine = 512
};

#define SP_DP_FIELD_OFFSET(type,field) ((int)&(((type *)0)->field))
#define SP_DP_ARRAY_SIZE(array)        ((int)(sizeof(array)/sizeof(array[0])))

typedef struct tagSP_DPMetaField {
	short mMySize;
	short mId;

	char * mName;
	short mOffset;
	short mType;
	char mIsPtr;
	char mIsRequired;
	short mArraySize;
	char * mReferTo;
	short mFieldSize;
	short mItemSize;
	char mIsReferred;
} SP_DPMetaField_t;

typedef struct tagSP_DPMetaStruct {
	short mMySize;
	short mId;

	char * mName;
	int mSize;
	short mFieldCount;
	SP_DPMetaField_t * mFieldList;
} SP_DPMetaStruct_t;

typedef struct tagSP_DPMetaInfo {
	short mMySize;

	char * mPrefix;
	char * mFileName;
	short mStructCount;
	SP_DPMetaStruct_t * mStructList;
} SP_DPMetaInfo_t;

#ifdef __cplusplus
}
#endif

#endif

