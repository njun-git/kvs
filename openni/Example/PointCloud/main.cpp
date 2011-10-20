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
#include <kvs/ni/ImageGenerator>
#include <kvs/ni/PointCloudObject>

#include <kvs/glut/Application>
#include <kvs/glut/Screen>
#include <kvs/glut/Timer>

kvs::ni::Context            g_context;
kvs::ni::DepthGenerator     g_depth;
kvs::ni::ImageGenerator     g_image;
kvs::ni::PointCloudObject*  g_cloud = NULL;

class TimerEvent : public kvs::TimerEventListener
{
    void update( kvs::TimeEvent* event )
    {
        g_context.update( g_depth );
        g_context.update( g_image );
        if ( g_cloud ) g_cloud->update( g_image, g_depth );

        screen()->redraw();
    }
};

int main( int argc, char** argv )
{
    g_context.initialize();

    g_depth.create( g_context );
    g_image.create( g_context );

    kvs::glut::Application app( argc, argv );
    kvs::glut::Screen screen( &app );
    screen.setTitle( "kvs::ni::PointCloudObject" );
    screen.setGeometry( 0, 0, 960, 640 );
    screen.background()->setColor( kvs::RGBColor( 48, 48, 48 ) );

    TimerEvent timer_event;
    kvs::glut::Timer timer( &timer_event, 33 );
    screen.addTimerEvent( &timer_event, &timer );

    screen.show();

    g_cloud = new kvs::ni::PointCloudObject();

    g_context.setMirror( true );
    g_context.startGeneration();

    screen.registerObject( g_cloud );

    return( app.run() );
}
