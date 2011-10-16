//
//  main.cpp
//  
//
//  Created by Jun Nishimura on 10/16/11.
//  Copyright (c) 2011 Jun Nishimura. All rights reserved.
//

#include <kvs/glut/Application>
#include <kvs/glut/Screen>
#include <kvs/CommandLine>
#include <kvs/ImageImporter>

#include "ImageProcess.h"

ImageProcess* g_processor = NULL;

class Argument : public kvs::CommandLine
{

public:

    Argument( int argc, char** argv ) :
        CommandLine( argc, argv )
    {
        this->add_help_option();
        this->add_value( "kvs::ImageObject file path." );
        if ( !this->parse() ) exit( EXIT_FAILURE );
    }
};

void deleteAtExit( void )
{
    if ( g_processor )
    {
        delete g_processor;
        g_processor = NULL;
    }
}

class PaintEvent : public kvs::PaintEventListener
{
    void update( void )
    {
        g_processor->render();
    }
};

class KeyPressEvent : public kvs::KeyPressEventListener
{
    void update( kvs::KeyEvent* event )
    {
        g_processor->keyPressEvent( event );
    }
};

int main( int argc, char** argv )
{
    atexit( deleteAtExit );
    Argument arg( argc, argv );
    kvs::ImageObject* image = new kvs::ImageImporter( arg.value<std::string>() );
    if ( !image )
    {
        kvsMessageError( "Cannot read image." );
        return( 0 );
    }

    g_processor = new ImageProcess( image );

    kvs::glut::Application app( argc, argv );
    kvs::glut::Screen screen( &app );

    PaintEvent paint_event;
    screen.addPaintEvent( &paint_event );

    KeyPressEvent key_press_event;
    screen.addKeyPressEvent( &key_press_event );

    screen.show();

    g_processor->initialize();

    return( app.run() );
}
