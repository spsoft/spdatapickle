/*
 * Copyright 2009 Stephen Liu
 * For license terms, see the file COPYING along with this library.
 */

#ifndef __spdpbase64_hpp__
#define __spdpbase64_hpp__

#include <sys/types.h>

class SP_DPBase64EncodedBuffer {
public:
	SP_DPBase64EncodedBuffer( const void * plainBuffer, size_t len );
	~SP_DPBase64EncodedBuffer();

	const char * getBuffer();
	size_t getLength();

private:
	char * mBuffer;
	size_t mLength;
};

class SP_DPBase64DecodedBuffer {
public:
	SP_DPBase64DecodedBuffer( const char * encodedBuffer, size_t len );
	~SP_DPBase64DecodedBuffer();

	const void * getBuffer();
	size_t getLength();

private:
	char * mBuffer;
	size_t mLength;
};

#endif

