//
//  SkeletonPolygonObject.cpp
//  
//
//  Created by Jun Nishimura on 10/24/11.
//  Copyright (c) 2011 Jun Nishimura. All rights reserved.
//

#include "SkeletonPolygonObject.h"

#include <kvs/Math>
#include <kvs/Quaternion>
#include <kvs/ColorMap>

namespace kvs
{

namespace ni
{

namespace octahedron
{

const float coords[6][3] =
{
    { 0.0f, 0.0f,   1.0f },
    {   0.707106781f,   0.707106781f, 0.0f },
    { - 0.707106781f,   0.707106781f, 0.0f },
    { - 0.707106781f, - 0.707106781f, 0.0f },
    {   0.707106781f, - 0.707106781f, 0.0f },
    { 0.0f, 0.0f, - 1.0f },
};

const unsigned int connections[8][3] =
{
    { 0, 1, 2 },
    { 0, 2, 3 },
    { 0, 3, 4 },
    { 0, 4, 1 },
    { 5, 2, 1 },
    { 5, 3, 2 },
    { 5, 4, 3 },
    { 5, 1, 4 },
};

const float normals[8][3] =
{
    { 0.0f, 0.707106781f, 0.707106781f },
    { - 0.707106781f, 0.0f, 0.707106781f },
    { 0.0f, - 0.707106781f, 0.707106781f },
    { 0.707106781f, 0.0f, 0.707106781f },
    { 0.0f, 0.707106781f, - 0.707106781f },
    { - 0.707106781f, 0.0f, - 0.707106781f },
    { 0.0f, - 0.707106781f, - 0.707106781f },
    { 0.707106781f, 0.0f, - 0.707106781f },
};

}

SkeletonPolygonObject::SkeletonPolygonObject( void ) :
    kvs::PolygonObject()
{
    this->initialize();
}

SkeletonPolygonObject::~SkeletonPolygonObject( void )
{
}

void SkeletonPolygonObject::initialize( void )
{
    m_width = XN_VGA_X_RES;
    m_height = XN_VGA_Y_RES;
    m_cx = 0.5f * XN_VGA_X_RES;
    m_cy = 0.5f * XN_VGA_Y_RES;
    m_p2r = 0.0f;

    const kvs::Vector3f min_coord(
        - 0.001f * m_width / 2.0f, - 0.001f * m_height / 2.0f, - 4.0f );
    const kvs::Vector3f max_coord(
          0.001f * m_width / 2.0f,   0.001f * m_height / 2.0f, - 2.0f );

    BaseClass::setMinMaxObjectCoords( min_coord, max_coord );
    BaseClass::setMinMaxExternalCoords( min_coord, max_coord );

    BaseClass::setPolygonType( kvs::PolygonObject::Triangle );
    BaseClass::setColorType( kvs::PolygonObject::PolygonColor );
    BaseClass::setNormalType( kvs::PolygonObject::PolygonNormal );

    m_joint_list.allocate( 15 );
    m_joint_list[  0 ] = kvs::ni::Skeleton::Head;
    m_joint_list[  1 ] = kvs::ni::Skeleton::Neck;
    m_joint_list[  2 ] = kvs::ni::Skeleton::Torso;
    m_joint_list[  3 ] = kvs::ni::Skeleton::LeftShoulder;
    m_joint_list[  4 ] = kvs::ni::Skeleton::LeftElbow;
    m_joint_list[  5 ] = kvs::ni::Skeleton::LeftHand;
    m_joint_list[  6 ] = kvs::ni::Skeleton::RightShoulder;
    m_joint_list[  7 ] = kvs::ni::Skeleton::RightElbow;
    m_joint_list[  8 ] = kvs::ni::Skeleton::RightHand;
    m_joint_list[  9 ] = kvs::ni::Skeleton::LeftHip;
    m_joint_list[ 10 ] = kvs::ni::Skeleton::LeftKnee;
    m_joint_list[ 11 ] = kvs::ni::Skeleton::LeftFoot;
    m_joint_list[ 12 ] = kvs::ni::Skeleton::RightHip;
    m_joint_list[ 13 ] = kvs::ni::Skeleton::RightKnee;
    m_joint_list[ 14 ] = kvs::ni::Skeleton::RightFoot;

    m_joint_connect.allocate( 16 * 2 );
    m_joint_connect[  0 ] = kvs::ni::Skeleton::Head;            m_joint_connect[  1 ] = kvs::ni::Skeleton::Neck;
    m_joint_connect[  2 ] = kvs::ni::Skeleton::Neck;            m_joint_connect[  3 ] = kvs::ni::Skeleton::LeftShoulder;
    m_joint_connect[  4 ] = kvs::ni::Skeleton::Neck;            m_joint_connect[  5 ] = kvs::ni::Skeleton::RightShoulder;
    m_joint_connect[  6 ] = kvs::ni::Skeleton::LeftShoulder;    m_joint_connect[  7 ] = kvs::ni::Skeleton::LeftElbow;
    m_joint_connect[  8 ] = kvs::ni::Skeleton::LeftElbow;       m_joint_connect[  9 ] = kvs::ni::Skeleton::LeftHand;
    m_joint_connect[ 10 ] = kvs::ni::Skeleton::RightShoulder;   m_joint_connect[ 11 ] = kvs::ni::Skeleton::RightElbow;
    m_joint_connect[ 12 ] = kvs::ni::Skeleton::RightElbow;      m_joint_connect[ 13 ] = kvs::ni::Skeleton::RightHand;
    m_joint_connect[ 14 ] = kvs::ni::Skeleton::LeftShoulder;    m_joint_connect[ 15 ] = kvs::ni::Skeleton::Torso;
    m_joint_connect[ 16 ] = kvs::ni::Skeleton::RightShoulder;   m_joint_connect[ 17 ] = kvs::ni::Skeleton::Torso;
    m_joint_connect[ 18 ] = kvs::ni::Skeleton::Torso;           m_joint_connect[ 19 ] = kvs::ni::Skeleton::LeftHip;
    m_joint_connect[ 20 ] = kvs::ni::Skeleton::LeftHip;         m_joint_connect[ 21 ] = kvs::ni::Skeleton::LeftKnee;
    m_joint_connect[ 22 ] = kvs::ni::Skeleton::LeftKnee;        m_joint_connect[ 23 ] = kvs::ni::Skeleton::LeftFoot;
    m_joint_connect[ 24 ] = kvs::ni::Skeleton::Torso;           m_joint_connect[ 25 ] = kvs::ni::Skeleton::RightHip;
    m_joint_connect[ 26 ] = kvs::ni::Skeleton::RightHip;        m_joint_connect[ 27 ] = kvs::ni::Skeleton::RightKnee;
    m_joint_connect[ 28 ] = kvs::ni::Skeleton::RightKnee;       m_joint_connect[ 29 ] = kvs::ni::Skeleton::RightFoot;
    m_joint_connect[ 30 ] = kvs::ni::Skeleton::LeftHip;         m_joint_connect[ 31 ] = kvs::ni::Skeleton::RightHip;
}

void SkeletonPolygonObject::update(
    kvs::ni::DepthGenerator& depth,
    kvs::ni::UserGenerator& user )
{
    if ( kvs::Math::IsZero( m_p2r ) )
    {
        const unsigned long zpd = depth.zeroPlaneDistance();
        const float zpps = depth.zeroPlanePixelSize();
        m_p2r = zpps * ( 1280.0f / m_width ) / static_cast<float>( zpd );
    }

    m_tmp_coords.clear();
    m_tmp_colors.clear();
    m_tmp_normals.clear();
    m_tmp_connections.clear();

    kvs::ValueArray<unsigned int> users = user.users();
    kvs::ColorMap color_map( 5 );
    color_map.create();
    for ( size_t i = 0; i < users.size(); i++ )
    {
        if ( user.skeleton().isTracking( users[i] ) )
        {
            const size_t color_index = users[i] % 5;
            const kvs::RGBColor color = color_map[ color_index ];
            for ( size_t j = 0; j < m_joint_list.size(); j++ )
            {
                const float size = ( m_joint_list[j] == kvs::ni::Skeleton::Head ) ? 0.1f : 0.0375f;
                this->add_octahedron_of_joint(
                    users[i], m_joint_list[j], depth, user, color, size );
            }
            for ( size_t j = 0; j < m_joint_connect.size() / 2; j++ )
            {
                this->add_octahedron_of_joint_connect(
                    users[i], m_joint_connect[ j * 2 ], m_joint_connect[ j * 2 + 1 ],
                    depth, user, color, 0.0375f );
            }
        }
    }

    BaseClass::setCoords( kvs::ValueArray<float>( m_tmp_coords ) );
    BaseClass::setColors( kvs::ValueArray<unsigned char>( m_tmp_colors ) );
    BaseClass::setNormals( kvs::ValueArray<float>( m_tmp_normals ) );
    BaseClass::setConnections( kvs::ValueArray<unsigned int>( m_tmp_connections ) );

}

const kvs::Vector3f SkeletonPolygonObject::vertex_of_joint(
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

void SkeletonPolygonObject::add_octahedron_of_joint(
    const unsigned int index,
    const kvs::ni::Skeleton::SkeletonJoint& joint,
    kvs::ni::DepthGenerator& depth,
    kvs::ni::UserGenerator& user,
    const kvs::RGBColor& color,
    const float size )
{
    const size_t start_index = m_tmp_coords.size() / 3;
    const kvs::Vector3f joint_position = this->vertex_of_joint( index, joint, depth, user );
    for ( size_t i = 0; i < 6; i++ )
    {
        m_tmp_coords.push_back( size * kvs::ni::octahedron::coords[i][0] + joint_position.x() );
        m_tmp_coords.push_back( size * kvs::ni::octahedron::coords[i][1] + joint_position.y() );
        m_tmp_coords.push_back( size * kvs::ni::octahedron::coords[i][2] + joint_position.z() );
    }

    for ( size_t i = 0; i < 8; i++ )
    {
        for ( size_t j = 0; j < 3; j++ )
        {
            m_tmp_connections.push_back( start_index + kvs::ni::octahedron::connections[i][j] );
            m_tmp_normals.push_back( kvs::ni::octahedron::normals[i][j] );
        }
        m_tmp_colors.push_back( color.r() );
        m_tmp_colors.push_back( color.g() );
        m_tmp_colors.push_back( color.b() );
    }
}

void SkeletonPolygonObject::add_octahedron_of_joint_connect(
    const unsigned int index,
    const kvs::ni::Skeleton::SkeletonJoint& joint1,
    const kvs::ni::Skeleton::SkeletonJoint& joint2,
    kvs::ni::DepthGenerator& depth,
    kvs::ni::UserGenerator& user,
    const kvs::RGBColor& color,
    const float size )
{
    const size_t start_index = m_tmp_coords.size() / 3;

    const kvs::Vector3f p1 = this->vertex_of_joint( index, joint1, depth, user );
    const kvs::Vector3f p2 = this->vertex_of_joint( index, joint2, depth, user );
    const kvs::Vector3f p12 = p2 - p1;
    const float half_length = 0.5f * p12.length();
    const kvs::Quaternion<float> q = kvs::Quaternion<float>::rotationQuaternion( kvs::Vector3f( 0.0f, 0.0f, 1.0f ), p12 );
    const kvs::Matrix33f rotation = q.toMatrix();

    for ( size_t i = 0; i < 6; i++ )
    {
        const kvs::Vector3f oct_vertex( kvs::ni::octahedron::coords[i] );
        kvs::Vector3f vertex;
        vertex.x() = size * oct_vertex.x();
        vertex.y() = size * oct_vertex.y();
        vertex.z() = half_length * oct_vertex.z();

        vertex = rotation * vertex;

        m_tmp_coords.push_back( p1.x() + vertex.x() + 0.5f * p12.x() );
        m_tmp_coords.push_back( p1.y() + vertex.y() + 0.5f * p12.y() );
        m_tmp_coords.push_back( p1.z() + vertex.z() + 0.5f * p12.z() );
    }

    for ( size_t i = 0; i < 8; i++ )
    {
        const kvs::Vector3f oct_normal( kvs::ni::octahedron::normals[i] );
        const kvs::Vector3f normal = rotation * oct_normal;
        for ( size_t j = 0; j < 3; j++ )
        {
            m_tmp_connections.push_back( start_index + kvs::ni::octahedron::connections[i][j] );
            m_tmp_normals.push_back( normal[j] );
        }
        m_tmp_colors.push_back( color.r() );
        m_tmp_colors.push_back( color.g() );
        m_tmp_colors.push_back( color.b() );
    }
}

}

}
