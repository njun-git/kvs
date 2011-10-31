//
//  ScreenController.cpp
//  
//
//  Created by Jun Nishimura on 10/25/11.
//  Copyright (c) 2011 Jun Nishimura. All rights reserved.
//

#include "ScreenController.h"

#include <kvs/Math>
#include <kvs/Quaternion>

namespace kvs
{

namespace ni
{

ScreenController::ScreenController( void ) :
    m_ref_screen( NULL )
{
    this->initialize();
}

ScreenController::ScreenController( kvs::ScreenBase* screen ) :
    m_ref_screen( NULL )
{
    this->initialize();
    this->attachScreen( screen );
}

ScreenController::~ScreenController( void )
{
}

void ScreenController::initialize( void )
{
    m_width = XN_VGA_X_RES;
    m_height = XN_VGA_Y_RES;
    m_cx = 0.5f * XN_VGA_X_RES;
    m_cy = 0.5f * XN_VGA_Y_RES;
    m_p2r = 0.0f;
}

void ScreenController::update(
    kvs::ni::DepthGenerator& depth,
    kvs::ni::UserGenerator& user )
{
    if ( !m_ref_screen )
    {
        kvsMessageError( "kvs::ScreenBase* is not attached." );
        return;
    }

    if ( kvs::Math::IsZero( m_p2r ) )
    {
        const unsigned long zpd = depth.zeroPlaneDistance();
        const float zpps = depth.zeroPlanePixelSize();
        m_p2r = zpps * ( 1280.0f / m_width ) / static_cast<float>( zpd );
    }

    kvs::ValueArray<unsigned int> users = user.users();
    const float hand_level = 0.35f;
    for ( size_t i = 0; i < users.size(); i++ )
    {
        HandList::iterator itr = m_list.begin();
        kvs::ni::HandPosition* user_hand = NULL;
        while ( itr != m_list.end() )
        {
            if ( (*itr).user() == users[i] )
            {
                user_hand = &(*itr);
                break;
            }
            itr++;
        }

        if ( !user_hand )
        {
            kvs::ni::HandPosition hand( users[i] );
            m_list.push_back( hand );
            user_hand = &(m_list.back());
        }

        if ( user.skeleton().isTracking( users[i] ) )
        {
            const kvs::Vector3f neck = this->vertex_of_joint(
                users[i], kvs::ni::Skeleton::Neck, depth, user );
            const kvs::Vector3f left_hand = this->vertex_of_joint(
                users[i], kvs::ni::Skeleton::LeftHand, depth, user );
            const kvs::Vector3f right_hand = this->vertex_of_joint(
                users[i], kvs::ni::Skeleton::RightHand, depth, user );

            kvs::Vector3f prev_left_hand;
            kvs::Vector3f prev_right_hand;
            bool is_prev_left_active = false;
            bool is_prev_right_active = false;
            if ( !user_hand->isActive( 0 ) )
            {
                if ( left_hand.z() - neck.z() > hand_level )
                {
                    user_hand->setIsActive( 0, true );
                    user_hand->setPosition( 0, left_hand );
                }
            }
            else
            {
                prev_left_hand = user_hand->position( 0 );
                if ( left_hand.z() - neck.z() > hand_level )
                {
                    is_prev_left_active = true;
                    user_hand->setPosition( 0, left_hand );
                }
                else
                {
                    user_hand->setIsActive( 0, false );
                }
            }

            if ( !user_hand->isActive( 1 ) )
            {
                if ( right_hand.z() - neck.z() > hand_level )
                {
                    user_hand->setIsActive( 1, true );
                    user_hand->setPosition( 1, right_hand );
                }
            }
            else
            {
                prev_right_hand = user_hand->position( 1 );
                if ( right_hand.z() - neck.z() > hand_level )
                {
                    is_prev_right_active = true;
                    user_hand->setPosition( 1, right_hand );
                }
                else
                {
                    user_hand->setIsActive( 1, false );
                }
            }

            if ( user_hand->isActive( 0 ) && !user_hand->isActive( 1 ) &&
                 is_prev_left_active )
            {
                // Translation with left hand.
                this->translate( prev_left_hand, user_hand->position( 0 ) );
            }
            else if ( !user_hand->isActive( 0 ) && user_hand->isActive( 1 ) &&
                      is_prev_right_active )
            {
                // Rotation with right hand.
                this->rotate( prev_right_hand, user_hand->position( 1 ), neck );
            }
            else if ( user_hand->isActive( 0 ) && user_hand->isActive( 1 ) &&
                      is_prev_left_active && is_prev_right_active )
            {
                // Scaling with two hand.
                this->scale( prev_left_hand, prev_right_hand,
                    user_hand->position( 0 ), user_hand->position( 1 ) );
            }
            else if ( !user_hand->isActive( 0 ) && !user_hand->isActive( 1 ) &&
                      itr != m_list.end() )
            {
                // Delete from list.
                m_list.erase( itr );
            }
        }
        if ( this->is_cross_pose( users[i], depth, user ) )
        {
            m_ref_screen->reset();
        }
    }

}

void ScreenController::attachScreen( kvs::ScreenBase* screen )
{
    m_ref_screen = screen;
}

const kvs::Vector3f ScreenController::vertex_of_joint(
    const unsigned int index,
    const kvs::ni::Skeleton::SkeletonJoint& joint,
    kvs::ni::DepthGenerator& depth,
    kvs::ni::UserGenerator& user )
{
    const kvs::ni::JointPosition pos = user.skeleton().jointPosition( index, joint );
    const kvs::Vector3f proj_pos = depth.projectPosition( pos.position() );
    const float d = 0.001f * proj_pos.z();
    return( kvs::Vector3f(
        d * m_p2r * ( proj_pos.x() - m_cx ),
        d * m_p2r * ( float( m_height ) - proj_pos.y() - m_cy ),
        - d ) );
}

void ScreenController::rotate(
    const kvs::Vector3f& pre_hand,
    const kvs::Vector3f& cur_hand,
    const kvs::Vector3f& center )
{
    kvs::Quaternion<float> q; q.identity();
    kvs::Vector3f p1 = center - pre_hand;
    kvs::Vector3f p2 = center - cur_hand;

    if ( ( p1 - p2 ).length() > 0.125f ) return;

    if( m_ref_screen->controlTarget() == kvs::ScreenBase::TargetCamera )
    {
        q = kvs::Quaternion<float>::rotationQuaternion( p1, p2 );
        m_ref_screen->camera()->rotate( q.toMatrix() );
        return;
    }

    // Calculate view.
    const kvs::Vector3f init_vec( 0.0, 0.0, 1.0 );
    const kvs::Vector3f from_vec( m_ref_screen->camera()->position() - m_ref_screen->camera()->lookAt() );
    const kvs::Quaternion<float> rot =
        kvs::Quaternion<float>::rotationQuaternion( init_vec, from_vec );

    p1 = kvs::Quaternion<float>::rotate( p1, rot );
    p2 = kvs::Quaternion<float>::rotate( p2, rot );

    q = kvs::Quaternion<float>::rotationQuaternion( p1, p2 );

    switch( m_ref_screen->controlTarget() )
    {
    case kvs::ScreenBase::TargetLight:
        m_ref_screen->light()->rotate( q.toMatrix() );
        break;
    case kvs::ScreenBase::TargetObject:
        m_ref_screen->objectManager()->rotate( q.toMatrix() );
        break;
    default: break;
    }
}

void ScreenController::translate(
    const kvs::Vector3f& pre_hand,
    const kvs::Vector3f& cur_hand )
{
    if ( ( cur_hand - pre_hand ).length() > 0.125f ) return;

    kvs::Vector3f t = 6.0f * ( cur_hand - pre_hand ); t.z() = 0.0f;

    switch( m_ref_screen->controlTarget() )
    {
    case kvs::ScreenBase::TargetCamera:
        m_ref_screen->camera()->translate( t );
        break;
    case kvs::ScreenBase::TargetLight:
        m_ref_screen->light()->translate( t );
        break;
    case kvs::ScreenBase::TargetObject:
        m_ref_screen->objectManager()->translate( t );
        break;
    default: break;
    }
}

void ScreenController::scale(
    const kvs::Vector3f& pre_left_hand,
    const kvs::Vector3f& pre_right_hand,
    const kvs::Vector3f& cur_left_hand,
    const kvs::Vector3f& cur_right_hand )
{
    kvs::Vector3f pre_diff = pre_left_hand - pre_right_hand; pre_diff.z() = 0.0f;
    const float pre_length = pre_diff.length() + 1.0f;
    kvs::Vector3f cur_diff = cur_left_hand - cur_right_hand; cur_diff.z() = 0.0f;
    const float cur_length = cur_diff.length() + 1.0f;
    const kvs::Vector3f s( cur_length / pre_length );

    if ( ( pre_length - cur_length ) > 0.125f ) return;

    switch( m_ref_screen->controlTarget() )
    {
    case kvs::ScreenBase::TargetCamera:
        m_ref_screen->camera()->scale( s );
        break;
    case kvs::ScreenBase::TargetLight:
        m_ref_screen->light()->scale( s );
        break;
    case kvs::ScreenBase::TargetObject:
        m_ref_screen->objectManager()->scale( s );
        break;
    default: break;
    }
}

const bool ScreenController::is_cross_pose(
    const unsigned int index,
    kvs::ni::DepthGenerator& depth,
    kvs::ni::UserGenerator& user )
{
    const kvs::Vector3f l0 = this->vertex_of_joint( index, kvs::ni::Skeleton::LeftHand, depth, user );
    const kvs::Vector3f l1 = this->vertex_of_joint( index, kvs::ni::Skeleton::LeftElbow, depth, user );
    const kvs::Vector3f r0 = this->vertex_of_joint( index, kvs::ni::Skeleton::RightHand, depth, user );
    const kvs::Vector3f r1 = this->vertex_of_joint( index, kvs::ni::Skeleton::RightElbow, depth, user );

    if ( kvs::Math::Abs( ( ( l1 - l0 ).normalize() ).dot( ( r1 - r0 ).normalize() ) ) < 0.1f )
    {
        if ( ( ( l1 + l0 ) - ( r1 + r0 ) ).length() < 2.0 * 0.1f ) return( true );
    }
    return( false );
}


}

}
