/*
 *  TerrainScreen.cpp
 *  
 *
 *  Created by njun on 11/10/11.
 *  Copyright 2011 Jun Nishimura. All rights reserved.
 *
 */

#include "TerrainScreen.h"

namespace kvs
{

TerrainScreen::TerrainScreen( kvs::glut::Application* app, kvs::PaintImageObject* image ) :
    kvs::glut::Screen( app )
{
    this->initialize( app, image );
}

TerrainScreen::~TerrainScreen( void )
{
}

void TerrainScreen::initialize( kvs::glut::Application* app, kvs::PaintImageObject* image )
{
    BaseClass::setTitle( "kvs::TerrainScreen" );
    BaseClass::setGeometry( 0, 0, 512, 512 );

    m_object = new kvs::TerrainObject( image->width() / 2, image->height() / 2 );
    m_renderer = new kvs::glew::TerrainRenderer();
    m_renderer->attachImage( image );

    BaseClass::registerObject( m_object, m_renderer );
}

}
