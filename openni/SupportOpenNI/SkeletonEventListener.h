//
//  SkeletonEventListener.h
//  
//
//  Created by Jun Nishimura on 10/22/11.
//  Copyright (c) 2011 Jun Nishimura. All rights reserved.
//

#ifndef KVS__OPENNI__SKELETON_EVENT_LISTENER_H_INCLUDE
#define KVS__OPENNI__SKELETON_EVENT_LISTENER_H_INCLUDE

#include <kvs/ClassName>

#include "OpenNI.h"

namespace kvs
{

namespace ni
{

class CalibrationStartEventListener
{

    // Class name.
    kvsClassName( kvs::ni::CalibrationStartEventListener );

public:

    CalibrationStartEventListener( void ) {}

    virtual ~CalibrationStartEventListener( void ) {}

public:

    virtual void update(
        const unsigned int user ) = 0;

};

class CalibrationEndEventListener
{

    // Class name.
    kvsClassName( kvs::ni::CalibrationEndEventListener );

public:

    CalibrationEndEventListener( void ) {}

    virtual ~CalibrationEndEventListener( void ) {}

public:

    virtual void update(
        const unsigned int user,
        const bool is_success ) = 0;

};

}

}

#endif
