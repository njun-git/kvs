//
//  HandEventListener.h
//  
//
//  Created by Jun Nishimura on 10/19/11.
//  Copyright (c) 2011 Jun Nishimura. All rights reserved.
//

#ifndef KVS__OPENNI__HAND_EVENT_LISTENER_H_INCLUDE
#define KVS__OPENNI__HAND_EVENT_LISTENER_H_INCLUDE

#include <string>
#include <kvs/ClassName>
#include <kvs/Vector3>

#include "OpenNI.h"

namespace kvs
{

namespace ni
{

class HandCreateEventListener
{

    // Class name.
    kvsClassName( kvs::ni::HandCreateEventListener );

public:

    HandCreateEventListener( void ) {}

    virtual ~HandCreateEventListener( void ) {}

public:

    virtual void update(
        const unsigned int user,
        const kvs::Vector3f& position,
        const float time ) = 0;

};

class HandUpdateEventListener
{

    // Class name.
    kvsClassName( kvs::ni::HandUpdateEventListener );

public:

    HandUpdateEventListener( void ) {}

    virtual ~HandUpdateEventListener( void ) {}

public:

    virtual void update(
        const unsigned int user,
        const kvs::Vector3f& position,
        const float time ) = 0;

};

class HandDestroyEventListener
{

    // Class name.
    kvsClassName( kvs::ni::HandDestroyEventListener );

public:

    HandDestroyEventListener( void ) {}

    virtual ~HandDestroyEventListener( void ) {}

public:

    virtual void update(
        const unsigned int user,
        const float time ) = 0;

};

}

}



#endif
