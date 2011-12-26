//
//  main.cpp
//  
//
//  Created by Jun Nishimura on 11/4/11.
//  Copyright (c) 2011 Jun Nishimura. All rights reserved.
//

#include <kvs/CommandLine>

#include <kvs/UnstructuredVolumeImporter>
#include <kvs/PolygonImporter>

#include <kvs/UnstructuredVolumeExporter>
#include <kvs/PolygonExporter>

class Argument : public kvs::CommandLine
{

public:

    Argument( int argc, char** argv ) :
        CommandLine( argc, argv )
    {
        add_help_option();

        add_option( "t", "[float] kvs::Vector3f. ( default : 1.0 1.0 1.0 )", 3, false );

        add_option( "volume", "[string] kvs::UnstructuredVolumeObject file path. ( optional )", 1, false );
        add_option( "polygon", "[string] kvs::PolygonObject file path. ( optional )", 1, false );
        add_option( "output", "[string] Output file path. ( default : output.kvsml )", 1, false );

        if( !this->parse() ) exit( EXIT_FAILURE );
    }

};

kvs::ValueArray<float> TranslatedCoordinates(
    const kvs::ValueArray<float>& coords,
    const kvs::Vector3f translation )
{
    const size_t nvertices = coords.size() / 3;
    kvs::ValueArray<float> t_coords( nvertices * 3 );

    for ( size_t i = 0; i < nvertices; i++ )
    {
        t_coords[ i * 3     ] = coords[ i * 3     ] + translation.x();
        t_coords[ i * 3 + 1 ] = coords[ i * 3 + 1 ] + translation.y();
        t_coords[ i * 3 + 2 ] = coords[ i * 3 + 2 ] + translation.z();
    }

    return( t_coords );
}

int main( int argc, char** argv )
{
    Argument arg( argc, argv );

    const std::string output_path = arg.hasOption( "output" ) ? arg.optionValue<std::string>( "output" ) : "output.kvsml";
    const kvs::Vector3f translation = arg.hasOption( "t" ) ?
        kvs::Vector3f( arg.optionValue<float>( "t", 0 ), arg.optionValue<float>( "t", 1 ), arg.optionValue<float>( "t", 2 ) ) :
        kvs::Vector3f( 1.0f, 1.0f, 1.0f );

    if ( arg.hasOption( "volume" ) )
    {
        const std::string filename = arg.optionValue<std::string>( "volume" );
        kvs::UnstructuredVolumeObject* volume = new kvs::UnstructuredVolumeImporter( filename );
        if ( !volume )
        {
            kvsMessageError( "Cannot import volume<%s>.", filename.c_str() );
            return( 1 );
        }

        volume->setCoords( TranslatedCoordinates( volume->coords(), translation ) );

        kvs::KVSMLObjectUnstructuredVolume* kvsml = new kvs::UnstructuredVolumeExporter<kvs::KVSMLObjectUnstructuredVolume>( volume );
        kvsml->setWritingDataType( kvs::KVSMLObjectUnstructuredVolume::ExternalBinary );
        if ( !kvsml->write( output_path ) )
        {
            kvsMessageError( "Cannot write polygon <%s>.", output_path.c_str() );
        }
        delete volume;
        delete kvsml;
    }
    else if ( arg.hasOption( "polygon" ) )
    {
        const std::string filename = arg.optionValue<std::string>( "polygon" );
        kvs::PolygonObject* polygon = new kvs::PolygonImporter( filename );
        if ( !polygon )
        {
            kvsMessageError( "Cannot import polygon<%s>.", filename.c_str() );
            return( 1 );
        }

        polygon->setCoords( TranslatedCoordinates( polygon->coords(), translation ) );

        kvs::KVSMLObjectPolygon* kvsml = new kvs::PolygonExporter<kvs::KVSMLObjectPolygon>( polygon );
        kvsml->setWritingDataType( kvs::KVSMLObjectPolygon::ExternalBinary );
        if ( !kvsml->write( output_path ) )
        {
            kvsMessageError( "Cannot write polygon <%s>.", output_path.c_str() );
        }
        delete polygon;
        delete kvsml;
    }

    return( 0 );
}
