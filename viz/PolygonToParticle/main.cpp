//
//  main.cpp
//  
//
//  Created by njun on 11/27/7.
//  Copyright 2011 Jun Nishimura. All rights reserved.
//

#include <kvs/glut/Application>
#include <kvs/glut/Screen>

#include <kvs/CommandLine>
#include <kvs/PolygonImporter>
#include <kvs/glew/ParticleVolumeRenderer>
#include <kvs/PointExporter>

#include "PolygonToParticle.h"

class Argument : public kvs::CommandLine
{

public:

    Argument( int argc, char** argv ) :
        CommandLine( argc, argv )
    {
        add_help_option();

        add_value( "Input file name.", true );
        add_option( "out", "Output file name. ( default : point_of_polygon.kvsml )", 1, false );

        add_option( "np", "[size_t] Number of particles. ( default : 100000 )", 1, false );
        add_option( "r", "[size_t] Repeat level to show result. ( default : 1 )", 1, false );

        if( !this->parse() ) exit( EXIT_FAILURE );
    }
};

int main( int argc, char** argv )
{
    Argument arg( argc, argv );

    kvs::glut::Application app( argc, argv );
    kvs::glut::Screen screen( &app );
    screen.setTitle( "SPTRenderer" );
    screen.show();

    const std::string infile = arg.value<std::string>();
    kvs::PolygonObject* polygon = new kvs::PolygonImporter( infile );

    const size_t np = arg.hasOption( "np" ) ? arg.optionValue<size_t>( "np" ) : 100000;
    kvs::PointObject* point = new kvs::PolygonToParticle( polygon, np );
    delete polygon;

    const size_t repeat = arg.hasOption( "r" ) ? arg.optionValue<size_t>( "r" ) : 1;
    kvs::glew::ParticleVolumeRenderer* renderer = new kvs::glew::ParticleVolumeRenderer( point, 1, repeat );
    renderer->enableLODControl();
    screen.registerObject( point, renderer );

    const std::string outfile = arg.hasOption( "out" ) ? arg.optionValue<std::string>( "out" ) : "point_of_polygon.kvsml";
    kvs::KVSMLObjectPoint* kvsml = new kvs::PointExporter<kvs::KVSMLObjectPoint>( point );
    kvsml->setWritingDataType( kvs::KVSMLObjectPoint::ExternalBinary );
    if ( kvsml->write( outfile ) )
    {
        std::cout << *point << std::endl;
        std::cout << "Write to " << outfile << std::endl;
    }
    delete kvsml;

    return( app.run() );
}
