//
//  ScreenController.h
//  
//
//  Created by Jun Nishimura on 10/25/11.
//  Copyright (c) 2011 Jun Nishimura. All rights reserved.
//

#ifndef KVS__TDW__SCREEN_CONTROLLER_H_INCLUDE
#define KVS__TDW__SCREEN_CONTROLLER_H_INCLUDE

#include <kvs/ClassName>
#include <kvs/Module>
#include <kvs/Assert>
#include <kvs/ScreenBase>
#include <list>

#include <kvs/tdw/Screen>

#include "DepthGenerator.h"
#include "UserGenerator.h"

namespace kvs
{

namespace tdw
{

class HandPosition
{

    // Class name.
    kvsClassName( kvs::tdw::HandPosition );

protected:

    unsigned int    m_user_id;
    bool            m_is_active[2];
    kvs::Vector3f   m_position[2];

public:

    HandPosition( const unsigned int user_id )
    {
        m_user_id = user_id;
        for ( size_t i = 0; i < 2; i++ )
        {
            m_is_active[i] = false;
            m_position[i].set( 0.0f, 0.0f, 0.0f );
        }
    }

    virtual ~HandPosition( void ) {}

public:

    const unsigned int user( void ) const { return( m_user_id ); }

    const bool isActive( const size_t index ) const
    {
        KVS_ASSERT( index < 2 );
        return( m_is_active[ index ] );
    }

    const kvs::Vector3f& position( const size_t index ) const
    {
        KVS_ASSERT( index < 2 );
        return( m_position[ index ] );
    }

public:

    void setIsActive( const size_t index, const bool is_active )
    {
        KVS_ASSERT( index < 2 );
        m_is_active[ index ] = is_active;
    }

    void setPosition( const size_t index, const kvs::Vector3f& position )
    {
        KVS_ASSERT( index < 2 );
        m_position[ index ] = position;
    }

};

class ScreenController
{

    // Class name.
    kvsClassName( kvs::tdw::ScreenController );

    typedef std::list<kvs::tdw::HandPosition> HandList;

protected:

    size_t  m_width;
    size_t  m_height;

    float   m_cx;
    float   m_cy;
    float   m_p2r;

    kvs::ScreenBase*                m_ref_screen;
    kvs::tdw::MasterMessageSender*  m_ref_sender;
    HandList                        m_list;

public:

    ScreenController( void );

    ScreenController( kvs::ScreenBase* screen, kvs::tdw::MasterMessageSender* sender );

    virtual ~ScreenController( void );

public:

    void initialize( void );

    void update(
        kvs::ni::DepthGenerator& depth,
        kvs::ni::UserGenerator& user );

public:

    void attachScreen( kvs::ScreenBase* screen );

protected:

    const kvs::Vector3f vertex_of_joint(
        const unsigned int index,
        const kvs::ni::Skeleton::SkeletonJoint& joint,
        kvs::ni::DepthGenerator& depth,
        kvs::ni::UserGenerator& user );

    void rotate(
        const kvs::Vector3f& pre_hand,
        const kvs::Vector3f& cur_hand,
        const kvs::Vector3f& center );

    void translate(
        const kvs::Vector3f& pre_hand,
        const kvs::Vector3f& cur_hand );

    void scale(
        const kvs::Vector3f& pre_left_hand,
        const kvs::Vector3f& pre_right_hand,
        const kvs::Vector3f& cur_left_hand,
        const kvs::Vector3f& cur_right_hand );

    const bool is_cross_pose(
        const unsigned int index,
        kvs::ni::DepthGenerator& depth,
        kvs::ni::UserGenerator& user );

};

}

}

#endif
