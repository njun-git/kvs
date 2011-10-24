//
//  SkeletonPolygonObject.h
//  
//
//  Created by Jun Nishimura on 10/24/11.
//  Copyright (c) 2011 Jun Nishimura. All rights reserved.
//

#ifndef KVS__OPENNI__SKELETON_POLYGON_OBJECT_H_INCLUDE
#define KVS__OPENNI__SKELETON_POLYGON_OBJECT_H_INCLUDE

#include <kvs/ClassName>
#include <kvs/Module>
#include <kvs/PolygonObject>

#include "DepthGenerator.h"
#include "UserGenerator.h"

namespace kvs
{

namespace ni
{

namespace octahedron
{

extern const float coords[6][3];
extern const unsigned int connections[8][3];
extern const float normals[8][3];

}

class SkeletonPolygonObject : public kvs::PolygonObject
{

    // Class name.
    kvsClassName( kvs::ni::SkeletonPolygonObject );

    // Module information.
    kvsModuleCategory( Object );
    kvsModuleBaseClass( kvs::PolygonObject );

protected:

    size_t  m_width;
    size_t  m_height;

    kvs::ValueArray<kvs::ni::Skeleton::SkeletonJoint> m_joint_list;
    kvs::ValueArray<kvs::ni::Skeleton::SkeletonJoint> m_joint_connect;
    float   m_cx;
    float   m_cy;
    float   m_p2r;
    std::vector<float>          m_tmp_coords;
    std::vector<unsigned char>  m_tmp_colors;
    std::vector<float>          m_tmp_normals;
    std::vector<unsigned int>   m_tmp_connections;

public:

    SkeletonPolygonObject( void );

    virtual ~SkeletonPolygonObject( void );

public:

    void initialize( void );

    void update(
        kvs::ni::DepthGenerator& depth,
        kvs::ni::UserGenerator& user );

private:

    const kvs::Vector3f vertex_of_joint(
        const unsigned int index,
        const kvs::ni::Skeleton::SkeletonJoint& joint,
        kvs::ni::DepthGenerator& depth,
        kvs::ni::UserGenerator& user );

    void add_octahedron_of_joint(
        const unsigned int index,
        const kvs::ni::Skeleton::SkeletonJoint& joint,
        kvs::ni::DepthGenerator& depth,
        kvs::ni::UserGenerator& user,
        const kvs::RGBColor& color,
        const float size );

    void add_octahedron_of_joint_connect(
        const unsigned int index,
        const kvs::ni::Skeleton::SkeletonJoint& joint1,
        const kvs::ni::Skeleton::SkeletonJoint& joint2,
        kvs::ni::DepthGenerator& depth,
        kvs::ni::UserGenerator& user,
        const kvs::RGBColor& color,
        const float size );

};

}

}

#endif
