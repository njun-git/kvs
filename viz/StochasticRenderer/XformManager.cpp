
#include "XformManager.h"

namespace kvs
{

XformManager::XformManager( kvs::ScreenBase* screen ) :
    m_ref_screen( screen )
{
}

XformManager::~XformManager( void )
{
    m_ref_screen = NULL;
}

/*
    float[3] translation
    float[3] scaling
    float[9] rotation
*/

const bool XformManager::applyXform( const std::string& filename )
{
    FILE* ifs = fopen( filename.c_str(), "rb" );
    if ( !ifs )
    {
        kvsMessageError( "Cannot open file <%s>.", filename.c_str() );
        return( false );
    }

    float buffer[15];

    fread( buffer, sizeof(float), 15, ifs );

    fclose( ifs );

    kvs::Vector3f translation( buffer );
    kvs::Vector3f scaling( buffer + 3 );
    kvs::Matrix33f rotation( buffer + 6 );

    kvs::Xform xform( translation, scaling, rotation );

    m_ref_screen->objectManager()->resetXform();
    m_ref_screen->objectManager()->translate( xform.translation() );
    m_ref_screen->objectManager()->scale( xform.scaling() );
    m_ref_screen->objectManager()->rotate( xform.rotation() );

    return( true );
}

const bool XformManager::saveXform( const std::string& filename )
{
    const kvs::Xform xform = m_ref_screen->objectManager()->xform();

    FILE* ofs = fopen( filename.c_str(), "wb" );
    if ( !ofs )
    {
        kvsMessageError( "Cannot open file <%s>.", filename.c_str() );
        return( false );
    }

    float buffer[15];
    size_t index = 0;

    kvs::Vector3f translation( xform.translation() );
    for ( size_t i = 0; i < 3; i++ )
    {
        buffer[index] = translation[i];
        index++;
    }

    kvs::Vector3f scaling( xform.scaling() );
    for ( size_t i = 0; i < 3; i++ )
    {
        buffer[index] = scaling[i];
        index++;
    }

    kvs::Matrix33f rotation( xform.rotation() );
    for ( size_t i = 0; i < 3; i++ )
    {
        for ( size_t j = 0; j < 3; j++ )
        {
            buffer[index] = rotation[i][j];
            index++;
        }
    }

    fwrite( buffer, sizeof(float), 15, ofs );

    fclose( ofs );

    return( true );
}

}

