//
//  main.cpp
//  
//
//  Created by Jun Nishimura on 11/4/11.
//  Copyright (c) 2011 Jun Nishimura. All rights reserved.
//

#include <kvs/UnstructuredVolumeImporter>
#include <kvs/ExternalFaces>
#include <kvs/PolygonExporter>

int main( int argc, char** argv )
{
    if ( argc < 2 )
    {
        kvsMessageError( "Please input unstructured volume file path." );
        return( 1 );
    }

    const std::string filename( argv[1] );
    kvs::UnstructuredVolumeObject* volume = new kvs::UnstructuredVolumeImporter( filename );
    if ( !volume )
    {
        kvsMessageError( "Cannot import volume<%s>.", filename.c_str() );
        return( 1 );
    }

    kvs::PolygonObject* surface = new kvs::ExternalFaces( volume );
    if ( !surface )
    {
        kvsMessageError( "Cannot extract surface." );
        delete volume;
        return( 1 );
    }
    delete volume;

    kvs::KVSMLObjectPolygon* kvsml = new kvs::PolygonExporter<kvs::KVSMLObjectPolygon>( surface );
    kvsml->setWritingDataType( kvs::KVSMLObjectPolygon::ExternalBinary );
    std::string output_filename = "output.kvsml";
    if ( argc > 2 ) output_filename = std::string( argv[2] );
    if ( !kvsml->write( output_filename ) )
    {
        kvsMessageError( "Cannot write surface <%s>.", output_filename.c_str() );
    }
    delete surface;
    delete kvsml;

    return( 0 );
}
