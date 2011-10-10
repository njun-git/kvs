//
//  main.cpp
//  
//
//  Created by njun on 11/25/7.
//  Copyright 2011 Jun Nishimura. All rights reserved.
//

#include <kvs/glut/Application>
#include <kvs/glut/Screen>

#include <kvs/glut/GLUT>

#include "XformManager.h"

class PaintEvent : public kvs::PaintEventListener
{
    void update( void )
    {
        glEnable( GL_DEPTH_TEST );
        glEnable( GL_LIGHTING );
        glutSolidTeapot( 2.5f );
        glDisable( GL_LIGHTING );
        glDisable( GL_DEPTH_TEST );
    }
};

class KeyPressEvent : public kvs::KeyPressEventListener
{
    void update( kvs::KeyEvent* event )
    {
        switch ( event->key() )
        {
            case kvs::Key::x:
            {
                kvs::XformManager manager( screen() );
                if ( manager.saveXform( "xform.dat" ) )
                {
                    std::cout << "Write current xform to xform.dat" <<std::endl;
                }
                break;
            }
            case kvs::Key::X:
            {
                kvs::XformManager manager( screen() );
                if ( manager.applyXform( "xform.dat" ) )
                {
                    std::cout << "Apply saved xform to xform.dat" <<std::endl;
                }
                break;
            }
            default: break;
        }
    }
};

int main( int argc, char** argv )
{
    kvs::glut::Application app( argc, argv );
    kvs::glut::Screen screen( &app );
    screen.setTitle( "kvs::XformManager" );

    PaintEvent paint_event;
    screen.addPaintEvent( &paint_event );

    KeyPressEvent key_press_event;
    screen.addKeyPressEvent( &key_press_event );

    screen.show();

    return( app.run() );
}
