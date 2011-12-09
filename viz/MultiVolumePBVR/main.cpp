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

#include <kvs/Time>
#include <kvs/Bmp>
#include <kvs/Camera>
#include <kvs/RGBFormulae>

#include <kvs/CellByCellMetropolisSampling>
#include <kvs/glew/ParticleVolumeRenderer>

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
    kvs::PointObject* point = new kvs::PointObject();

    if ( arg.hasOption( "volume1" ) )
    {
        const std::string filename = arg.optionValue<std::string>( "volume1" );
        kvs::UnstructuredVolumeObject* volume = new kvs::UnstructuredVolumeImporter( filename );
        kvs::TransferFunction tfunc;
        kvs::PointObject* p = new kvs::CellByCellMetropolisSampling( volume, subpixel_level, sampling_step, tfunc );
        delete volume;
        point->add( *p );
        point->updateMinMaxCoords();
        delete p;
    }

    if ( arg.hasOption( "volume2" ) )
    {
        const std::string filename = arg.optionValue<std::string>( "volume2" );
        kvs::UnstructuredVolumeObject* volume = new kvs::UnstructuredVolumeImporter( filename );
        kvs::TransferFunction tfunc;
        tfunc.setColorMap( kvs::RGBFormulae::PM3D( 256 ) );
        kvs::PointObject* p = new kvs::CellByCellMetropolisSampling( volume, subpixel_level, sampling_step, tfunc );
        delete volume;
        point->add( *p );
        delete p;
    }

    kvs::glew::ParticleVolumeRenderer* renderer = new kvs::glew::ParticleVolumeRenderer( point, 1, subpixel_level * repeat_level );
    renderer->enableLODControl();
    renderer->disableShading();
    screen.registerObject( point, renderer );

    return( app.run() );
}
