//
//  Skeleton.cpp
//  
//
//  Created by Jun Nishimura on 10/22/11.
//  Copyright (c) 2011 Jun Nishimura. All rights reserved.
//

#include "Skeleton.h"

#include "UserGenerator.h"

namespace { kvs::ni::Skeleton* context; }

namespace kvs
{

namespace ni
{

void XN_CALLBACK_TYPE CalibrationStart(
    xn::SkeletonCapability& skeleton,
    XnUserID user,
    void* cookie )
{
    ::context->calibrationStart( skeleton, user, cookie );
}

void XN_CALLBACK_TYPE CalibrationEnd(
    xn::SkeletonCapability& skeleton,
    XnUserID user,
    XnBool is_success,
    void* cookie )
{
    ::context->calibrationEnd( skeleton, user, is_success, cookie );
}

Skeleton::Skeleton( void ) :
    m_is_attached( false ),
    m_skeleton( NULL )
{
    ::context = this;
}

Skeleton::Skeleton( kvs::ni::UserGenerator* generator ) :
    m_is_attached( false ),
    m_skeleton( NULL )
{
    ::context = this;
    this->attachUserGenerator( generator );
    this->initialize();
}

Skeleton::~Skeleton( void )
{
}

const bool Skeleton::initialize( void )
{
    if ( !m_is_attached )
    {
        kvsMessageError( "kvs::ni::UserGenerator* is not attached." );
        return( false );
    }

    {
        XnStatus status = m_skeleton.RegisterCalibrationCallbacks( CalibrationStart, CalibrationEnd, NULL, m_handler );
        if ( status != XN_STATUS_OK )
        {
            kvsMessageError( "%s error: <%s>.",
                xnGetStatusName( status ),
                xnGetStatusString( status ) );
            return( false );
        }
    }

    {
        XnStatus status = m_skeleton.SetSkeletonProfile( XN_SKEL_PROFILE_ALL );
        if ( status != XN_STATUS_OK )
        {
            kvsMessageError( "%s error: <%s>.",
                xnGetStatusName( status ),
                xnGetStatusString( status ) );
            return( false );
        }
    }

    return( true );
}

void Skeleton::attachUserGenerator( kvs::ni::UserGenerator* generator )
{
    m_skeleton = generator->generator().GetSkeletonCap();
    m_is_attached = true;
}

void Skeleton::addCalibrationStartEvent( kvs::ni::CalibrationStartEventListener* event )
{
    m_start_list.push_back( event );
}

void Skeleton::addCalibrationEndEvent( kvs::ni::CalibrationEndEventListener* event )
{
    m_end_list.push_back( event );
}

xn::SkeletonCapability& Skeleton::skeleton( void )
{
    return( m_skeleton );
}

kvs::ValueArray<Skeleton::SkeletonJoint>& Skeleton::availableJoints( void )
{
    if ( m_available_joints.size() == 0 )
    {
        this->get_all_active_joints();
    }

    return( m_available_joints );
}

const bool Skeleton::isCalibrated( const unsigned int user )
{
    if ( !m_is_attached )
    {
        kvsMessageError( "kvs::ni::UserGenerator* is not attached." );
        return( false );
    }

    return( m_skeleton.IsCalibrated( static_cast<XnUserID>( user ) ) );
}

const bool Skeleton::isCalibrating( const unsigned int user )
{
    if ( !m_is_attached )
    {
        kvsMessageError( "kvs::ni::UserGenerator* is not attached." );
        return( false );
    }

    return( m_skeleton.IsCalibrating( static_cast<XnUserID>( user ) ) );
}

const bool Skeleton::isTracking( const unsigned int user )
{
    if ( !m_is_attached )
    {
        kvsMessageError( "kvs::ni::UserGenerator* is not attached." );
        return( false );
    }

    return( m_skeleton.IsTracking( static_cast<XnUserID>( user ) ) );
}

const JointTransform Skeleton::jointTransform( const int user, const Skeleton::SkeletonJoint& joint )
{
    JointTransform transform;
    if ( !m_is_attached )
    {
        kvsMessageError( "kvs::ni::UserGenerator* is not attached." );
        return( transform );
    }

    XnSkeletonJointTransformation xn_transform;
    
    XnStatus status = m_skeleton.GetSkeletonJoint(
        static_cast<XnUserID>( user ), static_cast<XnSkeletonJoint>( joint ), xn_transform );
    if ( status != XN_STATUS_OK )
    {
        kvsMessageError( "%s error: <%s>.",
            xnGetStatusName( status ),
            xnGetStatusString( status ) );
        return( transform );
    }

    transform.setJointTransform( xn_transform );
    return( transform );
}

const JointPosition Skeleton::jointPosition( const int user, const Skeleton::SkeletonJoint& joint )
{
    JointPosition position;
    if ( !m_is_attached )
    {
        kvsMessageError( "kvs::ni::UserGenerator* is not attached." );
        return( position );
    }

    XnSkeletonJointPosition xn_position;
    
    XnStatus status = m_skeleton.GetSkeletonJointPosition(
        static_cast<XnUserID>( user ), static_cast<XnSkeletonJoint>( joint ), xn_position );
    if ( status != XN_STATUS_OK )
    {
        kvsMessageError( "%s error: <%s>.",
            xnGetStatusName( status ),
            xnGetStatusString( status ) );
        return( position );
    }

    position.setJointPosition( xn_position );
    return( position );
}

const JointOrientation Skeleton::jointOrientation( const int user, const Skeleton::SkeletonJoint& joint )
{
    JointOrientation orientation;
    if ( !m_is_attached )
    {
        kvsMessageError( "kvs::ni::UserGenerator* is not attached." );
        return( orientation );
    }

    XnSkeletonJointOrientation xn_orientation;
    
    XnStatus status = m_skeleton.GetSkeletonJointOrientation(
        static_cast<XnUserID>( user ), static_cast<XnSkeletonJoint>( joint ), xn_orientation );
    if ( status != XN_STATUS_OK )
    {
        kvsMessageError( "%s error: <%s>.",
            xnGetStatusName( status ),
            xnGetStatusString( status ) );
        return( orientation );
    }

    orientation.setJointOrientation( xn_orientation );
    return( orientation );
}

const bool Skeleton::setSmoothing( const float smoothing )
{
    if ( !m_is_attached )
    {
        kvsMessageError( "kvs::ni::UserGenerator* is not attached." );
        return( false );
    }

    XnStatus status = m_skeleton.SetSmoothing(
        static_cast<XnFloat>( smoothing ) );
    if ( status != XN_STATUS_OK )
    {
        kvsMessageError( "%s error: <%s>.",
            xnGetStatusName( status ),
            xnGetStatusString( status ) );
        return( false );
    }

    return( true );
}

const bool Skeleton::startTracking( const unsigned int user )
{
    if ( !m_is_attached )
    {
        kvsMessageError( "kvs::ni::UserGenerator* is not attached." );
        return( false );
    }

    XnStatus status = m_skeleton.StartTracking(
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

const bool Skeleton::stopTracking( const unsigned int user )
{
    if ( !m_is_attached )
    {
        kvsMessageError( "kvs::ni::UserGenerator* is not attached." );
        return( false );
    }

    XnStatus status = m_skeleton.StopTracking(
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

const bool Skeleton::requestCalibration( const unsigned int user, const bool is_force )
{
    if ( !m_is_attached )
    {
        kvsMessageError( "kvs::ni::UserGenerator* is not attached." );
        return( false );
    }

    XnStatus status = m_skeleton.RequestCalibration(
        static_cast<XnUserID>( user ), static_cast<XnBool>( is_force ) );
    if ( status != XN_STATUS_OK )
    {
        kvsMessageError( "%s error: <%s>.",
            xnGetStatusName( status ),
            xnGetStatusString( status ) );
        return( false );
    }

    return( true );
}

const bool Skeleton::readCalibrationData( const unsigned int user, const std::string& filename )
{
    if ( !m_is_attached )
    {
        kvsMessageError( "kvs::ni::UserGenerator* is not attached." );
        return( false );
    }

    XnStatus status = m_skeleton.LoadCalibrationDataFromFile(
        static_cast<XnUserID>( user ), static_cast<const XnChar*>( filename.c_str() ) );
    if ( status != XN_STATUS_OK )
    {
        kvsMessageError( "%s error: <%s>.",
            xnGetStatusName( status ),
            xnGetStatusString( status ) );
        return( false );
    }

    return( true );
}

const bool Skeleton::writeCalibrationData( const unsigned int user, const std::string& filename )
{
    if ( !m_is_attached )
    {
        kvsMessageError( "kvs::ni::UserGenerator* is not attached." );
        return( false );
    }

    XnStatus status = m_skeleton.SaveCalibrationDataToFile(
        static_cast<XnUserID>( user ), static_cast<const XnChar*>( filename.c_str() ) );
    if ( status != XN_STATUS_OK )
    {
        kvsMessageError( "%s error: <%s>.",
            xnGetStatusName( status ),
            xnGetStatusString( status ) );
        return( false );
    }

    return( true );
}

const bool Skeleton::get_all_active_joints( void )
{
    if ( !m_is_attached )
    {
        kvsMessageError( "kvs::ni::UserGenerator* is not attached." );
        return( false );
    }

    XnUInt16 njoints = 24;
    XnSkeletonJoint pjoints[24];

    XnStatus status = m_skeleton.EnumerateActiveJoints( pjoints, njoints );
    if ( status != XN_STATUS_OK )
    {
        kvsMessageError( "%s error: <%s>.",
            xnGetStatusName( status ),
            xnGetStatusString( status ) );
        return( false );
    }

    m_available_joints.allocate( static_cast<size_t>( njoints ) );
    for ( size_t i = 0; i < m_available_joints.size(); i++ )
    {
        m_available_joints[i] = static_cast<SkeletonJoint>( pjoints[i] );
    }

    return( true );
}

void Skeleton::calibrationStart(
    xn::SkeletonCapability& skeleton,
    XnUserID user,
    void* cookie )
{
    const unsigned int user_id = static_cast<unsigned int>( user );
    StartList::iterator itr = m_start_list.begin();
    StartList::iterator end = m_start_list.end();
    while ( itr != end )
    {
        (*itr)->update( user_id );
        itr++;
    }
}

void Skeleton::calibrationEnd(
    xn::SkeletonCapability& skeleton,
    XnUserID user,
    XnBool is_success,
    void* cookie )
{
    const unsigned int user_id = static_cast<unsigned int>( user );
    const bool success = static_cast<bool>( is_success );
    EndList::iterator itr = m_end_list.begin();
    EndList::iterator end = m_end_list.end();
    while ( itr != end )
    {
        (*itr)->update( user_id, success );
        itr++;
    }
}

}

}
