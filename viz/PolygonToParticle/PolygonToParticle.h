//
//  PolygonToParticle.h
//  
//
//  Created by njun on 11/27/7.
//  Copyright 2011 Jun Nishimura. All rights reserved.
//

#ifndef KVS__POLYGON_TO_PARTICLE_H_INCLUDE
#define KVS__POLYGON_TO_PARTICLE_H_INCLUDE

#include <kvs/ClassName>
#include <kvs/Module>
#include <kvs/PolygonObject>
#include <kvs/PointObject>
#include <kvs/CellByCellParticleGenerator>

namespace kvs
{

class PolygonToParticle : public kvs::PointObject
{
    // Class name.
    kvsClassName( PolygonToParticle );

    // Module information.
    kvsModuleBaseClass( kvs::PointObject );

public:

    PolygonToParticle( void );

    PolygonToParticle( const kvs::PolygonObject* polygon, const size_t nparticles );

    virtual ~PolygonToParticle( void );

public:

    void mapping( const kvs::PolygonObject* polygon, const size_t nparticles );

};

}

#endif
