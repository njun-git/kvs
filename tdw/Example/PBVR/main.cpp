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

#include <kvs/CommandLine>
#include <kvs/UnstructuredVolumeImporter>
#include <kvs/UnstructuredVectorToScalar>
#include <kvs/StructuredVolumeImporter>
#include <kvs/StructuredVectorToScalar>
#include <kvs/CellByCellMetropolisSampling>

#include "NullObject.h"
#include "StochasticRenderer.h"
#include "StochasticPointRenderer.h"

class Argument : public kvs::CommandLine
{

public:

    Argument( int argc, char** argv ) :
        CommandLine( argc, argv )
    {
        add_help_option();
        add_option( "r", "[size_t] Repeat level. ( default : 1 )", 1, false );

        add_option( "structure", "[string] kvs::StructuredVolumeObject file path. ( optional )", 1, false );
        add_option( "unstructure", "[string] kvs::UnstructuredVolumeObject file path. ( optional )", 1, false );
        add_option( "tfunc", "[string] kvs::TransferFunction file path. ( optional )", 1, false );

        add_option( "s", "[float] sampling step for PBVR. ( default : 1 )", 1, false );
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
    screen.setTitle( "PBVR" );

    KeyPressEvent key_press_event;
    screen.addKeyPressEvent( &key_press_event );

    screen.show();

    if ( kvs::tdw::Application::IsRenderer() )
    {

    const size_t repeat_level = arg.hasOption( "r" ) ? arg.optionValue<size_t>( "r" ) : 1;
    kvs::glew::StochasticRenderer* renderer = new kvs::glew::StochasticRenderer( repeat_level );
    renderer->disableLODControl();

    const size_t subpixel_level = size_t( sqrtf( float(repeat_level) ) );
    const float step = arg.hasOption( "s" ) ? arg.optionValue<float>( "s" ) : 1.0f;

    kvs::NullObject* null = NULL;

    if ( arg.hasOption( "unstructure" ) )
    {
        const std::string filename = arg.optionValue<std::string>( "unstructure" );
        kvs::UnstructuredVolumeObject* volume = new kvs::UnstructuredVolumeImporter( filename );
        if ( volume->veclen() == 3 )
        {
            kvs::UnstructuredVolumeObject* volume2 = new kvs::UnstructuredVectorToScalar( volume );
            delete volume;
            volume = volume2;
        }

        kvs::TransferFunction tfunc;
        if ( arg.hasOption( "tfunc" ) )
        {
            tfunc = kvs::TransferFunction( arg.optionValue<std::string>( "tfunc" ) );
        }

        kvs::PointObject* point = new kvs::CellByCellMetropolisSampling(
            volume, subpixel_level, step, tfunc );
        delete volume;

        kvs::glew::StochasticPointRenderer* point_renderer = new kvs::glew::StochasticPointRenderer( point, repeat_level );
        point_renderer->setShader( kvs::Shader::BlinnPhong() );

        renderer->registerRenderer( point_renderer );
        if ( !null ) null = new kvs::NullObject( point );
    }

    if ( arg.hasOption( "structure" ) )
    {
        const std::string filename = arg.optionValue<std::string>( "structure" );
        kvs::StructuredVolumeObject* volume = new kvs::StructuredVolumeImporter( filename );
        if ( volume->veclen() == 3 )
        {
            kvs::StructuredVolumeObject* volume2 = new kvs::StructuredVectorToScalar( volume );
            delete volume;
            volume = volume2;
        }

        kvs::TransferFunction tfunc;
        if ( arg.hasOption( "tfunc" ) )
        {
            tfunc = kvs::TransferFunction( arg.optionValue<std::string>( "tfunc" ) );
        }

        kvs::PointObject* point = new kvs::CellByCellMetropolisSampling(
            volume, subpixel_level, step, tfunc );
        delete volume;

        kvs::glew::StochasticPointRenderer* point_renderer = new kvs::glew::StochasticPointRenderer( point, repeat_level );
        point_renderer->setShader( kvs::Shader::BlinnPhong() );

        renderer->registerRenderer( point_renderer );
        if ( !null ) null = new kvs::NullObject( point );
    }

    if ( null ) screen.registerObject( null, renderer );

    }

    return( app.run() );
}
