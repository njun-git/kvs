//
//  AnalogClockObject.cpp
//  
//
//  Created by Jun Nishimura on 11/25/8.
//  Copyright 2011 Jun Nishimura. All rights reserved.
//

#include "AnalogClockObject.h"

namespace kvs
{

AnalogClockObject::AnalogClockObject( void ) :
    m_color( 255, 255, 255 )
{
    this->initialize();
}

AnalogClockObject::~AnalogClockObject( void )
{
}

void AnalogClockObject::initialize( void )
{
    const float radius = 8.5f;
    const float p_radius = 0.1f;
    for ( size_t i = 0; i < 60; i++ )
    {
        const float theta = kvs::Math::Deg2Rad( 6.0f * i );
        m_particles[i].initialize( radius * kvs::Vector3f( sinf( theta ), cosf( theta ), 0.0f ), p_radius );
    }
    m_time.now();
}

void AnalogClockObject::update( const kvs::Time& time )
{
    m_time = time;
    int second = m_time.second();
    int n_second = ( second >= 59 ) ? 0 : second + 1;

    for ( int i = 0; i < 60; i++ )
    {
        if ( i == second )
        {
        }
        else if ( i == n_second )
        {
            m_particles[i].setScale( m_particles[i].scale() + 0.36f );
        }
        else
        {
            m_particles[i].setScale( m_particles[i].scale() - 0.09f );
        }
    }
}

const kvs::ObjectBase::ObjectType AnalogClockObject::objectType( void ) const
{
    return( kvs::ObjectBase::Geometry );
}

const kvs::PolygonParticle& AnalogClockObject::particle( const size_t index ) const
{
    if ( index > 59 ) return( m_particles[59] );
    return( m_particles[ index ] );
}

const kvs::RGBColor& AnalogClockObject::color( void ) const
{
    return( m_color );
}

}
