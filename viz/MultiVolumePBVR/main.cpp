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
#include <kvs/PointImporter>
#include <kvs/PolygonImporter>

#include <kvs/Time>
#include <kvs/Timer>
#include <kvs/Bmp>
#include <kvs/Camera>
#include <kvs/RGBFormulae>

#include <kvs/CellByCellMetropolisSampling>
#include "PolygonToParticle.h"

#include "NullObject.h"
#include "StochasticRenderer.h"
#include "StochasticPointRenderer.h"

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

        add_option( "point", "[string] kvs::PointObject file path. ( optional )", 1, false );
        add_option( "polygon1", "[string] kvs::PolygonObject file path. ( optional )", 1, false );
        add_option( "polygon2", "[string] kvs::PolygonObject file path. ( optional )", 1, false );
        add_option( "np", "[size_t] Number of particles. ( default : 100000 )", 1, false );

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
                std::cout << r->timer().msec() << std::endl; break;
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
    screen.setTitle( "PBVRRenderer" );

    KeyPressEvent key_press_event;
    screen.addKeyPressEvent( &key_press_event );

    screen.show();

    const size_t repeat_level = arg.hasOption( "r" ) ? arg.optionValue<size_t>( "r" ) : 1;
    const size_t subpixel_level = static_cast<size_t>( sqrtf( float( repeat_level ) ) );
    const float sampling_step = arg.hasOption( "e" ) ? arg.optionValue<float>( "e" ) : 1.0f;

    kvs::glew::StochasticRenderer* renderer = new kvs::glew::StochasticRenderer( repeat_level );
    renderer->enableLODControl();

    kvs::NullObject* null = new kvs::NullObject();

    if ( arg.hasOption( "volume1" ) )
    {
        const std::string filename = arg.optionValue<std::string>( "volume1" );
        kvs::UnstructuredVolumeObject* volume = new kvs::UnstructuredVolumeImporter( filename );
        kvs::TransferFunction tfunc;

        kvs::Timer timer; timer.start();
        kvs::PointObject* point = new kvs::CellByCellMetropolisSampling( volume, subpixel_level, sampling_step, tfunc );
        timer.stop();
        std::cout << "Generated point is ..." << std::endl;
        std::cout << *point << std::endl;
        std::cout << "Generation time of volume1 is " << timer.msec() << " [msec]" << std::endl;
        delete volume;
        null->update( point );

        kvs::glew::StochasticPointRenderer* point_renderer = new kvs::glew::StochasticPointRenderer( point, repeat_level );
        if ( arg.hasOption( "DisableShading" ) ) point_renderer->disableShading();

        renderer->registerRenderer( point_renderer );
    }

    if ( arg.hasOption( "volume2" ) )
    {
        const std::string filename = arg.optionValue<std::string>( "volume2" );
        kvs::UnstructuredVolumeObject* volume = new kvs::UnstructuredVolumeImporter( filename );
        kvs::TransferFunction tfunc;
        tfunc.setColorMap( kvs::RGBFormulae::PM3D( 256 ) );

        kvs::Timer timer; timer.start();
        kvs::PointObject* point = new kvs::CellByCellMetropolisSampling( volume, subpixel_level, sampling_step, tfunc );
        timer.stop();
        std::cout << "Generated point is ..." << std::endl;
        std::cout << *point << std::endl;
        std::cout << "Generation time of volume2 is " << timer.msec() << " [msec]" << std::endl;
        delete volume;
        null->update( point );

        kvs::glew::StochasticPointRenderer* point_renderer = new kvs::glew::StochasticPointRenderer( point, repeat_level );
        if ( arg.hasOption( "DisableShading" ) ) point_renderer->disableShading();

        renderer->registerRenderer( point_renderer );
    }

    if ( arg.hasOption( "point" ) )
    {
        const std::string filename = arg.optionValue<std::string>( "point" );
        kvs::PointObject* point = new kvs::PointImporter( filename );
        null->update( point );

        kvs::glew::StochasticPointRenderer* point_renderer = new kvs::glew::StochasticPointRenderer( point, repeat_level );

        renderer->registerRenderer( point_renderer );
    }

    if ( arg.hasOption( "polygon1" ) )
    {
        const std::string filename = arg.optionValue<std::string>( "polygon1" );
        const size_t np = arg.hasOption( "np" ) ? arg.optionValue<size_t>( "np" ) : 100000;
        kvs::PolygonObject* polygon = new kvs::PolygonImporter( filename );

        kvs::Timer timer; timer.start();
        kvs::PointObject* point = new kvs::PolygonToParticle( polygon, np );
        timer.stop();
        std::cout << "Generated point is ..." << std::endl;
        std::cout << *point << std::endl;
        std::cout << "Generation time of polygon is " << timer.msec() << " [msec]" << std::endl;
        delete polygon;
        null->update( point );

        kvs::glew::StochasticPointRenderer* point_renderer = new kvs::glew::StochasticPointRenderer( point, repeat_level );

        renderer->registerRenderer( point_renderer );
    }

    if ( arg.hasOption( "polygon2" ) )
    {
        const std::string filename = arg.optionValue<std::string>( "polygon2" );
        const size_t np = arg.hasOption( "np" ) ? arg.optionValue<size_t>( "np" ) : 100000;
        kvs::PolygonObject* polygon = new kvs::PolygonImporter( filename );

        kvs::Timer timer; timer.start();
        kvs::PointObject* point = new kvs::PolygonToParticle( polygon, np );
        timer.stop();
        std::cout << "Generated point is ..." << std::endl;
        std::cout << *point << std::endl;
        std::cout << "Generation time of polygon is " << timer.msec() << " [msec]" << std::endl;
        delete polygon;
        null->update( point );

        kvs::glew::StochasticPointRenderer* point_renderer = new kvs::glew::StochasticPointRenderer( point, repeat_level );

        renderer->registerRenderer( point_renderer );
    }

    screen.registerObject( null, renderer );

    return( app.run() );
}
