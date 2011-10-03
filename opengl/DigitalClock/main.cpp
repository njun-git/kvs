/*
 *  main.cpp
 *  
 *
 *  Created by njun on 11/08/23.
 *  Copyright 2011 Jun Nishimura. All rights reserved.
 *
 */

#include <kvs/glut/Application>
#include <kvs/glut/Screen>
#include <kvs/glut/Timer>

#include "DigitalClockObject.h"
#include "DigitalClockRenderer.h"

#include "AnalogClockObject.h"
#include "AnalogClockRenderer.h"

kvs::DigitalClockObject* digital = NULL;
kvs::AnalogClockObject* analog = NULL;

class TimerEvent : public kvs::TimerEventListener
{
    void update( kvs::TimeEvent* event )
    {
        kvs::Time time;
        time.now();
        digital->update( time );
        analog->update( time );
        screen()->redraw();
    }
};

int main( int argc, char** argv )
{
    kvs::glut::Application app( argc, argv );
    kvs::glut::Screen screen( &app );
    screen.setTitle( "DigitalClock" );
    screen.background()->setColor( kvs::RGBColor( 0, 0, 0 ) );
    screen.setGeometry( 0, 0, 512, 512 );

    TimerEvent timer_event;
    kvs::glut::Timer timer( &timer_event, 100 );
    screen.addTimerEvent( &timer_event, &timer );

    screen.show(); 

    digital = new kvs::DigitalClockObject();
    digital->setOffColor( kvs::RGBColor( 24, 24, 24 ) );
    kvs::DigitalClockRenderer* d_renderer = new kvs::DigitalClockRenderer();
    screen.registerObject( digital, d_renderer );

    analog = new kvs::AnalogClockObject();
    kvs::AnalogClockRenderer* a_renderer = new kvs::AnalogClockRenderer();
    screen.registerObject( analog, a_renderer );

    return( app.run() );
}
