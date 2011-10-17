/*
 *  main.cpp
 *  
 *
 *  Created by njun on 11/07/09.
 *  Copyright 2011 Jun Nishimura. All rights reserved.
 *
 */

#include <kvs/tdw/Application>
#include <kvs/tdw/Screen>

class PaintEvent : public kvs::PaintEventListener
{
    void update( void )
    {
        if ( kvs::tdw::Application::IsRenderer() )
        {
            glBegin( GL_TRIANGLES );
            glColor3ub( 255,   0,   0 ); glVertex3d(  0.0,  3.0, 0.0 ); // c0, v0
            glColor3ub(   0, 255,   0 ); glVertex3d(  3.0, -3.0, 0.0 ); // c1, v1
            glColor3ub(   0,   0, 255 ); glVertex3d( -3.0, -3.0, 0.0 ); // c2, v2
            glEnd();
        }
    }
};

int main( int argc, char** argv )
{
    kvs::tdw::Application app( argc, argv );
    kvs::tdw::Screen screen( &app );

    PaintEvent paint_event;
    screen.addPaintEvent( &paint_event );

    screen.show();

    return ( app.run() );
}
