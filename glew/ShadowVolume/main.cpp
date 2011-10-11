//
//  main.cpp
//  
//
//  Created by Jun Nishimura on 11/9/10.
//  Copyright 2011 Jun Nishimura. All rights reserved.
//

#include <kvs/glut/Application>
#include <kvs/glut/Screen>

#include <kvs/CommandLine>
#include <kvs/PolygonImporter>

#include "PolygonToPolygon.h"
#include "ShadowVolumeRenderer.h"

kvs::glew::ShadowVolumeRenderer*    g_renderer = NULL;
float   g_shadow_strength = 0.5f; // default value.

class Argument : public kvs::CommandLine
{

public:

    Argument( int argc, char** argv ) :
        CommandLine( argc, argv )
    {
        add_help_option();
        add_value( "kvs::PolygonObject file path.", true );

        if( !this->parse() ) exit( EXIT_FAILURE );
    }

};

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
            {
                g_renderer->enableDebugDraw();
                break;
            }
            case kvs::Key::D:
            {
                g_renderer->disableDebugDraw();
                break;
            }
            case kvs::Key::Up:
            {
                g_shadow_strength += 0.05f;
                if ( g_shadow_strength > 1.0f ) g_shadow_strength = 1.0f;
                g_renderer->setShadowStrength( g_shadow_strength );
                break;
            }
            case kvs::Key::Down:
            {
                g_shadow_strength -= 0.05f;
                if ( g_shadow_strength < 0.0f ) g_shadow_strength = 0.0f;
                g_renderer->setShadowStrength( g_shadow_strength );
                break;
            }
            default: break;
        }
    }
};

int main( int argc, char** argv )
{
    Argument arg( argc, argv );
    kvs::glut::Application app( argc, argv );
    kvs::glut::Screen screen( &app );
    screen.setTitle( "kvs::glew::ShadowVolumeRenderer" );

    // Add user key press event.
    KeyPressEvent key_press_event;
    screen.addKeyPressEvent( &key_press_event );

    screen.show();

    kvs::PolygonObject* origin = new kvs::PolygonImporter( arg.value<std::string>() );
    if ( !origin )
    {
        kvsMessageError( "Cannot read polygon object." );
        return( EXIT_FAILURE );
    }

    kvs::PolygonObject* object = new kvs::PolygonToPolygon( origin );
    if ( !object )
    {
        kvsMessageError( "Cannot convert polygon object." );
        return( EXIT_FAILURE );
    }
    delete origin;

    g_renderer = new kvs::glew::ShadowVolumeRenderer();
    if ( !g_renderer )
    {
        kvsMessageError( "Cannot allocate renderer." );
        delete object;
        return( EXIT_FAILURE );
    }

    screen.registerObject( object, g_renderer );

    return( app.run() );
}
