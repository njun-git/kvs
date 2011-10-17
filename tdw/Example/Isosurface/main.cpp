/*
 *  main.cpp
 *  
 *
 *  Created by njun on 11/07/09.
 *  Copyright 2011 Jun Nishimura. All rights reserved.
 *
 */

#include <kvs/tdw/Application>
#include <kvs/tdw/Screen>

#include <kvs/CommandLine>
#include <kvs/StructuredVolumeImporter>
#include <kvs/Isosurface>

kvs::StructuredVolumeObject* volume = NULL;
kvs::TransferFunction tfunc( 256 );
float iso_level = 0.5f;

class Argument : public kvs::CommandLine
{

public:

    Argument( int argc, char** argv ) :
        CommandLine( argc, argv )
    {
        add_help_option();
        add_option( "level", "Iso-level [ 0 ~ 1 ] ( default : 0.5 )", 1, false );
        add_value( "kvs::StructuredVolumeObject kvsml file path.", true );

        if ( !this->parse() ) exit( EXIT_FAILURE );
    }

};

kvs::PolygonObject* newIsosurface( void )
{
    if ( !volume ) return( NULL );

    const double iso_value = iso_level * ( volume->minValue() + volume->maxValue() );

    kvs::PolygonObject* object = new kvs::Isosurface(
        volume,
        iso_value,
        kvs::PolygonObject::VertexNormal,
        false,
        tfunc );
    object->setName( "Isosurface" );

    return( object );
}

class KeyPressEvent : public kvs::KeyPressEventListener
{
    void update( kvs::KeyEvent* event )
    {
        if ( kvs::tdw::Application::IsMaster() ) return;
        switch ( event->key() )
        {
            case kvs::Key::Up:
            {
                iso_level += 0.01f;
                if ( iso_level > 1.0f ) iso_level = 1.0f;
                kvs::PolygonObject* object = newIsosurface();
                screen()->objectManager()->change( "Isosurface", object );
                break;
            }
            case kvs::Key::Down:
            {
                iso_level -= 0.01f;
                if ( iso_level < 0.0f ) iso_level = 0.0f;
                kvs::PolygonObject* object = newIsosurface();
                screen()->objectManager()->change( "Isosurface", object );
                break;
            }

            default: break;
        }
    }
};

int main( int argc, char** argv )
{
    Argument arg( argc, argv );
    kvs::tdw::Application app( argc, argv );
    kvs::tdw::Screen screen( &app );

    KeyPressEvent key_press_event;
    screen.addKeyPressEvent( &key_press_event );

    screen.show();

    if ( kvs::tdw::Application::IsRenderer() )
    {
        const std::string filename = arg.value<std::string>( );
        volume = new kvs::StructuredVolumeImporter( filename );

        iso_level = arg.hasOption( "level" ) ? arg.optionValue<float>( "level" ) : 0.5f;

        kvs::PolygonObject* object = newIsosurface();

        screen.registerObject( object );
    }

    return( app.run() );
}
