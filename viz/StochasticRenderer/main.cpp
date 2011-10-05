//
//  main.cpp
//  
//
//  Created by njun on 11/25/7.
//  Copyright 2011 Jun Nishimura. All rights reserved.
//

#include <kvs/glut/Application>
#include <kvs/glut/Screen>

#include <kvs/CommandLine>
#include <kvs/UnstructuredVolumeImporter>
#include <kvs/UnstructuredVectorToScalar>
#include <kvs/PolygonImporter>
#include <kvs/LineImporter>
#include <kvs/ExternalFaces>

#include "NullObject.h"
#include "StochasticRenderer.h"
#include "StochasticPTRenderer.h"
#include "StochasticPolygonRenderer.h"
#include "StochasticLineRenderer.h"
#include "PolygonToPolygon.h"

class Argument : public kvs::CommandLine
{

public:

    Argument( int argc, char** argv ) :
        CommandLine( argc, argv )
    {
        add_help_option();
        add_option( "r", "[size_t] Repeat level. ( default : 1 )", 1, false );

        add_option( "v", "[string] kvs::UnstructuredVolumeObject file path. ( optional )", 1, false );
        add_option( "p", "[string] kvs::PolygonObject file path. ( optional )", 1, false );
        add_option( "l", "[string] kvs::LineObject file path. ( optional )", 1, false );
        add_option( "t", "[string] kvs::TransferFunction file path. ( optional )", 1, false );

        add_option( "e", "[float]  Edge size of volume. ( default : 1 )", 1, false );
        add_option( "DisableShading", "Disable shading. ( default : eable shading )", 0, false );

        if( !this->parse() ) exit( EXIT_FAILURE );
    }

};

class KeyPressEvent : public kvs::KeyPressEventListener
{
    void update( kvs::KeyEvent* event )
    {
        switch ( event->key() )
        {
        case kvs::Key::o: screen()->controlTarget() = kvs::ScreenBase::TargetObject; break;
        case kvs::Key::l: screen()->controlTarget() = kvs::ScreenBase::TargetLight; break;
            default: break;
        }
    }
};

int main( int argc, char** argv )
{
    Argument arg( argc, argv );
    kvs::glut::Application app( argc, argv );
    kvs::glut::Screen screen( &app );
    screen.setTitle( "SPTRenderer" );

    KeyPressEvent key_press_event;
    screen.addKeyPressEvent( &key_press_event );

    screen.show();

    const size_t repeat_level = arg.hasOption( "r" ) ? arg.optionValue<size_t>( "r" ) : 1;
    kvs::glew::StochasticRenderer* renderer = new kvs::glew::StochasticRenderer( repeat_level );
    renderer->enableLODControl();

    kvs::NullObject* null = NULL;

    if ( arg.hasOption( "v" ) )
    {
        const std::string filename = arg.optionValue<std::string>( "v" );
        kvs::UnstructuredVolumeObject* volume = new kvs::UnstructuredVolumeImporter( filename );
        if ( volume->veclen() == 3 )
        {
            kvs::UnstructuredVolumeObject* volume2 = new kvs::UnstructuredVectorToScalar( volume );
            delete volume;
            volume = volume2;
        }

        kvs::glew::StochasticPTRenderer* spt_renderer = new kvs::glew::StochasticPTRenderer( volume );
        if ( arg.hasOption( "e" ) ) spt_renderer->setEdgeSize( arg.optionValue<float>( "e" ) );
        if ( arg.hasOption( "DisableShading" ) )
        {
            spt_renderer->disableShading();
        }
        else
        {
            spt_renderer->setShader( kvs::Shader::BlinnPhong() );
        }
        if ( arg.hasOption( "t" ) )
        {
            kvs::TransferFunction tfunc( arg.optionValue<std::string>( "t" ) );
            spt_renderer->setTransferFunction( tfunc );
        }
        renderer->registerRenderer( volume, spt_renderer );
        if ( !null ) null = new kvs::NullObject( volume );
    }

    if ( arg.hasOption( "p" ) )
    {
        kvs::PolygonObject* import_polygon = new kvs::PolygonImporter( arg.optionValue<std::string>( "p" ) );
        kvs::PolygonObject* polygon = new kvs::PolygonToPolygon( import_polygon );
        delete import_polygon;

        polygon->setOpacity( 64 );

        kvs::glew::StochasticPolygonRenderer* polygon_renderer = new kvs::glew::StochasticPolygonRenderer( polygon );
        polygon_renderer->setShader( kvs::Shader::BlinnPhong() );

        renderer->registerRenderer( polygon, polygon_renderer );
        if ( !null ) null = new kvs::NullObject( polygon );
    }

    if ( arg.hasOption( "l" ) )
    {
        kvs::LineObject* line = new kvs::LineImporter( arg.optionValue<std::string>( "l" ) );

        kvs::glew::StochasticLineRenderer* line_renderer = new kvs::glew::StochasticLineRenderer( line );
        line_renderer->setOpacity( 64 );

        renderer->registerRenderer( line, line_renderer );
        if ( !null ) null = new kvs::NullObject( line );
    }

    if ( null ) screen.registerObject( null, renderer );

    return( app.run() );
}
