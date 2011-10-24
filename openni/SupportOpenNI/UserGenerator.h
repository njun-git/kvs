//
//  UserGenerator.h
//  
//
//  Created by Jun Nishimura on 10/19/11.
//  Copyright (c) 2011 Jun Nishimura. All rights reserved.
//

#ifndef KVS__OPENNI__USER_GENERATOR_H_INCLUDE
#define KVS__OPENNI__USER_GENERATOR_H_INCLUDE

#include <kvs/ClassName>
#include <kvs/ValueArray>
#include <list>

#include "GeneratorBase.h"
#include "UserEventListener.h"
#include "Skeleton.h"
#include "PoseDetection.h"

namespace kvs
{

namespace ni
{

class UserGenerator : public kvs::ni::GeneratorBase
{

    // Class name.
    kvsClassName( kvs::ni::UserGenerator );

    typedef kvs::ni::GeneratorBase BaseClass;
    typedef std::list<kvs::ni::NewUserEventListener*>  NewUserList;
    typedef std::list<kvs::ni::LostUserEventListener*> LostUserList;

protected:

    xn::UserGenerator   m_generator;
    XnCallbackHandle    m_handler;
    NewUserList         m_new_list;
    LostUserList        m_lost_list;

    kvs::ni::Skeleton       m_skeleton;
    kvs::ni::PoseDetection  m_pose;

public:

    UserGenerator( void );

    virtual ~UserGenerator( void );

public:

    const bool create( kvs::ni::Context& context );

public:

    xn::UserGenerator& generator( void );

    kvs::ni::Skeleton& skeleton( void );

    kvs::ni::PoseDetection& pose( void );

    const unsigned short nusers( void );

    const kvs::ValueArray<unsigned int> users( void );

    const kvs::Vector3f centerOfMass( const unsigned int user );

    const unsigned short* pointer( const unsigned int user );

public:

    void addNewUserEvent( kvs::ni::NewUserEventListener* event );

    void addLostUserEvent( kvs::ni::LostUserEventListener* event );

public:

    void newUser(
        xn::UserGenerator& generator,
        XnUserID user,
        void* cookie );

    void lostUser(
        xn::UserGenerator& generator,
        XnUserID user,
        void* cookie );

};

}

}

#endif
