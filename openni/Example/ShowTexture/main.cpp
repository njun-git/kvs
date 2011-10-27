//
//  main.cpp
//  
//
//  Created by Jun Nishimura on 10/17/11.
//  Copyright (c) 2011 Jun Nishimura. All rights reserved.
//

#include <kvs/ni/OpenNI>
#include <kvs/ni/Context>
#include <kvs/ni/DepthGenerator>
#include <kvs/ni/UserGenerator>

#include <kvs/ni/TextureObject>
#include <kvs/ni/TextureRenderer>

#include <kvs/glut/Application>
#include <kvs/glut/Screen>
#include <kvs/glut/Timer>

kvs::ni::Context                g_context;
kvs::ni::DepthGenerator         g_depth;
kvs::ni::UserGenerator          g_user;
kvs::ni::TextureObject*         g_texture = NULL;
kvs::ni::TextureRenderer*       g_renderer = NULL;

class TimerEvent : public kvs::TimerEventListener
{
    void update( kvs::TimeEvent* event )
    {
        g_context.update( g_depth );
        g_context.update( g_user );
        if ( g_texture ) g_texture->update( g_depth, g_user );

        screen()->redraw();
    }
};

class KeyPressEvent : public kvs::KeyPressEventListener
{
    void update( kvs::KeyEvent* event )
    {
        switch( event->key() )
        {
            case kvs::Key::Up:
            {
                if ( g_renderer->scale() + 0.05f <= 1.0f )
                {
                    g_renderer->setScale( g_renderer->scale() + 0.05f );
                }
                break;
            }
            case kvs::Key::Down:
            {
                if ( g_renderer->scale() - 0.05f > 0.0f )
                {
                    g_renderer->setScale( g_renderer->scale() - 0.05f );
                }
                break;
            }
            case kvs::Key::c:
                g_renderer->setType( kvs::ni::TextureRenderer::Center ); break;
            case kvs::Key::s:
                g_renderer->setType( kvs::ni::TextureRenderer::Stretch ); break;
            case kvs::Key::L:
                g_renderer->setType( kvs::ni::TextureRenderer::LeftTop ); break;
            case kvs::Key::l:
                g_renderer->setType( kvs::ni::TextureRenderer::LeftBottom ); break;
            case kvs::Key::R:
                g_renderer->setType( kvs::ni::TextureRenderer::RightTop ); break;
            case kvs::Key::r:
                g_renderer->setType( kvs::ni::TextureRenderer::RightBottom ); break;
            default: break;
        }
    }
};

int main( int argc, char** argv )
{
    g_context.initialize();

    g_depth.create( g_context );
    g_user.create( g_context );

    kvs::glut::Application app( argc, argv );
    kvs::glut::Screen screen( &app );
    screen.setTitle( "kvs::ni::TextureRenderer" );
    screen.setGeometry( 0, 0, 960, 640 );
    screen.background()->setColor( kvs::RGBColor( 48, 48, 48 ) );

    TimerEvent timer_event;
    kvs::glut::Timer timer( &timer_event, 33 );
    screen.addTimerEvent( &timer_event, &timer );

    KeyPressEvent key_press_event;
    screen.addKeyPressEvent( &key_press_event );

    screen.show();

    g_texture = new kvs::ni::TextureObject();
    g_renderer = new kvs::ni::TextureRenderer();
    screen.registerObject( g_texture, g_renderer );

    g_context.setMirror( true );
    g_context.startGeneration();

    return( app.run() );
}
