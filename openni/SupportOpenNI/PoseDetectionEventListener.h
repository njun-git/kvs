//
//  PoseDetectionEventListener.h
//  
//
//  Created by Jun Nishimura on 10/24/11.
//  Copyright (c) 2011 Jun Nishimura. All rights reserved.
//

#ifndef KVS__OPENNI__POSE_DETECTION_EVENT_LISTENER_H_INCLUDE
#define KVS__OPENNI__POSE_DETECTION_EVENT_LISTENER_H_INCLUDE

#include <kvs/ClassName>
#include <string>

#include "OpenNI.h"

namespace kvs
{

namespace ni
{

class PoseStartEventListener
{

    // Class name.
    kvsClassName( kvs::ni::PoseStartEventListener );

public:

    PoseStartEventListener( void ) {}

    virtual ~PoseStartEventListener( void ) {}

public:

    virtual void update(
        const std::string& pose_name,
        const unsigned int user ) = 0;

};

class PoseEndEventListener
{

    // Class name.
    kvsClassName( kvs::ni::PoseEndEventListener );

public:

    PoseEndEventListener( void ) {}

    virtual ~PoseEndEventListener( void ) {}

public:

    virtual void update(
        const std::string& pose_name,
        const unsigned int user ) = 0;

};

}

}

#endif
