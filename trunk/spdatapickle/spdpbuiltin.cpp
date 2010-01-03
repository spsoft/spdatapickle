/*
 * Copyright 2010 Stephen Liu
 * For license terms, see the file COPYING along with this library.
 */

#include "spdpbuiltin.hpp"

#include "spdatapickle/spdpmetainfo.hpp"

static SP_DPMetaField_t gMetaSP_DPChar [] = {
	{ sizeof(SP_DPMetaField_t), 1, "Value",
		SP_DP_FIELD_OFFSET(SP_DPChar_t, mValue), eTypeSPDPChar, 0, 1, 0,
		"", sizeof(char), sizeof(char), 0 }
};

static SP_DPMetaField_t gMetaSP_DPInt16 [] = {
	{ sizeof(SP_DPMetaField_t), 1, "Value",
		SP_DP_FIELD_OFFSET(SP_DPInt16_t, mValue), eTypeSPDPInt16, 0, 1, 0,
		"", sizeof(short), sizeof(short), 0 }
};

static SP_DPMetaField_t gMetaSP_DPUInt16 [] = {
	{ sizeof(SP_DPMetaField_t), 1, "Value",
		SP_DP_FIELD_OFFSET(SP_DPUInt16_t, mValue), eTypeSPDPUint16, 0, 1, 0,
		"", sizeof(unsigned short), sizeof(unsigned short), 0 }
};

static SP_DPMetaField_t gMetaSP_DPInt32 [] = {
	{ sizeof(SP_DPMetaField_t), 1, "Value",
		SP_DP_FIELD_OFFSET(SP_DPInt32_t, mValue), eTypeSPDPInt32, 0, 1, 0,
		"", sizeof(int), sizeof(int), 0 }
};

static SP_DPMetaField_t gMetaSP_DPUInt32 [] = {
	{ sizeof(SP_DPMetaField_t), 1, "Value",
		SP_DP_FIELD_OFFSET(SP_DPUInt32_t, mValue), eTypeSPDPUint32, 0, 1, 0,
		"", sizeof(unsigned int), sizeof(unsigned int), 0 }
};

static SP_DPMetaField_t gMetaSP_DPInt64 [] = {
	{ sizeof(SP_DPMetaField_t), 1, "Value",
		SP_DP_FIELD_OFFSET(SP_DPInt64_t, mValue), eTypeSPDPInt64, 0, 1, 0,
		"", sizeof(int64_t), sizeof(int64_t), 0 }
};

static SP_DPMetaField_t gMetaSP_DPUInt64 [] = {
	{ sizeof(SP_DPMetaField_t), 1, "Value",
		SP_DP_FIELD_OFFSET(SP_DPUInt64_t, mValue), eTypeSPDPUint64, 0, 1, 0,
		"", sizeof(uint64_t), sizeof(uint64_t), 0 }
};

static SP_DPMetaField_t gMetaSP_DPFloat [] = {
	{ sizeof(SP_DPMetaField_t), 1, "Value",
		SP_DP_FIELD_OFFSET(SP_DPFloat_t, mValue), eTypeSPDPFloat, 0, 1, 0,
		"", sizeof(float), sizeof(float), 0 }
};

static SP_DPMetaField_t gMetaSP_DPDouble [] = {
	{ sizeof(SP_DPMetaField_t), 1, "Value",
		SP_DP_FIELD_OFFSET(SP_DPDouble_t, mValue), eTypeSPDPDouble, 0, 1, 0,
		"", sizeof(double), sizeof(double), 0 }
};

static SP_DPMetaField_t gMetaSP_DPString [] = {
	{ sizeof(SP_DPMetaField_t), 1, "String",
		SP_DP_FIELD_OFFSET(SP_DPString_t, mString), eTypeSPDPChar, 1, 1, 0,
		"", sizeof(char *), sizeof(char), 0 }
};

static SP_DPMetaField_t gMetaSP_DPStringList [] = {
	{ sizeof(SP_DPMetaField_t), 1, "List",
		SP_DP_FIELD_OFFSET(SP_DPStringList_t, mList), eTypeSP_DPString, 1, 1, 0,
		"Count", sizeof(SP_DPString_t *), sizeof(SP_DPString_t), 0 },
	{ sizeof(SP_DPMetaField_t), 2, "Count",
		SP_DP_FIELD_OFFSET(SP_DPStringList_t, mCount), eTypeSPDPInt32, 0, 1, 0,
		"", sizeof(int), sizeof(int), 1 }
};

static SP_DPMetaField_t gMetaSP_DPBuffer [] = {
	{ sizeof(SP_DPMetaField_t), 1, "Buffer",
		SP_DP_FIELD_OFFSET(SP_DPBuffer_t, mBuffer), eTypeSPDPChar, 1, 1, 0,
		"Size", sizeof(char *), sizeof(char), 0 },
	{ sizeof(SP_DPMetaField_t), 2, "Size",
		SP_DP_FIELD_OFFSET(SP_DPBuffer_t, mSize), eTypeSPDPInt32, 0, 1, 0,
		"", sizeof(int), sizeof(int), 1 }
};

static SP_DPMetaField_t gMetaSP_DPInt32List [] = {
	{ sizeof(SP_DPMetaField_t), 1, "List",
		SP_DP_FIELD_OFFSET(SP_DPInt32List_t, mList), eTypeSPDPInt32, 1, 1, 0,
		"Count", sizeof(int *), sizeof(int), 0 },
	{ sizeof(SP_DPMetaField_t), 2, "Count",
		SP_DP_FIELD_OFFSET(SP_DPInt32List_t, mCount), eTypeSPDPInt32, 0, 1, 0,
		"", sizeof(int), sizeof(int), 1 }
};

static SP_DPMetaField_t gMetaSP_DPUInt32List [] = {
	{ sizeof(SP_DPMetaField_t), 1, "List",
		SP_DP_FIELD_OFFSET(SP_DPUInt32List_t, mList), eTypeSPDPUint32, 1, 1, 0,
		"Count", sizeof(unsigned int *), sizeof(unsigned int), 0 },
	{ sizeof(SP_DPMetaField_t), 2, "Count",
		SP_DP_FIELD_OFFSET(SP_DPUInt32List_t, mCount), eTypeSPDPInt32, 0, 1, 0,
		"", sizeof(int), sizeof(int), 1 }
};

static SP_DPMetaField_t gMetaSP_DPInt64List [] = {
	{ sizeof(SP_DPMetaField_t), 1, "List",
		SP_DP_FIELD_OFFSET(SP_DPInt64List_t, mList), eTypeSPDPInt64, 1, 1, 0,
		"Count", sizeof(int64_t *), sizeof(int64_t), 0 },
	{ sizeof(SP_DPMetaField_t), 2, "Count",
		SP_DP_FIELD_OFFSET(SP_DPInt64List_t, mCount), eTypeSPDPInt32, 0, 1, 0,
		"", sizeof(int), sizeof(int), 1 }
};

static SP_DPMetaField_t gMetaSP_DPUInt64List [] = {
	{ sizeof(SP_DPMetaField_t), 1, "List",
		SP_DP_FIELD_OFFSET(SP_DPUInt64List_t, mList), eTypeSPDPUint64, 1, 1, 0,
		"Count", sizeof(uint64_t *), sizeof(uint64_t), 0 },
	{ sizeof(SP_DPMetaField_t), 2, "Count",
		SP_DP_FIELD_OFFSET(SP_DPUInt64List_t, mCount), eTypeSPDPInt32, 0, 1, 0,
		"", sizeof(int), sizeof(int), 1 }
};

static SP_DPMetaStruct_t gMetaSP_DPStructList [] = {
	{ sizeof( SP_DPMetaStruct_t ), eTypeSP_DPChar, "Char", sizeof(SP_DPChar_t),
		SP_DP_ARRAY_SIZE(gMetaSP_DPChar), gMetaSP_DPChar },
	{ sizeof( SP_DPMetaStruct_t ), eTypeSP_DPInt16, "Int16", sizeof(SP_DPInt16_t),
		SP_DP_ARRAY_SIZE(gMetaSP_DPInt16), gMetaSP_DPInt16 },
	{ sizeof( SP_DPMetaStruct_t ), eTypeSP_DPUInt16, "UInt16", sizeof(SP_DPUInt16_t),
		SP_DP_ARRAY_SIZE(gMetaSP_DPUInt16), gMetaSP_DPUInt16 },
	{ sizeof( SP_DPMetaStruct_t ), eTypeSP_DPInt32, "Int32", sizeof(SP_DPInt32_t),
		SP_DP_ARRAY_SIZE(gMetaSP_DPInt32), gMetaSP_DPInt32 },
	{ sizeof( SP_DPMetaStruct_t ), eTypeSP_DPUInt32, "UInt32", sizeof(SP_DPUInt32_t),
		SP_DP_ARRAY_SIZE(gMetaSP_DPUInt32), gMetaSP_DPUInt32 },
	{ sizeof( SP_DPMetaStruct_t ), eTypeSP_DPInt64, "Int64", sizeof(SP_DPInt64_t),
		SP_DP_ARRAY_SIZE(gMetaSP_DPInt64), gMetaSP_DPInt64 },
	{ sizeof( SP_DPMetaStruct_t ), eTypeSP_DPUInt64, "UInt64", sizeof(SP_DPUInt64_t),
		SP_DP_ARRAY_SIZE(gMetaSP_DPUInt64), gMetaSP_DPUInt64 },
	{ sizeof( SP_DPMetaStruct_t ), eTypeSP_DPFloat, "Float", sizeof(SP_DPFloat_t),
		SP_DP_ARRAY_SIZE(gMetaSP_DPFloat), gMetaSP_DPFloat },
	{ sizeof( SP_DPMetaStruct_t ), eTypeSP_DPDouble, "Double", sizeof(SP_DPDouble_t),
		SP_DP_ARRAY_SIZE(gMetaSP_DPDouble), gMetaSP_DPDouble },
	{ sizeof( SP_DPMetaStruct_t ), eTypeSP_DPString, "String", sizeof(SP_DPString_t),
		SP_DP_ARRAY_SIZE(gMetaSP_DPString), gMetaSP_DPString },
	{ sizeof( SP_DPMetaStruct_t ), eTypeSP_DPBuffer, "Buffer", sizeof(SP_DPBuffer_t),
		SP_DP_ARRAY_SIZE(gMetaSP_DPBuffer), gMetaSP_DPBuffer },
	{ sizeof( SP_DPMetaStruct_t ), eTypeSP_DPStringList, "StringList", sizeof(SP_DPStringList_t),
		SP_DP_ARRAY_SIZE(gMetaSP_DPStringList), gMetaSP_DPStringList },
	{ sizeof( SP_DPMetaStruct_t ), eTypeSP_DPInt32List, "Int32List", sizeof(SP_DPInt32List_t),
		SP_DP_ARRAY_SIZE(gMetaSP_DPInt32List), gMetaSP_DPInt32List },
	{ sizeof( SP_DPMetaStruct_t ), eTypeSP_DPUInt32List, "UInt32List", sizeof(SP_DPUInt32List_t),
		SP_DP_ARRAY_SIZE(gMetaSP_DPUInt32List), gMetaSP_DPUInt32List },
	{ sizeof( SP_DPMetaStruct_t ), eTypeSP_DPInt64List, "Int64List", sizeof(SP_DPInt64List_t),
		SP_DP_ARRAY_SIZE(gMetaSP_DPInt64List), gMetaSP_DPInt64List },
	{ sizeof( SP_DPMetaStruct_t ), eTypeSP_DPUInt64List, "UInt64List", sizeof(SP_DPUInt64List_t),
		SP_DP_ARRAY_SIZE(gMetaSP_DPUInt64List), gMetaSP_DPUInt64List },
};

static SP_DPMetaInfo_t gMetaSP_DPBuiltin = {
	sizeof( SP_DPMetaInfo_t ), "SP_DP", "builtin", SP_DP_ARRAY_SIZE(gMetaSP_DPStructList), gMetaSP_DPStructList
};

SP_DPMetaInfo_t * gSP_DPBuiltinMetaInfo = &gMetaSP_DPBuiltin;

