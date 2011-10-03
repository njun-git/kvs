/*
 *  DigitalClockObject.h
 *  
 *
 *  Created by njun on 11/08/23.
 *  Copyright 2011 Jun Nishimura. All rights reserved.
 *
 */

#ifndef KVS__DIGITAL_CLOCK_OBJECT_H_INCLUDE
#define KVS__DIGITAL_CLOCK_OBJECT_H_INCLUDE

#include <kvs/ClassName>
#include <kvs/ObjectBase>
#include <kvs/Time>
#include <kvs/RGBColor>
#include "SevenSegmentLED.h"

namespace kvs
{

class DigitalClockObject : public kvs::ObjectBase
{
    // Class name.
    kvsClassName( kvs::DigitalClockObject );

protected:

    kvs::Time               m_time;
    kvs::SevenSegmentLED    m_leds[4];
    kvs::ColonLED           m_colon;
    kvs::RGBColor           m_on_color;
    kvs::RGBColor           m_off_color;

public:

    DigitalClockObject( void );

    virtual ~DigitalClockObject( void );

public:

    void initialize( void );

    void update( const kvs::Time& time );

public:

    const ObjectType objectType( void ) const;

    const kvs::SevenSegmentLED& led( const size_t index ) const;

    const kvs::ColonLED& colon( void ) const;

    const kvs::RGBColor& onColor( void ) const;

    const kvs::RGBColor& offColor( void ) const;

public:

    void setOnColor( const kvs::RGBColor& color );

    void setOffColor( const kvs::RGBColor& color );

};

}

#endif
