//
//  main.cpp
//  
//
//  Created by njun on 11/25/7.
//  Copyright 2011 Jun Nishimura. All rights reserved.
//

#include <kvs/tdw/Application>
#include <kvs/tdw/Screen>
#include <kvs/tdw/Timer>

#include <kvs/ni/OpenNI>
#include <kvs/ni/Context>
#include <kvs/ni/DepthGenerator>
#include <kvs/ni/UserGenerator>
#include <kvs/ni/TextureObject>
#include <kvs/ni/TextureRenderer>

#include "ScreenController.h"

kvs::ni::Context                g_context;
kvs::ni::DepthGenerator         g_depth;
kvs::ni::UserGenerator          g_user;
kvs::tdw::ScreenController*     g_controller = NULL;
kvs::ni::TextureObject*         g_texture = NULL;
kvs::ni::TextureRenderer*       g_renderer = NULL;

class NewUser : public kvs::ni::NewUserEventListener
{
    void update( const unsigned int user )
    {
        std::cout << "# of users : " << g_user.nusers() << ", new user : " << user << std::endl;

        //g_user.pose().startDetection( "Psi", user );
        g_user.skeleton().requestCalibration( user, true );
    }
};

class PoseDetected : public kvs::ni::PoseStartEventListener
{
    void update( const std::string& pose, const unsigned int user )
    {
        std::cout << pose << " pose detected : " << user << std::endl;
        if ( !g_user.skeleton().isTracking( user ) )
        {
            //g_user.pose().stopDetection( user );
            //g_user.skeleton().requestCalibration( user, true );
        }
        else
        {
        }
    }
};

class CalibrationEnd : public kvs::ni::CalibrationEndEventListener
{
    void update( const unsigned int user, const bool is_success )
    {
        if ( is_success )
        {
            std::cout << "Calibration success." << std::endl;
            g_user.skeleton().startTracking( user );
            //g_user.pose().startDetection( "Psi", user );
        }
        else
        {
            std::cout << "Calibration fail." << std::endl;
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
        if ( g_texture ) g_texture->update( g_depth, g_user );
        if ( g_controller ) g_controller->update( g_depth, g_user );
    }
};

int main( int argc, char** argv )
{
    kvs::tdw::Application app( argc, argv );

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

    kvs::tdw::Screen screen( &app );
    screen.setTitle( "kvs::tdw::ScreenControl" );

    TimerEvent timer_event;
    kvs::tdw::Timer timer( &timer_event, 66 );
    timer.setSendRenderer( false );
    screen.addTimerEvent( &timer_event, &timer );

    g_context.setMirror( true );
    g_context.startGeneration();

    g_texture = new kvs::ni::TextureObject();
    g_renderer = new kvs::ni::TextureRenderer();
    g_renderer->setType( kvs::ni::TextureRenderer::Stretch );
    g_renderer->setScale( 1.0f );
    screen.registerObject( g_texture, g_renderer );

    g_controller = new kvs::tdw::ScreenController( &screen, screen.masterSender() );

    screen.show();

    return( app.run() );
}
