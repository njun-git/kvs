//
//  NullObject.cpp
//  
//
//  Created by Jun Nishimura on 11/4/10.
//  Copyright 2011 Jun Nishimura. All rights reserved.
//

#include "NullObject.h"

namespace kvs
{

NullObject::NullObject( void ) :
    kvs::ObjectBase()
{
}

NullObject::NullObject( const kvs::ObjectBase* object ) :
    kvs::ObjectBase()
{
    const kvs::Vector3f min_coord = object->minObjectCoord();
    const kvs::Vector3f max_coord = object->maxObjectCoord();

    BaseClass::setMinMaxObjectCoords( min_coord, max_coord );
    BaseClass::setMinMaxExternalCoords( min_coord, max_coord );
}

NullObject::NullObject( const kvs::Vector3f& min_coord, const kvs::Vector3f& max_coord ) :
    kvs::ObjectBase()
{
    BaseClass::setMinMaxObjectCoords( min_coord, max_coord );
    BaseClass::setMinMaxExternalCoords( min_coord, max_coord );
}

NullObject::~NullObject( void )
{
}

kvs::NullObject* NullObject::DownCast( kvs::ObjectBase* object )
{
    const kvs::ObjectBase::ObjectType type = object->objectType();
    if ( type != kvs::ObjectBase::Geometry )
    {
        kvsMessageError( "Input object is not a geometry object." );
        return( NULL );
    }

    kvs::NullObject* null = static_cast<kvs::NullObject*>( object );

    return( null );
}

const kvs::NullObject* NullObject::DownCast( const kvs::ObjectBase* object )
{
    return( NullObject::DownCast( const_cast<kvs::ObjectBase*>( object ) ) );
}

void NullObject::shallowCopy( const NullObject& object )
{
    BaseClass::operator=( object );
}

void NullObject::deepCopy( const NullObject& object )
{
    BaseClass::operator=( object );
}

const kvs::ObjectBase::ObjectType NullObject::objectType( void ) const
{
    return( kvs::ObjectBase::Geometry );
}

void NullObject::update( const kvs::ObjectBase* object )
{
    const kvs::Vector3f o_min_coord = object->minObjectCoord();
    const kvs::Vector3f o_max_coord = object->maxObjectCoord();

    if ( !BaseClass::hasMinMaxObjectCoords() )
    {
        BaseClass::setMinMaxObjectCoords( o_min_coord, o_max_coord );
        BaseClass::setMinMaxExternalCoords( o_min_coord, o_max_coord );
    }

    kvs::Vector3f n_min_coord = this->minObjectCoord();
    kvs::Vector3f n_max_coord = this->maxObjectCoord();

    if ( o_min_coord.x() < n_min_coord.x() ) n_min_coord.x() = o_min_coord.x();
    if ( o_min_coord.y() < n_min_coord.y() ) n_min_coord.y() = o_min_coord.y();
    if ( o_min_coord.z() < n_min_coord.z() ) n_min_coord.z() = o_min_coord.z();

    if ( o_max_coord.x() > n_max_coord.x() ) n_max_coord.x() = o_max_coord.x();
    if ( o_max_coord.y() > n_max_coord.y() ) n_max_coord.y() = o_max_coord.y();
    if ( o_max_coord.z() > n_max_coord.z() ) n_max_coord.z() = o_max_coord.z();

    BaseClass::setMinMaxObjectCoords( n_min_coord, n_max_coord );
    BaseClass::setMinMaxExternalCoords( n_min_coord, n_max_coord );
}

}
