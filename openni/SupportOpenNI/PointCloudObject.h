//
//  PointCloudObject.h
//  
//
//  Created by Jun Nishimura on 10/19/11.
//  Copyright (c) 2011 Jun Nishimura. All rights reserved.
//

#ifndef KVS__OPENNI__POINT_CLOUD_OBJECT_H_INCLUDE
#define KVS__OPENNI__POINT_CLOUD_OBJECT_H_INCLUDE

#include <kvs/ClassName>
#include <kvs/Module>
#include <kvs/PointObject>

#include "ImageGenerator.h"
#include "DepthGenerator.h"

namespace kvs
{

namespace ni
{

class PointCloudObject : public kvs::PointObject
{

    // Class name.
    kvsClassName( kvs::ni::PointCloudObject );

    // Module information.
    kvsModuleCategory( Object );
    kvsModuleBaseClass( kvs::PointObject );

protected:

    size_t  m_width;
    size_t  m_height;

public:

    PointCloudObject( void );

    virtual ~PointCloudObject( void );

public:

    void initialize( void );

    void update(
        kvs::ni::DepthGenerator& depth );

    void update(
        kvs::ni::ImageGenerator& image,
        kvs::ni::DepthGenerator& depth );

private:

    void calculate_coords( kvs::ni::DepthGenerator& depth );

    void calculate_colors( kvs::ni::ImageGenerator& image );

};

}

}

#endif
