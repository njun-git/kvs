//
//  GestureEventListener.h
//  
//
//  Created by Jun Nishimura on 10/19/11.
//  Copyright (c) 2011 Jun Nishimura. All rights reserved.
//

#ifndef KVS__OPENNI__GESTURE_EVENT_LISTENER_H_INCLUDE
#define KVS__OPENNI__GESTURE_EVENT_LISTENER_H_INCLUDE

#include <string>
#include <kvs/ClassName>
#include <kvs/Vector3>

#include "OpenNI.h"

namespace kvs
{

namespace ni
{

class GestureRecognizedEventListener
{

    // Class name.
    kvsClassName( kvs::ni::GestureRecognizedEventListener );

public:

    GestureRecognizedEventListener( void ) {}

    virtual ~GestureRecognizedEventListener( void ) {}

public:

    virtual void update(
        const std::string& gesture_name,
        const kvs::Vector3f& id_position,
        const kvs::Vector3f& end_position ) = 0;

};

class GestureProgressEventListener
{

    // Class name.
    kvsClassName( kvs::ni::GestureProgressEventListener );

public:

    GestureProgressEventListener( void ) {}

    virtual ~GestureProgressEventListener( void ) {}

public:

    virtual void update(
        const std::string& gesture_name,
        const kvs::Vector3f& position,
        const float progress ) = 0;

};

}

}



#endif
