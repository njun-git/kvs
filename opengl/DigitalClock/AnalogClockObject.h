//
//  AnalogClockObject.h
//  
//
//  Created by Jun Nishimura on 11/25/8.
//  Copyright 2011 Jun Nishimura. All rights reserved.
//

#ifndef KVS__ANALOG_CLOCK_OBJECT_H_INCLUDE
#define KVS__ANALOG_CLOCK_OBJECT_H_INCLUDE

#include <kvs/ClassName>
#include <kvs/ObjectBase>
#include <kvs/Time>
#include <kvs/RGBColor>
#include "PolygonParticle.h"

namespace kvs
{

class AnalogClockObject : public kvs::ObjectBase
{
    // Class name.
    kvsClassName( kvs::AnalogClockObject );

protected:

    kvs::Time               m_time;
    kvs::PolygonParticle    m_particles[60];
    kvs::RGBColor           m_color;

public:

    AnalogClockObject( void );

    virtual ~AnalogClockObject( void );

public:

    void initialize( void );

    void update( const kvs::Time& time );

public:

    const ObjectType objectType( void ) const;

    const kvs::PolygonParticle& particle( const size_t index ) const;

    const kvs::RGBColor& color( void ) const;

};

}

#endif
