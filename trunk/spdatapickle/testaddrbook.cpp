/*
 * Copyright 2009 Stephen Liu
 * For license terms, see the file COPYING along with this library.
 */

#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <assert.h>

#include "dp_xyzaddrbookstruct.hpp"

#include "spdatapickle/spdpmetautils.hpp"

#include "spxml/spxmlutils.hpp"
#include "spjson/spjsonutils.hpp"

void printBuffer( SP_XmlStringBuffer * buffer )
{
	const char * pos = buffer->getBuffer();
	int size = buffer->getSize();

	int isPrint = 1;
	for( int i = 0; i < size; i++ ) {
		if( ! isprint( pos[i] ) && ! isspace( pos[i] ) ) {
			isPrint = 0;
			break;
		}
	}

	if( isPrint ) {
		printf( "len %d\n%s\n\n", size, pos );
 	} else {
		printf( "<binary buffer, size %d>\n", size );
	}
}

void testEmail( XYZAddrbookPickle * pickle )
{
	XYZEmail_t email;
	memset( &email, 0, sizeof( email ) );

	strncpy( email.mType, "work", sizeof( email.mType ) - 1 );
	email.mAddress = strdup( "foo <foo@bar.com>" );

	SP_XmlStringBuffer buffer;

	pickle->pickle( &email, &buffer );

	printBuffer( &buffer );

	pickle->freeFields( email );
}

void testPhoneNumber( XYZAddrbookPickle * pickle )
{
	XYZPhoneNumber_t phoneNumber;
	memset( &phoneNumber, 0, sizeof( phoneNumber ) );

	strncpy( phoneNumber.mType, "home", sizeof( phoneNumber.mType ) - 1 );
	phoneNumber.mPrimary = -1;
	phoneNumber.mContent = strdup( "12345678" );

	SP_XmlStringBuffer buffer;
	pickle->pickle( &phoneNumber, &buffer );

	printBuffer( &buffer );

	SP_DPAlloc alloc( gXYZAddrbookMetaInfo );
	alloc.free( &phoneNumber, sizeof( phoneNumber ), eTypeXYZPhoneNumber );
}

void initContact( XYZContact_t * contact )
{
	memset( contact, 0, sizeof( *contact ) );

	contact->mName = strdup( "foobar" );
	contact->mImage.mSize = 10;
	contact->mImage.mBuffer = (char*)malloc( contact->mImage.mSize );
	for( int i = 0; i < contact->mImage.mSize; i++ ) {
		contact->mImage.mBuffer[ i ] = i;
	}

	contact->mEmailCount = 2;
	{
		XYZEmail_t * list = (XYZEmail_t*)calloc( sizeof( XYZEmail_t ), 2 );
		strncpy( list[0].mType, "home", sizeof( list[0].mType ) - 1 );
		list[0].mAddress = strdup( "home <home@foo.bar>" );
		strncpy( list[1].mType, "work", sizeof( list[1].mType ) - 1 );
		list[1].mAddress = strdup( "work <work@foo.bar>" );
		contact->mEmailList = list;
	}

	contact->mPhoneCount = 2;
	{
		XYZPhoneNumber_t * list = (XYZPhoneNumber_t*)calloc( sizeof( XYZPhoneNumber_t ), 2 );
		strncpy( list[0].mType, "home", sizeof( list[0].mType ) - 1 );
		list[0].mPrimary = 1;
		list[0].mContent = strdup( "861012345678" );
		strncpy( list[1].mType, "work", sizeof( list[1].mType ) - 1 );
		list[1].mPrimary = 0;
		list[1].mContent = strdup( "862012345678" );

		contact->mPhoneList = list;
	}

	contact->mLuckNumber.mList = (int*)calloc( sizeof( int ), 4 );
	contact->mLuckNumber.mCount = 4;
	{
		contact->mLuckNumber.mList[0] = 16;
		contact->mLuckNumber.mList[1] = 8;
		contact->mLuckNumber.mList[2] = 6;
		contact->mLuckNumber.mList[3] = 88;
	}
}

void testContact( XYZAddrbookPickle * pickle )
{
	XYZContact_t contact;

	initContact( &contact );

	SP_XmlStringBuffer buffer;

	pickle->pickle( &contact, &buffer );

	printBuffer( &buffer );

	pickle->freeFields( contact );
}

void testUnpickle( XYZAddrbookPickle * pickle )
{
	XYZContact_t org;

	initContact( &org );

	SP_XmlStringBuffer buffer;

	pickle->pickle( &org, &buffer );

	printBuffer( &buffer );

	XYZContact_t contact;
	memset( &contact, 0, sizeof( contact ) );

	int ret = pickle->unpickle( &buffer, &contact );

	printf( "Unpickle %d\n\n", ret );

	if( 0 != ret ) {
		printf( "Unpickle fail\n" );
		return;
	}

	int i = 0;

	printf( "name %s\n", contact.mName );
	printf( "image.size %d\n", contact.mImage.mSize );
	printf( "image.buffer: " );
	for( i = 0; i < contact.mImage.mSize; i++ ) {
		printf( "%d ", contact.mImage.mBuffer[i] );
	}
	printf( "\n" );

	printf( "email.count %d\n", contact.mEmailCount );
	for( i = 0; i < contact.mEmailCount; i++ ) {
		XYZEmail_t * email = contact.mEmailList + i;
		printf( "email#%d: %s, %s\n", i, email->mAddress, email->mType );
	}

	printf( "phone.count %d\n", contact.mPhoneCount );
	for( i = 0; i < contact.mPhoneCount; i++ ) {
		XYZPhoneNumber_t * phone = contact.mPhoneList + i;
		printf( "phone#%d: %s, %s, %d\n", i, phone->mContent, phone->mType, phone->mPrimary );
	}

	printf( "luck.number.count %d\n", contact.mLuckNumber.mCount );
	for( i = 0; i < contact.mLuckNumber.mCount; i++ ) {
		printf( "number#%d: %d\n", i, contact.mLuckNumber.mList[i] );
	}

	SP_DPAlloc alloc( gXYZAddrbookMetaInfo );
	alloc.free( &contact, sizeof( contact ), eTypeXYZContact );
	alloc.free( &org, sizeof( org ), eTypeXYZContact );
}

//===================================================================

class XYZEmail {
public:
	XYZEmail();
	~XYZEmail();

	XYZEmail( const XYZEmail & other );

	XYZEmail( XYZEmail_t * impl );

	void copyTo( XYZEmail_t * impl ) const;

	XYZEmail & operator=( const XYZEmail & other );

	XYZEmail_t * getImpl() const;

	char * getType() const;

	void setAddress( const char * mAddress );
	const char * getAddress() const;

	void setNickname( const char * mNickname );
	const char * getNickname() const;

private:
	XYZEmail_t * mImpl;
	int mOwner;
};

XYZEmail :: XYZEmail()
{
	mImpl = (XYZEmail_t*)calloc( sizeof( XYZEmail_t ), 1 );
	mOwner = 1;
}

XYZEmail :: ~XYZEmail()
{
	if( mOwner ) {
		XYZAddrbookPickle::freeFields( *mImpl );
		free( mImpl );
	}
	mImpl = NULL;
}

XYZEmail :: XYZEmail( XYZEmail_t * impl )
{
	mImpl = impl;
	mOwner = 0;
}

XYZEmail :: XYZEmail( const XYZEmail & other )
{
	mImpl = NULL;
	operator=( other );
}

XYZEmail & XYZEmail :: operator=( const XYZEmail & other )
{
	if( NULL != mImpl )
	{
		XYZAddrbookPickle::freeFields( *mImpl );
		free( mImpl );
		mImpl = NULL;
	}

	mImpl = (XYZEmail_t*)calloc( sizeof( XYZEmail_t ), 1 );
	mOwner = 1;

	XYZAddrbookPickle::deepCopy( mImpl, other.mImpl );

	return *this;
}

void XYZEmail :: copyTo( XYZEmail_t * impl ) const
{
	XYZAddrbookPickle::deepCopy( mImpl, impl );
}

XYZEmail_t * XYZEmail :: getImpl() const
{
	return mImpl;
}

char * XYZEmail :: getType() const
{
	return mImpl->mType;
}

void XYZEmail :: setAddress( const char * mAddress )
{
	if( mImpl->mAddress ) free( mImpl->mAddress );
	mImpl->mAddress = NULL;
	if( mAddress ) mImpl->mAddress = strdup( mAddress );
}

const char * XYZEmail :: getAddress() const
{
	return mImpl->mAddress;
}

void XYZEmail :: setNickname( const char * mNickname )
{
	if( mImpl->mNickname ) free( mImpl->mNickname );
	mImpl->mNickname = NULL;
	if( mNickname ) mImpl->mNickname = strdup( mNickname );
}

const char * XYZEmail :: getNickname() const
{
	return mImpl->mNickname;
}

//===================================================================

void testDeepCopy()
{
	XYZAddrbookPickle pickle( SP_DataPickle::eXml );

	SP_XmlStringBuffer orgBuffer;
	XYZContact_t org;
	{
		initContact( &org );
		pickle.pickle( &org, &orgBuffer );
	}

	SP_XmlStringBuffer contactBuffer;
	XYZContact_t contact;
	{
		pickle.deepCopy( &org, &contact );
		pickle.pickle( &contact, &contactBuffer );
	}

	assert( 0 == memcmp( orgBuffer.getBuffer(), contactBuffer.getBuffer(), orgBuffer.getSize() ) );

	pickle.freeFields( contact );
	pickle.freeFields( org );
}

void testVector()
{
	XYZAddrbookPickle pickle( SP_DataPickle::eXml );

	XYZContact_t org;
	initContact( &org );

	SP_DPVector<XYZEmail_t, XYZEmail> emailList( & org.mEmailList, & org.mEmailCount, 0 );

	printf( "count %d\n", emailList.getCount() );
	for( int i = 0; i < emailList.getCount(); i++ ) {
		printf( "#%d: %s\n", i, emailList.getItem(i)->getAddress() );
	}

	XYZEmail email;
	email.setAddress( "school <school@foo.bar>" );

	emailList.append( &email );

	printf( "count %d\n", emailList.getCount() );
	for( int i = 0; i < emailList.getCount(); i++ ) {
		printf( "#%d: %s\n", i, emailList.getItem(i)->getAddress() );
	}

	pickle.freeFields( org );

	{
		printf( "test array\n" );

		XYZEmail_t tmpList[2];
		SP_DPVector<XYZEmail_t, XYZEmail> tmpVector( (XYZEmail_t**)&tmpList, NULL, 2 );

		for( int i = 0; i < 3; i++ ) {
			int ret = tmpVector.append( &email );
			if( 0 != ret ) {
				printf( "#%d append %d\n", i, ret );
			}
		}

		printf( "count %d\n", tmpVector.getCount() );
	}
}

int main( int argc, char * argv[] )
{
	if( argc < 2 ) {
		printf( "Usage: %s <type>\n\n", argv[0] );
		printf( "\ttype 0 - xml, 1 - xmlrpc, 2 - json, 3 - protobuf\n\n" );
		return -1;
	}

	int type = atoi( argv[1] );

	//SP_DPMetaUtils::dump( gXYZAddrbookMetaInfo );

	XYZAddrbookPickle xmlPickle( SP_DataPickle::eXml );
	XYZAddrbookPickle xmlRpcPickle( SP_DataPickle::eXmlRpc );
	XYZAddrbookPickle jsonPickle( SP_DataPickle::eJson );
	XYZAddrbookPickle pbPickle( SP_DataPickle::eProtoBuf );

	XYZAddrbookPickle * pickle = &xmlPickle;
	if( 1 == type ) pickle = &xmlRpcPickle;
	if( 2 == type ) pickle = &jsonPickle;
	if( 3 == type ) pickle = &pbPickle;

#if 0
	testEmail( pickle );

	testPhoneNumber( pickle );

	testContact( pickle );

	testUnpickle( pickle );

	testDeepCopy();
#endif

	testVector();

	return 0;
}

