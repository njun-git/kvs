//
//  HandsGenerator.cpp
//  
//
//  Created by Jun Nishimura on 10/19/11.
//  Copyright (c) 2011 Jun Nishimura. All rights reserved.
//

#include "HandsGenerator.h"

#include "Context.h"

namespace { kvs::ni::HandsGenerator* context; }

namespace kvs
{

namespace ni
{

void XN_CALLBACK_TYPE HandCreateEvent(
    xn::HandsGenerator& generator,
    XnUserID user,
    const XnPoint3D* position,
    XnFloat time,
    void* cookie )
{
    ::context->handCreate( generator, user, position, time, cookie );
}

void XN_CALLBACK_TYPE HandUpdateEvent(
    xn::HandsGenerator& generator,
    XnUserID user,
    const XnPoint3D* position,
    XnFloat time,
    void* cookie )
{
    ::context->handUpdate( generator, user, position, time, cookie );
}

void XN_CALLBACK_TYPE HandDestroyEvent(
    xn::HandsGenerator& generator,
    XnUserID user,
    XnFloat time,
    void* cookie )
{
    ::context->handDestroy( generator, user, time, cookie );
}

HandsGenerator::HandsGenerator( void ) :
    kvs::ni::GeneratorBase()
{
    context = this;
}

HandsGenerator::~HandsGenerator( void )
{
}

const bool HandsGenerator::create( kvs::ni::Context& context )
{
    if ( m_is_created )
    {
        kvsMessageError( "HandsGenerator is already created." );
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
        XnStatus status = m_generator.RegisterHandCallbacks(
            HandCreateEvent, HandUpdateEvent, HandDestroyEvent, NULL, m_handler );
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

xn::HandsGenerator& HandsGenerator::generator( void )
{
    return( m_generator );
}

const bool HandsGenerator::startTracking( const kvs::Vector3f& position )
{
    if ( !m_is_created )
    {
        kvsMessageError( "HandsGenerator is not created yet." );
        return( false );
    }

    XnPoint3D pos;
    pos.X = position.x();
    pos.Y = position.y();
    pos.Z = position.z();

    XnStatus status = m_generator.StartTracking( pos );
    if ( status != XN_STATUS_OK )
    {
        kvsMessageError( "%s error: <%s>.",
            xnGetStatusName( status ),
            xnGetStatusString( status ) );
        return( false );
    }

    return( true );
}

const bool HandsGenerator::stopTracking( const unsigned int user )
{
    if ( !m_is_created )
    {
        kvsMessageError( "HandsGenerator is not created yet." );
        return( false );
    }

    XnUserID user_id = static_cast<XnUserID>( user );

    XnStatus status = m_generator.StopTracking( user_id );
    if ( status != XN_STATUS_OK )
    {
        kvsMessageError( "%s error: <%s>.",
            xnGetStatusName( status ),
            xnGetStatusString( status ) );
        return( false );
    }

    return( true );
}

const bool HandsGenerator::stopTrackingAll( void )
{
    if ( !m_is_created )
    {
        kvsMessageError( "HandsGenerator is not created yet." );
        return( false );
    }

    XnStatus status = m_generator.StopTrackingAll();
    if ( status != XN_STATUS_OK )
    {
        kvsMessageError( "%s error: <%s>.",
            xnGetStatusName( status ),
            xnGetStatusString( status ) );
        return( false );
    }

    return( true );
}

void HandsGenerator::addHandCreateEvent( kvs::ni::HandCreateEventListener* event )
{
    m_create_list.push_back( event );
}

void HandsGenerator::addHandUpdateEvent( kvs::ni::HandUpdateEventListener* event )
{
    m_update_list.push_back( event );
}

void HandsGenerator::addHandDestroyEvent( kvs::ni::HandDestroyEventListener* event )
{
    m_destroy_list.push_back( event );
}

void HandsGenerator::handCreate(
    xn::HandsGenerator& generator,
    XnUserID user,
    const XnPoint3D* position,
    XnFloat time,
    void* cookie )
{
    const unsigned int user_id = static_cast<unsigned int>( user );
    const kvs::Vector3f pos( position->X, position->Y, position->Z );
    const float ftime = static_cast<float>( time );

    CreateList::iterator itr = m_create_list.begin();
    CreateList::iterator end = m_create_list.end();
    while ( itr != end )
    {
        (*itr)->update( user_id, pos, ftime );
        itr++;
    }
}

void HandsGenerator::handUpdate(
    xn::HandsGenerator& generator,
    XnUserID user,
    const XnPoint3D* position,
    XnFloat time,
    void* cookie )
{
    const unsigned int user_id = static_cast<unsigned int>( user );
    const kvs::Vector3f pos( position->X, position->Y, position->Z );
    const float ftime = static_cast<float>( time );

    UpdateList::iterator itr = m_update_list.begin();
    UpdateList::iterator end = m_update_list.end();
    while ( itr != end )
    {
        (*itr)->update( user_id, pos, ftime );
        itr++;
    }
}

void HandsGenerator::handDestroy(
    xn::HandsGenerator& generator,
    XnUserID user,
    XnFloat time,
    void* cookie )
{
    const unsigned int user_id = static_cast<unsigned int>( user );
    const float ftime = static_cast<float>( time );

    DestroyList::iterator itr = m_destroy_list.begin();
    DestroyList::iterator end = m_destroy_list.end();
    while ( itr != end )
    {
        (*itr)->update( user_id, ftime );
        itr++;
    }
}

}

}
