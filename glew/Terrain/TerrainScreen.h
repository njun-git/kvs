/*
 *  TerrainScreen.h
 *  
 *
 *  Created by njun on 11/10/11.
 *  Copyright 2011 Jun Nishimura. All rights reserved.
 *
 */

#ifndef KVS__TERRAIN_SCREEN_H_INCLUDE
#define KVS__TERRAIN_SCREEN_H_INCLUDE

#include <kvs/glut/Application>
#include <kvs/glut/Screen>

#include <kvs/ClassName>
#include <kvs/Module>

#include "TerrainObject.h"
#include "TerrainRenderer.h"

namespace kvs
{

class TerrainScreen : public kvs::glut::Screen
{

    // Class name.
    kvsClassName( kvs::TerrainScreen );

    // Module information.
    kvsModuleBaseClass( kvs::glut::Screen );

protected:

    kvs::TerrainObject*         m_object;
    kvs::glew::TerrainRenderer* m_renderer;

public:

    TerrainScreen( kvs::glut::Application* app, kvs::PaintImageObject* image );

    virtual ~TerrainScreen( void );

public:

    void initialize( kvs::glut::Application* app, kvs::PaintImageObject* image );

};

}

#endif
