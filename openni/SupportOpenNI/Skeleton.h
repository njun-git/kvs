//
//  Skeleton.h
//  
//
//  Created by Jun Nishimura on 10/22/11.
//  Copyright (c) 2011 Jun Nishimura. All rights reserved.
//

#ifndef KVS__OPENNI__SKELETON_H_INCLUDE
#define KVS__OPENNI__SKELETON_H_INCLUDE

#include <kvs/ClassName>
#include <kvs/ValueArray>
#include <kvs/Vector3>
#include <kvs/Matrix33>
#include <list>
#include <string>

#include "SkeletonEventListener.h"

namespace kvs
{

namespace ni
{

class JointPosition
{

    // Class name.
    kvsClassName( kvs::ni::JointPosition );

protected:

    float           m_confidence;
    kvs::Vector3f   m_position;

public:

    JointPosition( void )
    {
        m_confidence = 0.0f;
        m_position.set( 0.0f, 0.0f, 0.0f );
    }

    virtual ~JointPosition( void ) {}

public:

    const float confidence( void ) const { return( m_confidence ); }

    const kvs::Vector3f& position( void ) const { return( m_position ); }

public:

    void setJointPosition( XnSkeletonJointPosition& position )
    {
        m_confidence = position.fConfidence;
        m_position.set( position.position.X, position.position.Y, position.position.Z );
    }

};

class JointOrientation
{

    // Class name.
    kvsClassName( kvs::ni::JointOrientation );

protected:

    float           m_confidence;
    kvs::Matrix33f  m_orientation;

public:

    JointOrientation( void )
    {
        m_confidence = 0.0f;
        m_orientation.identity();
    }

    virtual ~JointOrientation( void ) {}

public:

    const float confidence( void ) const { return( m_confidence ); }

    const kvs::Matrix33f& orientation( void ) const { return( m_orientation ); }

public:

    void setJointOrientation( XnSkeletonJointOrientation& orientation )
    {
        m_confidence = orientation.fConfidence;
        m_orientation.set( static_cast<float*>( orientation.orientation.elements ) );
    }

};

class JointTransform
{

    // Class name.
    kvsClassName( kvs::ni::JointTransform );

protected:

    kvs::ni::JointPosition      m_position;
    kvs::ni::JointOrientation   m_orientation;

public:

    JointTransform( void ) {}

    virtual ~JointTransform( void ) {}

public:

    const kvs::ni::JointPosition& position( void ) const { return( m_position ); }

    const kvs::ni::JointOrientation& orientation( void ) const { return( m_orientation ); }

public:

    void setJointTransform( XnSkeletonJointTransformation& transform )
    {
        m_position.setJointPosition( transform.position );
        m_orientation.setJointOrientation( transform.orientation );
    }

};

class UserGenerator;

class Skeleton
{

    // Class name.
    kvsClassName( kvs::ni::Skeleton );

    typedef std::list<kvs::ni::CalibrationStartEventListener*>  StartList;
    typedef std::list<kvs::ni::CalibrationEndEventListener*>    EndList;

public:

    enum SkeletonJoint
    {
        Head            = XN_SKEL_HEAD,
        Neck            = XN_SKEL_NECK,
        Torso           = XN_SKEL_TORSO,
        Waist           = XN_SKEL_WAIST,
        LeftCollar      = XN_SKEL_LEFT_COLLAR,
        LeftShoulder    = XN_SKEL_LEFT_SHOULDER,
        LeftElbow       = XN_SKEL_LEFT_ELBOW,
        LeftWrist       = XN_SKEL_LEFT_WRIST,
        LeftHand        = XN_SKEL_LEFT_HAND,
        LeftFingertip   = XN_SKEL_LEFT_FINGERTIP,
        RightCollar     = XN_SKEL_RIGHT_COLLAR,
        RightShoulder   = XN_SKEL_RIGHT_SHOULDER,
        RightElbow      = XN_SKEL_RIGHT_ELBOW,
        RightWrist      = XN_SKEL_RIGHT_WRIST,
        RightHand       = XN_SKEL_RIGHT_HAND,
        RightFingertip  = XN_SKEL_RIGHT_FINGERTIP,
        LeftHip         = XN_SKEL_LEFT_HIP,
        LeftKnee        = XN_SKEL_LEFT_KNEE,
        LeftAnkle       = XN_SKEL_LEFT_ANKLE,
        LeftFoot        = XN_SKEL_LEFT_FOOT,
        RightHip        = XN_SKEL_RIGHT_HIP,
        RightKnee       = XN_SKEL_RIGHT_KNEE,
        RightAnkle      = XN_SKEL_RIGHT_ANKLE,
        RightFoot       = XN_SKEL_RIGHT_FOOT,
    };

protected:

    bool                    m_is_attached;
    xn::SkeletonCapability  m_skeleton;
    XnCallbackHandle        m_handler;
    StartList               m_start_list;
    EndList                 m_end_list;

    kvs::ValueArray<SkeletonJoint>  m_available_joints;

public:

    Skeleton( void );

    Skeleton( kvs::ni::UserGenerator* generator );

    virtual ~Skeleton( void );

public:

    const bool initialize( void );

    void attachUserGenerator( kvs::ni::UserGenerator* generator );

public:

    void addCalibrationStartEvent( kvs::ni::CalibrationStartEventListener* event );

    void addCalibrationEndEvent( kvs::ni::CalibrationEndEventListener* event );

public:

    xn::SkeletonCapability& skeleton( void );

    kvs::ValueArray<SkeletonJoint>& availableJoints( void );

public:

    const bool isCalibrated( const unsigned int user );

    const bool isCalibrating( const unsigned int user );

    const bool isTracking( const unsigned int user );

    const JointTransform jointTransform( const int user, const SkeletonJoint& joint );

    const JointPosition jointPosition( const int user, const SkeletonJoint& joint );

    const JointOrientation jointOrientation( const int user, const SkeletonJoint& joint );

    const bool startTracking( const unsigned int user );

    const bool stopTracking( const unsigned int user );

    const bool requestCalibration( const unsigned int user, const bool is_force );

    const bool readCalibrationData( const unsigned int user, const std::string& filename );

    const bool writeCalibrationData( const unsigned int user, const std::string& filename );

protected:

    const bool get_all_active_joints( void );

public:

    void calibrationStart(
        xn::SkeletonCapability& skeleton,
        XnUserID user,
        void* cookie );

    void calibrationEnd(
        xn::SkeletonCapability& skeleton,
        XnUserID user,
        XnBool is_success,
        void* cookie );

};

inline std::string JointStrting( const kvs::ni::Skeleton::SkeletonJoint& joint )
{
    switch ( joint )
    {
        case kvs::ni::Skeleton::Head:           return( "Head" ); break;
        case kvs::ni::Skeleton::Neck:           return( "Neck" ); break;
        case kvs::ni::Skeleton::Torso:          return( "Torso" ); break;
        case kvs::ni::Skeleton::Waist:          return( "Waist" ); break;
        case kvs::ni::Skeleton::LeftCollar:     return( "LeftCollar" ); break;
        case kvs::ni::Skeleton::LeftShoulder:   return( "LeftShoulder" ); break;
        case kvs::ni::Skeleton::LeftElbow:      return( "LeftElbow" ); break;
        case kvs::ni::Skeleton::LeftWrist:      return( "LeftWrist" ); break;
        case kvs::ni::Skeleton::LeftHand:       return( "LeftHand" ); break;
        case kvs::ni::Skeleton::LeftFingertip:  return( "LeftFingertip" ); break;
        case kvs::ni::Skeleton::RightCollar:    return( "RightCollar" ); break;
        case kvs::ni::Skeleton::RightShoulder:  return( "RightShoulder" ); break;
        case kvs::ni::Skeleton::RightElbow:     return( "RightElbow" ); break;
        case kvs::ni::Skeleton::RightWrist:     return( "RightWrist" ); break;
        case kvs::ni::Skeleton::RightHand:      return( "RightHand" ); break;
        case kvs::ni::Skeleton::RightFingertip: return( "RightFingertip" ); break;
        case kvs::ni::Skeleton::LeftHip:        return( "LeftHip" ); break;
        case kvs::ni::Skeleton::LeftKnee:       return( "LeftKnee" ); break;
        case kvs::ni::Skeleton::LeftAnkle:      return( "LeftAnkle" ); break;
        case kvs::ni::Skeleton::LeftFoot:       return( "LeftFoot" ); break;
        case kvs::ni::Skeleton::RightHip:       return( "RightHip" ); break;
        case kvs::ni::Skeleton::RightKnee:      return( "RightKnee" ); break;
        case kvs::ni::Skeleton::RightAnkle:     return( "RightAnkle" ); break;
        case kvs::ni::Skeleton::RightFoot:      return( "RightFoot" ); break;
        default: break;
    }
    return( "UnknownJoint" );
}

}

}

#endif
