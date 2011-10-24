//
//  PoseDetection.cpp
//  
//
//  Created by Jun Nishimura on 10/24/11.
//  Copyright (c) 2011 Jun Nishimura. All rights reserved.
//

#include "PoseDetection.h"

#include "UserGenerator.h"

namespace { kvs::ni::PoseDetection* context; }

namespace kvs
{

namespace ni
{

void XN_CALLBACK_TYPE PoseStart(
    xn::PoseDetectionCapability& pose,
    const XnChar* str_pose,
    XnUserID user,
    void* cookie )
{
    ::context->poseStart( pose, str_pose, user, cookie );
}

void XN_CALLBACK_TYPE PoseEnd(
    xn::PoseDetectionCapability& pose,
    const XnChar* str_pose,
    XnUserID user,
    void* cookie )
{
    ::context->poseEnd( pose, str_pose, user, cookie );
}

PoseDetection::PoseDetection( void ) :
    m_is_attached( false ),
    m_pose( NULL )
{
    ::context = this;
}

PoseDetection::PoseDetection( kvs::ni::UserGenerator* generator ) :
    m_is_attached( false ),
    m_pose( NULL )
{
    ::context = this;
    this->attachUserGenerator( generator );
    this->initialize();
}

PoseDetection::~PoseDetection( void )
{
}

const bool PoseDetection::initialize( void )
{
    if ( !m_is_attached )
    {
        kvsMessageError( "kvs::ni::UserGenerator* is not attached." );
        return( false );
    }

    XnStatus status = m_pose.RegisterToPoseCallbacks( PoseStart, PoseEnd, NULL, m_handler );
    if ( status != XN_STATUS_OK )
    {
        kvsMessageError( "%s error: <%s>.",
            xnGetStatusName( status ),
            xnGetStatusString( status ) );
        return( false );
    }

    return( true );
}

void PoseDetection::attachUserGenerator( kvs::ni::UserGenerator* generator )
{
    m_pose = generator->generator().GetPoseDetectionCap();
    m_is_attached = true;
}

void PoseDetection::addPoseStartEvent( kvs::ni::PoseStartEventListener* event )
{
    m_start_list.push_back( event );
}

void PoseDetection::addPoseEndEvent( kvs::ni::PoseEndEventListener* event )
{
    m_end_list.push_back( event );
}

xn::PoseDetectionCapability& PoseDetection::pose( void )
{
    return( m_pose );
}

const bool PoseDetection::startDetection( const std::string& pose, const unsigned int user )
{
    if ( !m_is_attached )
    {
        kvsMessageError( "kvs::ni::UserGenerator* is not attached." );
        return( false );
    }

    XnStatus status = m_pose.StartPoseDetection(
        static_cast<const XnChar*>( pose.c_str() ), static_cast<XnUserID>( user ) );
    if ( status != XN_STATUS_OK )
    {
        kvsMessageError( "%s error: <%s>.",
            xnGetStatusName( status ),
            xnGetStatusString( status ) );
        return( false );
    }

    return( true );
}

const bool PoseDetection::stopDetection( const unsigned int user )
{
    if ( !m_is_attached )
    {
        kvsMessageError( "kvs::ni::UserGenerator* is not attached." );
        return( false );
    }

    XnStatus status = m_pose.StopPoseDetection(
        static_cast<XnUserID>( user ) );
    if ( status != XN_STATUS_OK )
    {
        kvsMessageError( "%s error: <%s>.",
            xnGetStatusName( status ),
            xnGetStatusString( status ) );
        return( false );
    }

    return( true );
}

void PoseDetection::poseStart(
    xn::PoseDetectionCapability& pose,
    const XnChar* str_pose,
    XnUserID user,
    void* cookie )
{
    const std::string pose_name( str_pose );
    const unsigned int user_id = static_cast<unsigned int>( user );
    StartList::iterator itr = m_start_list.begin();
    StartList::iterator end = m_start_list.end();
    while ( itr != end )
    {
        (*itr)->update( pose_name, user_id );
        itr++;
    }
}

void PoseDetection::poseEnd(
    xn::PoseDetectionCapability& pose,
    const XnChar* str_pose,
    XnUserID user,
    void* cookie )
{
    const std::string pose_name( str_pose );
    const unsigned int user_id = static_cast<unsigned int>( user );
    EndList::iterator itr = m_end_list.begin();
    EndList::iterator end = m_end_list.end();
    while ( itr != end )
    {
        (*itr)->update( pose_name, user_id );
        itr++;
    }
}

}

}
