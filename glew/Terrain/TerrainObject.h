/*
 *  TerrainObject.h
 *  
 *
 *  Created by njun on 11/10/11.
 *  Copyright 2011 Jun Nishimura. All rights reserved.
 *
 */

#ifndef KVS__TERRAIN_OBJECT_H_INCLUDE
#define KVS__TERRAIN_OBJECT_H_INCLUDE

#include <kvs/ObjectBase>
#include <kvs/ClassName>
#include <kvs/Module>
#include <kvs/ValueArray>
#include <kvs/Vector2>

namespace kvs
{

class TerrainObject : public kvs::ObjectBase
{

    // Class name.
    kvsClassName( kvs::TerrainObject );

    // Module information.
    kvsModuleCategory( Object );
    kvsModuleBaseClass( kvs::ObjectBase );

protected:

    size_t      m_width;
    size_t      m_height;

    kvs::ValueArray<float>          m_coords;
    kvs::ValueArray<unsigned int>   m_connections;

public:

    TerrainObject( void );

    TerrainObject( const size_t width, const size_t height );

    virtual ~TerrainObject( void );

public:

    const ObjectType objectType( void ) const;

    const size_t nvertices( void ) const;

    const size_t npolygons( void ) const;

    const kvs::ValueArray<float>& coords( void ) const;

    const kvs::ValueArray<unsigned int>& connections( void ) const;

public:

    void create( const size_t width, const size_t height );

};

}

#endif
