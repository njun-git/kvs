//
//  UserEventListener.h
//  
//
//  Created by Jun Nishimura on 10/20/11.
//  Copyright (c) 2011 Jun Nishimura. All rights reserved.
//

#ifndef KVS__OPENNI__USER_EVENT_LISTENER_H_INCLUDE
#define KVS__OPENNI__USER_EVENT_LISTENER_H_INCLUDE

#include <string>
#include <kvs/ClassName>
#include <kvs/Vector3>

#include "OpenNI.h"

namespace kvs
{

namespace ni
{

class NewUserEventListener
{

    // Class name.
    kvsClassName( kvs::ni::NewUserEventListener );

public:

    NewUserEventListener( void ) {}

    virtual ~NewUserEventListener( void ) {}

public:

    virtual void update(
        const unsigned int user ) = 0;

};

class LostUserEventListener
{

    // Class name.
    kvsClassName( kvs::ni::LostUserEventListener );

public:

    LostUserEventListener( void ) {}

    virtual ~LostUserEventListener( void ) {}

public:

    virtual void update(
        const unsigned int user ) = 0;

};

}

}

#endif
