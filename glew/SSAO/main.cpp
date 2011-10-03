//
//  main.cpp
//  
//
//  Created by Jun Nishimura on 11/22/9.
//  Copyright 2011 Jun Nishimura. All rights reserved.
//

#include <kvs/glut/Application>
#include <kvs/glut/Screen>
#include <kvs/glut/Slider>

#include <kvs/PolygonImporter>

#include "PolygonToPolygon.h"
#include "SSAORenderer.h"

kvs::glew::SSAORenderer* ssao_renderer = NULL;

class KeyPressEvent : public kvs::KeyPressEventListener
{
    void update( kvs::KeyEvent* event )
    {
        switch ( event->key() )
        {
            case kvs::Key::o:
                screen()->controlTarget() = kvs::ScreenBase::TargetObject; break;
            case kvs::Key::l:
                screen()->controlTarget() = kvs::ScreenBase::TargetLight; break;
            case kvs::Key::d:
                ssao_renderer->enableDebugDraw(); break;
            case kvs::Key::D:
                ssao_renderer->disableDebugDraw(); break;
            default: break;
        }
    }
};

int main( int argc, char** argv )
{
    if ( argc < 2 )
    {
        kvsMessageError( "Please input Polygon file path." );
        return(0);
    }

    kvs::glut::Application app( argc, argv );
    kvs::glut::Screen screen( &app );
    screen.setTitle( "kvs::glew::SSAORenderer" );

    KeyPressEvent key_press_event;
    screen.addKeyPressEvent( &key_press_event );

    screen.show();

    kvs::PolygonObject* original_polygon = new kvs::PolygonImporter( argv[1] );
    if ( !original_polygon )
    {
        kvsMessageError( "Cannot import polygon." );
        return( EXIT_FAILURE );
    }

    kvs::PolygonObject* polygon = new kvs::PolygonToPolygon( original_polygon );
    delete original_polygon;
    if ( !polygon )
    {
        kvsMessageError( "Cannot convert polygon." );
        return( EXIT_FAILURE );
    }

    ssao_renderer = new kvs::glew::SSAORenderer();
    if ( !ssao_renderer )
    {
        kvsMessageError( "Cannot allocate SSAO renderer." );
        return( EXIT_FAILURE );
    }

    screen.registerObject( polygon, ssao_renderer );

    return( app.run() );
}
