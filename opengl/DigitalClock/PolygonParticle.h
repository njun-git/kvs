//
//  PolygonParticle.h
//  
//
//  Created by Jun Nishimura on 11/25/8.
//  Copyright 2011 Jun Nishimura. All rights reserved.
//

#ifndef KVS__POLYGON_PARTICLE_H_INCLUDE
#define KVS__POLYGON_PARTICLE_H_INCLUDE

#include <kvs/ClassName>
#include <kvs/ValueArray>
#include <kvs/Vector3>
#include <kvs/Math>

namespace kvs
{

class PolygonParticle
{
    // Class name.
    kvsClassName( kvs::PolygonParticle );

protected:

    float                   m_scale;
    float                   m_radius;
    kvs::Vector3f           m_center;
    kvs::ValueArray<float>  m_coords;

public:

    PolygonParticle( void );

    virtual ~PolygonParticle( void );

public:

    void initialize( const kvs::Vector3f& center, const float radius );

public:

    void setScale( const float scale );

public:

    const float scale( void ) const;

    const kvs::ValueArray<float>& coords( void ) const;

private:

    void calculate_coords( void );

};

}

#endif
