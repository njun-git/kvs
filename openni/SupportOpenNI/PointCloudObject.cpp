//
//  PointCloudObject.cpp
//  
//
//  Created by Jun Nishimura on 10/19/11.
//  Copyright (c) 2011 Jun Nishimura. All rights reserved.
//

#include "PointCloudObject.h"

namespace kvs
{

namespace ni
{

PointCloudObject::PointCloudObject( void ) :
    kvs::PointObject()
{
    this->initialize();
}

PointCloudObject::~PointCloudObject( void )
{
}

void PointCloudObject::initialize( void )
{
    m_width = XN_VGA_X_RES;
    m_height = XN_VGA_Y_RES;
    m_coords.allocate( 3 * m_width * m_height );
    m_colors.allocate( 3 * m_width * m_height );
    m_colors.fill( 0xff );
    const kvs::Vector3f min_coord(
        - 0.001f * m_width / 2.0f, - 0.001f * m_height / 2.0f, - 4.0f );
    const kvs::Vector3f max_coord(
          0.001f * m_width / 2.0f,   0.001f * m_height / 2.0f, - 2.0f );

    BaseClass::setMinMaxObjectCoords( min_coord, max_coord );
    BaseClass::setMinMaxExternalCoords( min_coord, max_coord );
}

void PointCloudObject::update(
    kvs::ni::DepthGenerator& depth )
{
    this->calculate_coords( depth );
}

void PointCloudObject::update(
    kvs::ni::ImageGenerator& image,
    kvs::ni::DepthGenerator& depth )
{
    depth.adjustDepthScale( image );

    this->calculate_coords( depth );
    this->calculate_colors( image );
}

void PointCloudObject::calculate_coords( kvs::ni::DepthGenerator& depth )
{
    const unsigned long zpd = depth.zeroPlaneDistance();
    const float zpps = depth.zeroPlanePixelSize();
    const float cx = static_cast<float>( m_width ) * 0.5f;
    const float cy = static_cast<float>( m_height ) * 0.5f;
    const float P2R = zpps * ( 1280.0f / m_width ) / static_cast<float>( zpd );

    const unsigned short* p_depth = depth.pointer();
    for ( size_t y = 0; y < m_height; y++ )
    {
        for ( size_t x = 0; x < m_width; x++ )
        {
            const size_t index = x + y * m_width;
            float d = 0.001f * static_cast<float>( p_depth[ index ] );
            m_coords[ index * 3     ] = d * P2R * ( float(x) - cx );
            m_coords[ index * 3 + 1 ] = d * P2R * ( float( m_height - y ) - cy );
            m_coords[ index * 3 + 2 ] = - d;
        }
    }
}

void PointCloudObject::calculate_colors( kvs::ni::ImageGenerator& image )
{
    const unsigned char* p_color = image.pointer();
    for ( size_t y = 0; y < m_height; y++ )
    {
        for ( size_t x = 0; x < m_width; x++ )
        {
            const size_t index = x + y * m_width;
            m_colors[ index * 3     ] = p_color[ index * 3     ];
            m_colors[ index * 3 + 1 ] = p_color[ index * 3 + 1 ];
            m_colors[ index * 3 + 2 ] = p_color[ index * 3 + 2 ];
        }
    }
}

}

}
