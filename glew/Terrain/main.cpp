/*
 *  main.cpp
 *  
 *
 *  Created by njun on 11/10/10.
 *  Copyright 2011 Jun Nishimura. All rights reserved.
 *
 */

#include "PaintScreen.h"
#include "TerrainScreen.h"

int main( int argc, char** argv )
{
    kvs::glut::Application app( argc, argv );

    kvs::PaintScreen paint_screen( &app );
    paint_screen.show();

    kvs::TerrainScreen terrain_screen( &app, paint_screen.image() );
    terrain_screen.show();

    paint_screen.attachScreen( &terrain_screen );

    return( app.run() );
}
