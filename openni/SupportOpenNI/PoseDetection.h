//
//  PoseDetection.h
//  
//
//  Created by Jun Nishimura on 10/24/11.
//  Copyright (c) 2011 Jun Nishimura. All rights reserved.
//

#ifndef KVS__OPENNI__POSE_DETECTION_H_INCLUDE
#define KVS__OPENNI__POSE_DETECTION_H_INCLUDE

#include <kvs/ClassName>
#include <list>
#include <string>

#include "PoseDetectionEventListener.h"

namespace kvs
{

namespace ni
{

class UserGenerator;

class PoseDetection
{

    // Class name.
    kvsClassName( kvs::ni::Skeleton );

    typedef std::list<kvs::ni::PoseStartEventListener*>  StartList;
    typedef std::list<kvs::ni::PoseEndEventListener*>    EndList;

protected:

    bool                        m_is_attached;
    xn::PoseDetectionCapability m_pose;
    XnCallbackHandle            m_handler;
    StartList                   m_start_list;
    EndList                     m_end_list;

public:

    PoseDetection( void );

    PoseDetection( kvs::ni::UserGenerator* generator );

    virtual ~PoseDetection( void );

public:

    const bool initialize( void );

    void attachUserGenerator( kvs::ni::UserGenerator* generator );

public:

    void addPoseStartEvent( kvs::ni::PoseStartEventListener* event );

    void addPoseEndEvent( kvs::ni::PoseEndEventListener* event );

public:

    xn::PoseDetectionCapability& pose( void );

public:

    const bool startDetection( const std::string& pose, const unsigned int user );

    const bool stopDetection( const unsigned int user );

public:

    void poseStart(
        xn::PoseDetectionCapability& pose,
        const XnChar* str_pose,
        XnUserID user,
        void* cookie );

    void poseEnd(
        xn::PoseDetectionCapability& pose,
        const XnChar* str_pose,
        XnUserID user,
        void* cookie );

};

}

}

#endif
