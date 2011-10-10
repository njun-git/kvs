/*
 *  TerrainObject.cpp
 *  
 *
 *  Created by njun on 11/10/11.
 *  Copyright 2011 Jun Nishimura. All rights reserved.
 *
 */

#include "TerrainObject.h"

namespace kvs
{

TerrainObject::TerrainObject( void ) :
    kvs::ObjectBase()
{
    this->create( 0, 0 );
}

TerrainObject::TerrainObject( const size_t width, const size_t height ) :
    kvs::ObjectBase()
{
    this->create( width, height );
}

TerrainObject::~TerrainObject( void )
{
}

const kvs::ObjectBase::ObjectType TerrainObject::objectType( void ) const
{
    return( kvs::ObjectBase::Geometry );
}

const size_t TerrainObject::nvertices( void ) const
{
    return( m_coords.size() / 3 );
}

const size_t TerrainObject::npolygons( void ) const
{
    return( m_connections.size() / 3 );
}

const kvs::ValueArray<float>& TerrainObject::coords( void ) const
{
    return( m_coords );
}

const kvs::ValueArray<unsigned int>& TerrainObject::connections( void ) const
{
    return( m_connections );
}

void TerrainObject::create( const size_t width, const size_t height )
{
    m_width = width + 1;
    m_height = height + 1;

    const size_t nvertices = m_width * m_height;
    m_coords.allocate( 3 * nvertices );

    const float factor_x = 1.0f / width;
    const float factor_y = 1.0f / height;
    for ( size_t x = 0; x < m_width; x++ )
    {
        for ( size_t y = 0; y < m_height; y++ )
        {
            const size_t index = m_width * y + x;
            m_coords[ index * 3     ] = x * factor_x;
            m_coords[ index * 3 + 1 ] = y * factor_y;
            m_coords[ index * 3 + 2 ] = 0.0f;
        }
    }

    const size_t npolygons = 2 * width * height;
    m_connections.allocate( 3 * npolygons );
    for ( size_t x = 0; x < width; x++ )
    {
        for ( size_t y = 0; y < height; y++ )
        {
            const size_t index = width * y + x;
            m_connections[ index * 6     ] = x     + y * m_width;
            m_connections[ index * 6 + 1 ] = x + 1 + y * m_width;
            m_connections[ index * 6 + 2 ] = x + 1 + ( y + 1 ) * m_width;
            m_connections[ index * 6 + 3 ] = x + 1 + ( y + 1 ) * m_width;
            m_connections[ index * 6 + 4 ] = x     + ( y + 1 ) * m_width;
            m_connections[ index * 6 + 5 ] = x     + y * m_width;
        }
    }

    BaseClass::setMinMaxObjectCoords( kvs::Vector3f( 0.0f, 0.0f, 0.0f ), kvs::Vector3f( 1.0f, 1.0f, 1.0f ) );
    BaseClass::setMinMaxExternalCoords( kvs::Vector3f( 0.0f, 0.0f, 0.0f ), kvs::Vector3f( 1.0f, 1.0f, 1.0f ) );
}

}
