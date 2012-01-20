//
//  SandObject.cpp
//  
//
//  Created by Jun Nishimura on 1/14/12.
//  Copyright (c) 2012 Jun Nishimura. All rights reserved.
//

#include "SandObject.h"

#include <vector>
#include <kvs/MersenneTwister>

namespace kvs
{

namespace ni
{

SandObject::SandObject( void ) :
    kvs::PointObject()
{
    this->initialize();
}

SandObject::~SandObject( void )
{
}

void SandObject::setMaxNParticles( const size_t nparticles )
{
    m_max_nparticles = nparticles;
}

void SandObject::setGravity( const kvs::Vector3f& gravity )
{
    m_gravity = gravity;
}

void SandObject::initialize( void )
{
    m_width = XN_VGA_X_RES;
    m_height = XN_VGA_Y_RES;
    m_max_nparticles = 500000;
    m_gravity.set( 0.0f, - 9.8f, 0.0f );
    m_gravity.set( 0.0f, 0.0f, - 9.8f );

    const kvs::Vector3f min_coord(
        - 0.001f * m_width / 2.0f, - 0.001f * m_height / 2.0f, - 4.0f );
    const kvs::Vector3f max_coord(
          0.001f * m_width / 2.0f,   0.001f * m_height / 2.0f, - 2.0f );

    BaseClass::setMinMaxObjectCoords( min_coord, max_coord );
    BaseClass::setMinMaxExternalCoords( min_coord, max_coord );

    BaseClass::setColor( kvs::RGBColor( 255, 255, 255 ) );
}

void SandObject::update(
    kvs::ni::ImageGenerator& image,
    kvs::ni::DepthGenerator& depth,
    kvs::ni::UserGenerator& user )
{
    depth.adjustDepthScale( image );

    const unsigned long zpd = depth.zeroPlaneDistance();
    const float zpps = depth.zeroPlanePixelSize();
    const float cx = static_cast<float>( m_width ) * 0.5f;
    const float cy = static_cast<float>( m_height ) * 0.5f;
    const float P2R = zpps * ( 1280.0f / m_width ) / static_cast<float>( zpd );

    std::vector<float> coords;
    std::vector<unsigned char> colors;

    const unsigned char* p_color = image.pointer();
    const unsigned short* p_depth = depth.pointer();
    kvs::ValueArray<unsigned int> users = user.users();
    kvs::MersenneTwister twister;
    for ( size_t i = 0; i < users.size(); i++ )
    {
        const unsigned short* p_user = user.pointer( users[i] );
        kvs::Vector3f com = user.centerOfMass( users[i] );
        const unsigned short max_depth = static_cast<unsigned short>( 2 * com.z() );
        for ( size_t y = 0; y < m_height; y += 2 )
        {
            for ( size_t x = 0; x < m_width; x += 2 )
            {
                const size_t index = x + y * m_width;
                if ( p_user[ index ] == users[i] )
                {
                    float d = 0.001f * static_cast<float>( p_depth[ index ] );
                    coords.push_back( d * P2R * ( float(x) - cx ) );
                    coords.push_back( d * P2R * ( float( m_height - y ) - cy ) );
                    coords.push_back( - d );

                    const float depth = static_cast<float>( max_depth - p_depth[ index ] ) / static_cast<float>( max_depth );
                    colors.push_back( static_cast<unsigned char>( float( p_color[ index * 3 + 0 ] ) * depth ) );
                    colors.push_back( static_cast<unsigned char>( float( p_color[ index * 3 + 1 ] ) * depth ) );
                    colors.push_back( static_cast<unsigned char>( float( p_color[ index * 3 + 2 ] ) * depth ) );
                }
            }
        }
    }
    const size_t new_np = coords.size() / 3;
    const size_t old_np = m_coords.size() / 3;
    const size_t np = ( old_np + new_np > m_max_nparticles ) ? m_max_nparticles - new_np : old_np;
    const float* p_coords = m_coords.pointer();
    const unsigned char* p_colors = m_colors.pointer();
    for ( size_t i = 0; i < np; i++ )
    {
        kvs::Vector3f old_vec( p_coords + i * 3 );
        kvs::Vector3f new_vec = old_vec + 0.003333f * ( m_gravity + 6.0f * kvs::Vector3f( twister.rand() - 0.5f, twister.rand() - 0.5f, twister.rand() - 0.5f ) );
        coords.push_back( new_vec.x() );
        coords.push_back( new_vec.y() );
        coords.push_back( new_vec.z() );

        kvs::RGBColor old_col( p_colors + i * 3 );
        colors.push_back( static_cast<unsigned char>( old_col.r() ) );
        colors.push_back( static_cast<unsigned char>( old_col.g() ) );
        colors.push_back( static_cast<unsigned char>( old_col.b() ) );
    }

    BaseClass::setCoords( kvs::ValueArray<float>( coords ) );
    BaseClass::setColors( kvs::ValueArray<unsigned char>( colors ) );
}

}

}
