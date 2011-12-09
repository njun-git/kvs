/****************************************************************************/
/**
 *  @file PolygonToPolygon.h
 */
/*----------------------------------------------------------------------------
 *
 *  Copyright (c) Visualization Laboratory, Kyoto University.
 *  All rights reserved.
 *  See http://www.viz.media.kyoto-u.ac.jp/kvs/copyright/ for details.
 *
 *  $Id$
 */
/****************************************************************************/

#ifndef KVS__POLYGON_TO_POLYGON_H_INCLUDE
#define KVS__POLYGON_TO_POLYGON_H_INCLUDE

#include <kvs/PolygonObject>
#include <kvs/ClassName>
#include <kvs/Module>
#include <kvs/FilterBase>

namespace kvs
{

/*==========================================================================*/
/**
 *  Polygon to polygon class.
 */
/*==========================================================================*/
class PolygonToPolygon : public kvs::FilterBase, public kvs::PolygonObject
{

    // Class name.
    kvsClassName( kvs::PolygonToPolygon );

    // Module information.
    kvsModuleCategory( Filter );
    kvsModuleBaseClass( kvs::FilterBase );
    kvsModuleSuperClass( kvs::PolygonObject );

public:

    PolygonToPolygon( void );

    PolygonToPolygon( const kvs::PolygonObject* object );

    virtual ~PolygonToPolygon( void );

public:

    SuperClass* exec( const kvs::ObjectBase* object );

private:

    void calculate_triangle_connections( const kvs::PolygonObject* object );

    void calculate_triangle_normals( void );

};

} // end of namespace kvs

#endif // KVS__POLYGON_TO_POLYGON_H_INCLUDE

