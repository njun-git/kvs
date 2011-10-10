/*
 *  main.cpp
 *  
 *
 *  Created by njun on 11/10/11.
 *  Copyright 2011 Jun Nishimura. All rights reserved.
 *
 */

#include "MainScreen.h"
#include "ControlScreen.h"

int main( int argc, char** argv )
{
    kvs::glut::Application app( argc, argv );

    kvs::MainScreen main_screen( &app );
    main_screen.show();

    kvs::ControlScreen control_screen( &app );
    control_screen.attachMainScreen( &main_screen );
    control_screen.show();

    return( app.run() );
}
