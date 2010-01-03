/*
 * Copyright 2010 Stephen Liu
 * For license terms, see the file COPYING along with this library.
 */

#ifndef __spdpbuiltin_hpp__
#define __spdpbuiltin_hpp__

#ifdef __cplusplus
extern "C" {
#endif

#include "spdatapickle/spdpmetainfo.hpp"
#include "spdatapickle/spdpbuiltin.hpp"
#include "spjson/spjsonport.hpp"

enum {
	eTypeSP_DPChar = eTypeSPDPBuiltin + 1,
	eTypeSP_DPInt16,
	eTypeSP_DPUInt16,
	eTypeSP_DPInt32,
	eTypeSP_DPUInt32,
	eTypeSP_DPInt64,
	eTypeSP_DPUInt64,
	eTypeSP_DPFloat,
	eTypeSP_DPDouble,
	eTypeSP_DPString,
	eTypeSP_DPBuffer,
	eTypeSP_DPStringList,
	eTypeSP_DPInt32List,
	eTypeSP_DPUInt32List,
	eTypeSP_DPInt64List,
	eTypeSP_DPUInt64List
};

typedef struct tagSP_DPChar {
	char mValue;
} SP_DPChar_t;

typedef struct tagSP_DPInt16 {
	int16_t mValue;
} SP_DPInt16_t;

typedef struct tagSP_DPUInt16 {
	uint16_t mValue;
} SP_DPUInt16_t;

typedef struct tagSP_DPInt32 {
	int32_t mValue;
} SP_DPInt32_t;

typedef struct tagSP_DPUInt32 {
	uint32_t mValue;
} SP_DPUInt32_t;

typedef struct tagSP_DPInt64 {
	int64_t mValue;
} SP_DPInt64_t;

typedef struct tagSP_DPUInt64 {
	uint64_t mValue;
} SP_DPUInt64_t;

typedef struct tagSP_DPFloat {
	float mValue;
} SP_DPFloat_t;

typedef struct tagSP_DPDouble {
	double mValue;
} SP_DPDouble_t;

typedef struct tagSP_DPString {
	char * mString;
} SP_DPString_t;

typedef struct tagSP_DPBuffer {
	char * mBuffer;

	int mSize;
} SP_DPBuffer_t;

typedef struct tagSP_DPStringList {
	SP_DPString_t * mList;

	int mCount;
} SP_DPStringList_t;

typedef struct tagSP_DPInt32List {
	int * mList;

	int mCount;
} SP_DPInt32List_t;

typedef struct tagSP_DPUInt32List {
	unsigned int * mList;

	int mCount;
} SP_DPUInt32List_t;

typedef struct tagSP_DPInt64List {
	int64_t * mList;

	int mCount;
} SP_DPInt64List_t;

typedef struct tagSP_DPUInt64List {
	uint64_t * mList;

	int mCount;
} SP_DPUInt64List_t;

typedef struct tagSP_DPMetaInfo SP_DPMetaInfo_t;
extern SP_DPMetaInfo_t * gSP_DPBuiltinMetaInfo;

#ifdef __cplusplus
}
#endif

#endif

