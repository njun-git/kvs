//
//  UserGenerator.cpp
//  
//
//  Created by Jun Nishimura on 10/19/11.
//  Copyright (c) 2011 Jun Nishimura. All rights reserved.
//

#include "UserGenerator.h"

#include "Context.h"

namespace { kvs::ni::UserGenerator* context; }

namespace kvs
{

namespace ni
{

void XN_CALLBACK_TYPE NewUserEvent(
    xn::UserGenerator& generator,
    XnUserID user,
    void* cookie )
{
    ::context->newUser( generator, user, cookie );
}

void XN_CALLBACK_TYPE LostUserEvent(
    xn::UserGenerator& generator,
    XnUserID user,
    void* cookie )
{
    ::context->lostUser( generator, user, cookie );
}

UserGenerator::UserGenerator( void ) :
    kvs::ni::GeneratorBase()
{
    context = this;
}

UserGenerator::~UserGenerator( void )
{
}

const bool UserGenerator::create( kvs::ni::Context& context )
{
    if ( m_is_created )
    {
        kvsMessageError( "UserGenerator is already created." );
        return( false );
    }

    {
        XnStatus status = m_generator.Create( context.context() );
        if ( status != XN_STATUS_OK )
        {
            kvsMessageError( "%s error: <%s>.",
                xnGetStatusName( status ),
                xnGetStatusString( status ) );
            return( false );
        }
    }

    {
        XnStatus status = m_generator.RegisterUserCallbacks(
            NewUserEvent, LostUserEvent, NULL, m_handler );
        if ( status != XN_STATUS_OK )
        {
            kvsMessageError( "%s error: <%s>.",
                xnGetStatusName( status ),
                xnGetStatusString( status ) );
            return( false );
        }
    }

    m_is_created = true;
    return( true );
}

xn::UserGenerator& UserGenerator::generator( void )
{
    return( m_generator );
}

const unsigned short UserGenerator::nusers( void )
{
    if ( !m_is_created )
    {
        kvsMessageError( "UserGenerator is not created yet." );
        return( 0 );
    }

    return( static_cast<unsigned short>( m_generator.GetNumberOfUsers() ) );
}

const kvs::ValueArray<unsigned int> UserGenerator::users( void )
{
    if ( !m_is_created )
    {
        kvsMessageError( "UserGenerator is not created yet." );
        return( kvs::ValueArray<unsigned int>( 0 ) );
    }

    XnUInt16  nusers;
    XnUserID* users;

    XnStatus status = m_generator.GetUsers( users, nusers );
    if ( status != XN_STATUS_OK )
    {
        kvsMessageError( "%s error: <%s>.",
            xnGetStatusName( status ),
            xnGetStatusString( status ) );
        return( kvs::ValueArray<unsigned int>( 0 ) );
    }

    return( kvs::ValueArray<unsigned int>( users, nusers ) );
}

const kvs::Vector3f UserGenerator::centerOfMass( const unsigned int user )
{
    if ( !m_is_created )
    {
        kvsMessageError( "UserGenerator is not created yet." );
        return( kvs::Vector3f( 0.0f, 0.0f, 0.0f ) );
    }

    XnPoint3D com;

    XnStatus status = m_generator.GetCoM( static_cast<XnUserID>( user ), com );
    if ( status != XN_STATUS_OK )
    {
        kvsMessageError( "%s error: <%s>.",
            xnGetStatusName( status ),
            xnGetStatusString( status ) );
        return( kvs::Vector3f( 0.0f, 0.0f, 0.0f ) );
    }

    return( kvs::Vector3f( com.X, com.Y, com.Z ) );
}

const unsigned short* UserGenerator::pointer( const unsigned int user )
{
    if ( !m_is_created )
    {
        kvsMessageError( "UserGenerator is not created yet." );
        return( NULL );
    }

    xn::SceneMetaData data;

    XnStatus status = m_generator.GetUserPixels( static_cast<XnUserID>( user ), data );
    if ( status != XN_STATUS_OK )
    {
        kvsMessageError( "%s error: <%s>.",
            xnGetStatusName( status ),
            xnGetStatusString( status ) );
        return( NULL );
    }

    return( data.Data() );
}

void UserGenerator::addNewUserEvent( kvs::ni::NewUserEventListener* event )
{
    m_new_list.push_back( event );
}

void UserGenerator::addLostUserEvent( kvs::ni::LostUserEventListener* event )
{
    m_lost_list.push_back( event );
}

void UserGenerator::newUser(
    xn::UserGenerator& generator,
    XnUserID user,
    void* cookie )
{
    const unsigned int user_id = static_cast<unsigned int>( user );
    NewUserList::iterator itr = m_new_list.begin();
    NewUserList::iterator end = m_new_list.end();
    while ( itr != end )
    {
        (*itr)->update( user_id );
        itr++;
    }
}

void UserGenerator::lostUser(
    xn::UserGenerator& generator,
    XnUserID user,
    void* cookie )
{
    const unsigned int user_id = static_cast<unsigned int>( user );
    LostUserList::iterator itr = m_lost_list.begin();
    LostUserList::iterator end = m_lost_list.end();
    while ( itr != end )
    {
        (*itr)->update( user_id );
        itr++;
    }
}

}

}
