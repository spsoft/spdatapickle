/*
 * Copyright 2010 Stephen Liu
 * For license terms, see the file COPYING along with this library.
 */

#ifndef __spdpvector_hpp__
#define __spdpvector_hpp__

#include <stdlib.h>

template< typename ItemStructType, typename ItemClassType >
class SP_DPVector {
public:
	SP_DPVector( ItemStructType ** listPtr, int * countPtr, int arraySize )
	{
		mArraySize = arraySize;
		mFakeCount = 0;

		mListPtr = listPtr;
		mCountPtr = ( NULL != countPtr ) ? countPtr : &mFakeCount;
		mMaxCount = mArraySize > 0 ? mArraySize : ( * countPtr );

		mObjects4Delete = NULL;

		if( mMaxCount > 0 ) {
			mObjects4Delete = (ItemClassType**)calloc( sizeof( void * ), mMaxCount );
		}
	}

	~SP_DPVector()
	{
		clearObjects();

		mListPtr = NULL;
		mCountPtr = NULL;
	}

	void clearObjects()
	{
		if( NULL != mObjects4Delete ) {
			for( int i = 0; i < mMaxCount; i++ ) {
					if( NULL != mObjects4Delete[i] ) delete mObjects4Delete[i];
			}

			free( mObjects4Delete );
			mObjects4Delete = NULL;
		}
	}

	void reserve( int count )
	{
		if( mMaxCount < count )
		{
			ItemStructType * newList = (ItemStructType*)calloc( sizeof( ItemStructType ), count );

			if( NULL != * mListPtr )
			{
				memcpy( newList, * mListPtr, sizeof( ItemStructType ) * ( * mCountPtr ) );
				free( * mListPtr );
			}

			* mListPtr = newList;
			mMaxCount = count;

			clearObjects();

			mObjects4Delete = (ItemClassType**)calloc( sizeof( void * ), mMaxCount );
		}
	}

	int append( const ItemClassType * item )
	{
		int count = * mCountPtr;

		if( mArraySize > 0 && count >= mArraySize ) return -1;

		reserve( count + 1 );

		item->copyTo( ( * mListPtr ) + count );

		( * mCountPtr ) ++;

		return 0;
	}

	int getCount()
	{
		return * mCountPtr;
	}

	ItemClassType * getItem( int index )
	{
		ItemClassType * ret = NULL;

		if( index < * mCountPtr ) {
			if( NULL == mObjects4Delete[ index ] ) {
				ret = new ItemClassType( ( * mListPtr ) + index );
				mObjects4Delete[ index ] = ret;
			}
		}

		return ret;
	}

private:
	ItemStructType ** mListPtr;
	int * mCountPtr;
	int mMaxCount, mFakeCount;
	int mArraySize;

	ItemClassType ** mObjects4Delete;
};

#endif

