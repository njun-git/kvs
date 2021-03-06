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
#include <kvs/RotationMatrix33>

#include <kvs/CommandLine>
#include <kvs/UnstructuredVolumeImporter>
#include <kvs/UnstructuredVectorToScalar>
#include <kvs/PolygonImporter>
#include <kvs/LineImporter>
#include <kvs/PointImporter>

#include "NullObject.h"
#include "StochasticRenderer.h"
#include "StochasticVolumeRenderer.h"
#include "StochasticPolygonRenderer.h"
#include "StochasticLineRenderer.h"
#include "StochasticPointRenderer.h"

#include "PolygonToPolygon.h"

class Argument : public kvs::CommandLine
{

public:

    Argument( int argc, char** argv ) :
        CommandLine( argc, argv )
    {
        add_help_option();
        add_option( "r", "[size_t] Repeat level. ( default : 1 )", 1, false );

        add_option( "volume", "[string] kvs::UnstructuredVolumeObject file path. ( optional )", 1, false );
        add_option( "point", "[string] kvs::PointObject file path. ( optional )", 1, false );
        add_option( "polygon", "[string] kvs::PolygonObject file path. ( optional )", 1, false );
        add_option( "line", "[string] kvs::LineObject file path. ( optional )", 1, false );
        add_option( "tfunc", "[string] kvs::TransferFunction file path. ( optional )", 1, false );

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
    kvs::tdw::Application app( argc, argv );
    kvs::tdw::Screen screen( &app );
    screen.setTitle( "StochasticRenderer" );

    KeyPressEvent key_press_event;
    screen.addKeyPressEvent( &key_press_event );

    screen.show();

    if ( kvs::tdw::Application::IsRenderer() )
    {

    const size_t repeat_level = arg.hasOption( "r" ) ? arg.optionValue<size_t>( "r" ) : 1;
    kvs::glew::StochasticRenderer* renderer = new kvs::glew::StochasticRenderer( repeat_level );
    renderer->enableLODControl();

    kvs::NullObject* null = NULL;

    if ( arg.hasOption( "volume" ) )
    {
        const std::string filename = arg.optionValue<std::string>( "volume" );
        kvs::UnstructuredVolumeObject* volume = new kvs::UnstructuredVolumeImporter( filename );
        if ( volume->veclen() == 3 )
        {
            kvs::UnstructuredVolumeObject* volume2 = new kvs::UnstructuredVectorToScalar( volume );
            delete volume;
            volume = volume2;
        }

        kvs::glew::StochasticVolumeRenderer* volume_renderer = new kvs::glew::StochasticVolumeRenderer( volume );
        if ( arg.hasOption( "e" ) ) volume_renderer->setEdgeSize( arg.optionValue<float>( "e" ) );
        if ( arg.hasOption( "DisableShading" ) ) volume_renderer->disableShading();
        else                                     volume_renderer->setShader( kvs::Shader::BlinnPhong() );

        if ( arg.hasOption( "tfunc" ) )
        {
            kvs::TransferFunction tfunc( arg.optionValue<std::string>( "tfunc" ) );
            volume_renderer->setTransferFunction( tfunc );
        }
        renderer->registerRenderer( volume_renderer );
        if ( !null ) null = new kvs::NullObject( volume );
    }

    if ( arg.hasOption( "polygon" ) )
    {
        kvs::PolygonObject* import_polygon = new kvs::PolygonImporter( arg.optionValue<std::string>( "polygon" ) );
        kvs::PolygonObject* polygon = new kvs::PolygonToPolygon( import_polygon );
        delete import_polygon;

        polygon->setOpacity( 64 );

        kvs::glew::StochasticPolygonRenderer* polygon_renderer = new kvs::glew::StochasticPolygonRenderer( polygon );
        polygon_renderer->setShader( kvs::Shader::BlinnPhong() );

        renderer->registerRenderer( polygon_renderer );
        if ( !null ) null = new kvs::NullObject( polygon );
    }

    if ( arg.hasOption( "line" ) )
    {
        kvs::LineObject* line = new kvs::LineImporter( arg.optionValue<std::string>( "line" ) );

        kvs::glew::StochasticLineRenderer* line_renderer = new kvs::glew::StochasticLineRenderer( line );
        //line_renderer->setOpacity( 64 );

        renderer->registerRenderer( line_renderer );
        if ( !null ) null = new kvs::NullObject( line );
    }

    if ( arg.hasOption( "point" ) )
    {
        kvs::PointObject* point = new kvs::PointImporter( arg.optionValue<std::string>( "point" ) );

        kvs::glew::StochasticPointRenderer* point_renderer = new kvs::glew::StochasticPointRenderer( point, repeat_level );
        point_renderer->setShader( kvs::Shader::BlinnPhong() );

        renderer->registerRenderer( point_renderer );
        if ( !null ) null = new kvs::NullObject( point );
    }

    if ( null ) screen.registerObject( null, renderer );

    }

    return( app.run() );
}
