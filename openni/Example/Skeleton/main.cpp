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
#include <kvs/ni/Skeleton>
#include <kvs/ni/PointCloudObject>
#include <kvs/ni/SkeletonPolygonObject>
#include <kvs/ni/TextureObject>
#include <kvs/ni/TextureRenderer>

#include <kvs/glut/Application>
#include <kvs/glut/Screen>
#include <kvs/glut/Timer>

#include <kvs/CommandLine>

kvs::ni::Context                g_context;
kvs::ni::DepthGenerator         g_depth;
kvs::ni::UserGenerator          g_user;
kvs::ni::PointCloudObject*      g_cloud = NULL;
kvs::ni::SkeletonPolygonObject* g_skeleton = NULL;
kvs::ni::TextureObject*         g_texture = NULL;
kvs::ni::TextureRenderer*       g_renderer = NULL;

class Argument : public kvs::CommandLine
{

public:

    Argument( int argc, char** argv ) :
        CommandLine( argc, argv )
    {
        add_help_option();
        add_option( "PointCloud", "Show point cloud. ( default : none )", 0, false );

        if( !this->parse() ) exit( EXIT_FAILURE );
    }

};

class NewUser : public kvs::ni::NewUserEventListener
{
    void update( const unsigned int user )
    {
        std::cout << "# of users : " << g_user.nusers() << ", new user : " << user << std::endl;

        g_user.pose().startDetection( "Psi", user );
    }
};

class PoseDetected : public kvs::ni::PoseStartEventListener
{
    void update( const std::string& pose, const unsigned int user )
    {
        std::cout << pose << " pose detected : " << user << std::endl;
        g_user.pose().stopDetection( user );
        g_user.skeleton().requestCalibration( user, true );
    }
};

class CalibrationEnd : public kvs::ni::CalibrationEndEventListener
{
    void update( const unsigned int user, const bool is_success )
    {
        if ( is_success )
        {
            std::cout << "Calibration end" << std::endl;
            g_user.skeleton().startTracking( user );
        }
        else
        {
            std::cout << "Calibration fail" << std::endl;
            g_user.skeleton().requestCalibration( user, true );
        }
    }
};

class TimerEvent : public kvs::TimerEventListener
{
    void update( kvs::TimeEvent* event )
    {
        g_context.update( g_depth );
        g_context.update( g_user );
        if ( g_cloud ) g_cloud->update( g_depth );
        if ( g_skeleton ) g_skeleton->update( g_depth, g_user );
        if ( g_texture ) g_texture->update( g_depth, g_user );

        screen()->redraw();
    }
};

int main( int argc, char** argv )
{
    Argument arg( argc, argv );

    g_context.initialize();

    NewUser new_user;
    g_user.addNewUserEvent( &new_user );

    PoseDetected pose_detected;
    g_user.pose().addPoseStartEvent( &pose_detected );

    CalibrationEnd calibration_end;
    g_user.skeleton().addCalibrationEndEvent( &calibration_end );

    g_depth.create( g_context );
    g_user.create( g_context );
    g_user.skeleton().setSmoothing( 0.75f );

    kvs::glut::Application app( argc, argv );
    kvs::glut::Screen screen( &app );
    screen.setTitle( "kvs::ni::Skeleton" );
    screen.setGeometry( 0, 0, 960, 640 );
    screen.background()->setColor( kvs::RGBColor( 48, 48, 48 ) );

    TimerEvent timer_event;
    kvs::glut::Timer timer( &timer_event, 33 );
    screen.addTimerEvent( &timer_event, &timer );

    screen.show();

    g_context.setMirror( true );
    g_context.startGeneration();

    if ( arg.hasOption( "PointCloud" ) )
    {
        g_cloud = new kvs::ni::PointCloudObject();
        g_cloud->setColor( kvs::RGBColor( 192, 192, 192 ) );
        screen.registerObject( g_cloud );
    }

    g_texture = new kvs::ni::TextureObject();
    g_renderer = new kvs::ni::TextureRenderer();
    g_renderer->setType( kvs::ni::TextureRenderer::RightBottom );
    g_renderer->setScale( 0.3f );
    screen.registerObject( g_texture, g_renderer );

    g_skeleton = new kvs::ni::SkeletonPolygonObject();
    screen.registerObject( g_skeleton );

    return( app.run() );
}
