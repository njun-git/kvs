/*
 *  DigitalClockObject.cpp
 *  
 *
 *  Created by njun on 11/08/23.
 *  Copyright 2011 Jun Nishimura. All rights reserved.
 *
 */

#include "DigitalClockObject.h"

namespace kvs
{

DigitalClockObject::DigitalClockObject( void ) :
    m_on_color( 255, 255, 255 ),
    m_off_color( 0, 0, 0 )
{
    this->initialize();
}

DigitalClockObject::~DigitalClockObject( void )
{
}

void DigitalClockObject::initialize( void )
{
    m_leds[0].initialize( kvs::Vector3f( - 5.6f, 0.0f, 0.0f ) );
    m_leds[1].initialize( kvs::Vector3f( - 2.3f, 0.0f, 0.0f ) );
    m_leds[2].initialize( kvs::Vector3f(   2.3f, 0.0f, 0.0f ) );
    m_leds[3].initialize( kvs::Vector3f(   5.6f, 0.0f, 0.0f ) );
    m_colon.initialize( kvs::Vector3f( 0.0f, 0.0f, 0.0f ) );
    m_time.now();

    this->setMinMaxObjectCoords( kvs::Vector3f( - 6.0f, - 2.0f, 0.0f ), kvs::Vector3f( 6.0f, 2.0f, 0.0f ) );
    this->setMinMaxExternalCoords( kvs::Vector3f( - 6.0f, - 2.0f, 0.0f ), kvs::Vector3f( 6.0f, 2.0f, 0.0f ) );
}

void DigitalClockObject::update( const kvs::Time& time )
{
    int second = m_time.second();

    m_time = time;
    int h0 = m_time.hour() / 10;
    int h1 = m_time.hour() % 10;
    int m0 = m_time.minute() / 10;
    int m1 = m_time.minute() % 10;

    m_leds[0].setNumber( h0 );
    m_leds[1].setNumber( h1 );
    m_leds[2].setNumber( m0 );
    m_leds[3].setNumber( m1 );

    if ( second != m_time.second() )
    {
        m_colon.setOn( !m_colon.isOn() );
    }
}

const kvs::ObjectBase::ObjectType DigitalClockObject::objectType( void ) const
{
    return( kvs::ObjectBase::Geometry );
}

const kvs::SevenSegmentLED& DigitalClockObject::led( const size_t index ) const
{
    if ( index >= 4 ) return( m_leds[3] );
    return( m_leds[ index ] );
}

const kvs::ColonLED& DigitalClockObject::colon( void ) const
{
    return( m_colon );
}

const kvs::RGBColor& DigitalClockObject::onColor( void ) const
{
    return( m_on_color );
}

const kvs::RGBColor& DigitalClockObject::offColor( void ) const
{
    return( m_off_color );
}

void DigitalClockObject::setOnColor( const kvs::RGBColor& color )
{
    m_on_color = color;
}

void DigitalClockObject::setOffColor( const kvs::RGBColor& color )
{
    m_off_color = color;
}

}

