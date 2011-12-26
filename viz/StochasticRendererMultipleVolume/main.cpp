//
//  main.cpp
//  
//
//  Created by njun on 11/25/7.
//  Copyright 2011 Jun Nishimura. All rights reserved.
//

#include <kvs/glut/Application>
#include <kvs/glut/Screen>
#include <kvs/glut/TransferFunctionEditor>

#include <kvs/CommandLine>
#include <kvs/UnstructuredVolumeImporter>
#include <kvs/UnstructuredVectorToScalar>
#include <kvs/PolygonImporter>
#include <kvs/LineImporter>
#include <kvs/PointImporter>

#include <kvs/Time>
#include <kvs/Bmp>
#include <kvs/Camera>
#include <kvs/RGBFormulae>

#include "NullObject.h"
#include "StochasticRenderer.h"
#include "StochasticVolume2Renderer.h"

#include "PolygonToPolygon.h"
#include "XformManager.h"

class Argument : public kvs::CommandLine
{

public:

    Argument( int argc, char** argv ) :
        CommandLine( argc, argv )
    {
        add_help_option();
        add_option( "r", "[size_t] Repeat level. ( default : 1 )", 1, false );

        add_option( "volume1", "[string] kvs::UnstructuredVolumeObject file path. ( optional )", 1, false );
        add_option( "volume2", "[string] kvs::UnstructuredVolumeObject file path. ( optional )", 1, false );
        add_option( "tfunc1", "[string] kvs::TransferFunction file path. ( optional )", 1, false );
        add_option( "tfunc2", "[string] kvs::TransferFunction file path. ( optional )", 1, false );

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
            case kvs::Key::f:
            {
                const kvs::RendererBase* r = screen()->rendererManager()->renderer();
                std::cout << "Rendering time : " << r->timer().msec() << " [msec]" << std::endl; break;
            }
            case kvs::Key::s:
            {
                kvs::Time now; now.now();
                std::string filename( std::string( "ScreenShot_" ) + now.toString( "" ) + std::string( ".bmp" ) );
                kvs::Camera camera;
                kvs::ColorImage screen_shot = camera.snapshot();

                kvs::Bmp* bmp = new kvs::Bmp( screen_shot.width(), screen_shot.height(), screen_shot.data() );
                if ( bmp->write( filename ) )
                {
                    std::cout << "Write screen shot to " << filename << std::endl;
                }
                else
                {
                    kvsMessageError( "Cannot write image." );
                }
                delete bmp;
                break;
            }
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
    Argument arg( argc, argv );
    kvs::glut::Application app( argc, argv );
    kvs::glut::Screen screen( &app );
    screen.setTitle( "StochasticRenderer" );

    KeyPressEvent key_press_event;
    screen.addKeyPressEvent( &key_press_event );

    screen.show();

    const size_t repeat_level = arg.hasOption( "r" ) ? arg.optionValue<size_t>( "r" ) : 1;
    kvs::glew::StochasticRenderer* renderer = new kvs::glew::StochasticRenderer( repeat_level );
    renderer->enableLODControl();

    kvs::NullObject* null = NULL;
    kvs::UnstructuredVolumeObject* volume1 = NULL;
    kvs::UnstructuredVolumeObject* volume2 = NULL;

    if ( arg.hasOption( "volume1" ) )
    {
        const std::string filename = arg.optionValue<std::string>( "volume1" );
        volume1 = new kvs::UnstructuredVolumeImporter( filename );
        if ( volume1->veclen() == 3 )
        {
            kvs::UnstructuredVolumeObject* volume_s = new kvs::UnstructuredVectorToScalar( volume1 );
            delete volume1;
            volume1 = volume_s;
        }

        if ( !null ) null = new kvs::NullObject( volume1 );
    }

    if ( arg.hasOption( "volume2" ) )
    {
        const std::string filename = arg.optionValue<std::string>( "volume2" );
        volume2 = new kvs::UnstructuredVolumeImporter( filename );
        if ( volume2->veclen() == 3 )
        {
            kvs::UnstructuredVolumeObject* volume_s = new kvs::UnstructuredVectorToScalar( volume2 );
            delete volume2;
            volume2 = volume_s;
        }

        if ( !null ) null = new kvs::NullObject( volume2 );
        else         null->update( volume2 );
    }

    if ( volume1 && volume2 )
    {
        kvs::glew::StochasticVolume2Renderer* volume_renderer
            = new kvs::glew::StochasticVolume2Renderer( volume1, volume2 );

        if ( arg.hasOption( "tfunc1" ) )
        {
            kvs::TransferFunction tfunc( arg.optionValue<std::string>( "tfunc1" ) );
            volume_renderer->setTransferFunction( tfunc, 0 );
        }

        if ( arg.hasOption( "tfunc2" ) )
        {
            kvs::TransferFunction tfunc( arg.optionValue<std::string>( "tfunc2" ) );
            volume_renderer->setTransferFunction( tfunc, 1 );
        }
        else
        {
            kvs::TransferFunction tfunc;
            tfunc.setColorMap( kvs::RGBFormulae::PM3D( 256 ) );
            volume_renderer->setTransferFunction( tfunc, 1 );
        }

        if ( arg.hasOption( "e" ) ) volume_renderer->setEdgeSize( arg.optionValue<float>( "e" ) );
        if ( arg.hasOption( "DisableShading" ) ) volume_renderer->disableShading();

        renderer->registerRenderer( volume_renderer );
    }
    else
    {
        if ( volume1 ) { delete volume1; volume1 = NULL; }
        if ( volume2 ) { delete volume2; volume2 = NULL; }
        if ( null ) { delete null; null = NULL; }
    }

    if ( null ) screen.registerObject( null, renderer );

    return( app.run() );
}
