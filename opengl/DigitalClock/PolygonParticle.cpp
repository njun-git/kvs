//
//  PolygonParticle.cpp
//  
//
//  Created by Jun Nishimura on 11/25/8.
//  Copyright 2011 Jun Nishimura. All rights reserved.
//

#include "PolygonParticle.h"

#define _NDIVISION 16

namespace kvs
{

PolygonParticle::PolygonParticle( void ) :
    m_scale( 1.0f )
{
}

PolygonParticle::~PolygonParticle( void )
{
}

void PolygonParticle::initialize( const kvs::Vector3f& center, const float radius )
{
    m_center = center;
    m_radius = radius;
    this->setScale( 1.0f );
}

void PolygonParticle::setScale( const float scale )
{
    if ( scale < 1.0f ) m_scale = 1.0f;
    else                m_scale = scale;
    this->calculate_coords();
}

const float PolygonParticle::scale( void ) const
{
    return( m_scale );
}

const kvs::ValueArray<float>& PolygonParticle::coords( void ) const
{
    return( m_coords );
}

void PolygonParticle::calculate_coords( void )
{
    const size_t nnodes = 1 + _NDIVISION + 1;
    if ( m_coords.size() != 3 * nnodes )
    {
        m_coords.allocate( 3 * nnodes );
    }

    for ( size_t i = 0; i < nnodes; i++ )
    {
        m_coords[ i * 3     ] = m_center.x();
        m_coords[ i * 3 + 1 ] = m_center.y();
        m_coords[ i * 3 + 2 ] = m_center.z();
    }

    const float step = 360.0f / _NDIVISION;
    const float radius = m_radius * m_scale;
    for ( size_t i = 1; i < nnodes; i++ )
    {
        m_coords[ i * 3     ] += radius * cosf( kvs::Math::Deg2Rad( step * ( i - 1 ) ) );
        m_coords[ i * 3 + 1 ] += radius * sinf( kvs::Math::Deg2Rad( step * ( i - 1 ) ) );
    }
}

}
