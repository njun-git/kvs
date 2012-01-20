//
//  SandObject.h
//  
//
//  Created by Jun Nishimura on 1/14/12.
//  Copyright (c) 2012 Jun Nishimura. All rights reserved.
//

#ifndef KVS__OPENNI__SAND_OBJECT_H_INCLUDE
#define KVS__OPENNI__SAND_OBJECT_H_INCLUDE

#include <kvs/ClassName>
#include <kvs/Module>
#include <kvs/PointObject>

#include <kvs/ni/ImageGenerator>
#include <kvs/ni/DepthGenerator>
#include <kvs/ni/UserGenerator>

namespace kvs
{

namespace ni
{

class SandObject : public kvs::PointObject
{

    // Class name.
    kvsClassName( kvs::ni::SandObject );

    // Module information.
    kvsModuleCategory( Object );
    kvsModuleBaseClass( kvs::PointObject );

protected:

    size_t  m_width;
    size_t  m_height;

    size_t          m_max_nparticles;
    kvs::Vector3f   m_gravity;

public:

    SandObject( void );

    virtual ~SandObject( void );

public:

    void setMaxNParticles( const size_t nparticles );

    void setGravity( const kvs::Vector3f& gravity );

public:

    void initialize( void );

    void update(
        kvs::ni::ImageGenerator& image,
        kvs::ni::DepthGenerator& depth,
        kvs::ni::UserGenerator& user );

};

}

}

#endif
