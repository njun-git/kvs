//
//  HandsGenerator.h
//  
//
//  Created by Jun Nishimura on 10/19/11.
//  Copyright (c) 2011 Jun Nishimura. All rights reserved.
//

#ifndef KVS__OPENNI__HANDS_GESTURE_H_INCLUDE
#define KVS__OPENNI__HANDS_GESTURE_H_INCLUDE

#include <kvs/ClassName>
#include <list>

#include "GeneratorBase.h"
#include "HandEventListener.h"

namespace kvs
{

namespace ni
{

class HandsGenerator : public kvs::ni::GeneratorBase
{

    // Class name.
    kvsClassName( kvs::ni::HandsGenerator );

    typedef kvs::ni::GeneratorBase BaseClass;
    typedef std::list<kvs::ni::HandCreateEventListener*>  CreateList;
    typedef std::list<kvs::ni::HandUpdateEventListener*>  UpdateList;
    typedef std::list<kvs::ni::HandDestroyEventListener*> DestroyList;

protected:

    xn::HandsGenerator  m_generator;
    XnCallbackHandle    m_handler;
    CreateList          m_create_list;
    UpdateList          m_update_list;
    DestroyList         m_destroy_list;

public:

    HandsGenerator( void );

    virtual ~HandsGenerator( void );

public:

    const bool create( kvs::ni::Context& context );

public:

    xn::HandsGenerator& generator( void );

public:

    const bool startTracking( const kvs::Vector3f& position );

    const bool stopTracking( const unsigned int user );

    const bool stopTrackingAll( void );

    void addHandCreateEvent( kvs::ni::HandCreateEventListener* event );

    void addHandUpdateEvent( kvs::ni::HandUpdateEventListener* event );

    void addHandDestroyEvent( kvs::ni::HandDestroyEventListener* event );

public:

    void handCreate(
        xn::HandsGenerator& generator,
        XnUserID user,
        const XnPoint3D* position,
        XnFloat time,
        void* cookie );

    void handUpdate(
        xn::HandsGenerator& generator,
        XnUserID user,
        const XnPoint3D* position,
        XnFloat time,
        void* cookie );

    void handDestroy(
        xn::HandsGenerator& generator,
        XnUserID user,
        XnFloat time,
        void* cookie );

};

}

}

#endif
