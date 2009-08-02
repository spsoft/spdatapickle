/*
 * Copyright 2009 Stephen Liu
 * For license terms, see the file COPYING along with this library.
 */

#include <string.h>
#include <stdlib.h>

#include "dp_xyzaddrbook.hpp"

#include "spdpmetautils.hpp"
#include "spxmlpickle.hpp"
#include "spjsonpickle.hpp"
#include "spdpalloc.hpp"

#include "spxml/spxmlutils.hpp"
#include "spjson/spjsonutils.hpp"

void testEmail( SP_DataPickle * pickle )
{
	XYZEmail_t email;
	memset( &email, 0, sizeof( email ) );

	strncpy( email.mType, "work", sizeof( email.mType ) - 1 );
	email.mAddress = strdup( "foo <foo@bar.com>" );

	SP_XmlStringBuffer buffer;

	pickle->pickle( &email, sizeof( email ), eTypeXYZEmail, &buffer );

	printf( "%s\n\n", buffer.getBuffer() );

	SP_DPAlloc alloc( gXYZAddrbookMetaInfo );
	alloc.free( &email, sizeof( email ), eTypeXYZEmail );
}

void testPhoneNumber( SP_DataPickle * pickle )
{
	XYZPhoneNumber_t phoneNumber;
	memset( &phoneNumber, 0, sizeof( phoneNumber ) );

	strncpy( phoneNumber.mType, "home", sizeof( phoneNumber.mType ) - 1 );
	phoneNumber.mPrimary = 1;
	phoneNumber.mContent = strdup( "12345678" );

	SP_XmlStringBuffer buffer;
	pickle->pickle( &phoneNumber, sizeof( phoneNumber ), eTypeXYZPhoneNumber, &buffer );

	printf( "%s\n\n", buffer.getBuffer() );

	SP_DPAlloc alloc( gXYZAddrbookMetaInfo );
	alloc.free( &phoneNumber, sizeof( phoneNumber ), eTypeXYZPhoneNumber );
}

void initContact( XYZContact_t * contact )
{
	memset( contact, 0, sizeof( *contact ) );

	contact->mName = strdup( "foobar" );
	contact->mImageSize = 10;
	contact->mImageBuffer = (char*)malloc( contact->mImageSize );
	for( int i = 0; i < contact->mImageSize; i++ ) {
		contact->mImageBuffer[ i ] = i;
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
}

void testContact( SP_DataPickle * pickle )
{
	XYZContact_t contact;

	initContact( &contact );

	SP_XmlStringBuffer buffer;

	pickle->pickle( &contact, sizeof( contact ), eTypeXYZContact, &buffer );

	printf( "%s\n\n", buffer.getBuffer() );

	SP_DPAlloc alloc( gXYZAddrbookMetaInfo );
	alloc.free( &contact, sizeof( contact ), eTypeXYZContact );
}

void testUnpickle( SP_DataPickle * pickle )
{
	XYZContact_t org;

	initContact( &org );

	SP_XmlStringBuffer buffer;

	pickle->pickle( &org, sizeof( org ), eTypeXYZContact, &buffer );

	printf( "pickle: %s\n", buffer.getBuffer() );

	XYZContact_t contact;
	memset( &contact, 0, sizeof( contact ) );

	int ret = pickle->unpickle( buffer.getBuffer(), buffer.getSize(),
			eTypeXYZContact, &contact, sizeof( contact ) );

	printf( "Unpickle %d\n\n", ret );

	if( 0 != ret ) {
		printf( "Unpickle fail\n" );
		return;
	}

	printf( "name %s\n", contact.mName );
	printf( "image.size %d\n", contact.mImageSize );
	printf( "image.buffer: " );
	for( int i = 0; i < contact.mImageSize; i++ ) {
		printf( "%d ", contact.mImageBuffer[i] );
	}
	printf( "\n" );

	printf( "email.count %d\n", contact.mEmailCount );
	for( int i = 0; i < contact.mEmailCount; i++ ) {
		XYZEmail_t * email = contact.mEmailList + i;
		printf( "email#%d: %s, %s\n", i, email->mAddress, email->mType );
	}

	printf( "phone.count %d\n", contact.mPhoneCount );
	for( int i = 0; i < contact.mPhoneCount; i++ ) {
		XYZPhoneNumber_t * phone = contact.mPhoneList + i;
		printf( "phone#%d: %s, %s, %d\n", i, phone->mContent, phone->mType, phone->mPrimary );
	}

	SP_DPAlloc alloc( gXYZAddrbookMetaInfo );
	alloc.free( &contact, sizeof( contact ), eTypeXYZContact );
	alloc.free( &org, sizeof( org ), eTypeXYZContact );
}

int main( int argc, char * argv[] )
{
	SP_DPMetaUtils::dump( gXYZAddrbookMetaInfo );

	SP_XmlPickle  xmlPickle( gXYZAddrbookMetaInfo );
	SP_JsonPickle jsonPickle( gXYZAddrbookMetaInfo );

	SP_DataPickle * pickle = &jsonPickle;

	testEmail( pickle );

	testPhoneNumber( pickle );

	testContact( pickle );

	testUnpickle( pickle );

	return 0;
}

