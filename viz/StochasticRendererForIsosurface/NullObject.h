//
//  NullObject.h
//  
//
//  Created by Jun Nishimura on 11/4/10.
//  Copyright 2011 Jun Nishimura. All rights reserved.
//

#ifndef KVS__NULL_OBJECT_H_INCLUDE
#define KVS__NULL_OBJECT_H_INCLUDE

#include <kvs/ObjectBase>
#include <kvs/ClassName>
#include <kvs/Module>

namespace kvs
{

class NullObject : public kvs::ObjectBase
{

    // Class name.
    kvsClassName( kvs::NullObject );

    // Module information.
    kvsModuleCategory( Object );
    kvsModuleBaseClass( kvs::ObjectBase );

protected:

    bool    m_has_min_max_coords;

public:

    NullObject( void );

    NullObject( const kvs::ObjectBase* object );

    NullObject( const kvs::Vector3f& min_coord, const kvs::Vector3f& max_coord );

    virtual ~NullObject( void );

public:

    static kvs::NullObject* DownCast( kvs::ObjectBase* object );

    static const kvs::NullObject* DownCast( const kvs::ObjectBase* object );

public:

    void shallowCopy( const NullObject& object );

    void deepCopy( const NullObject& object );

public:

    const ObjectType objectType( void ) const;

    void update( const kvs::ObjectBase* object );

};

}

#endif
