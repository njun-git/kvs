/*
 *  PaintImageObject.cpp
 *  
 *
 *  Created by njun on 11/10/10.
 *  Copyright 2011 Jun Nishimura. All rights reserved.
 *
 */

#include "PaintImageObject.h"

namespace kvs
{

PaintImageObject::PaintImageObject( void ) :
    kvs::ObjectBase(),
    m_is_initialize( false )
{
    this->setSize( 0, 0 );
}

PaintImageObject::PaintImageObject( const size_t width, const size_t height ) :
    kvs::ObjectBase(),
    m_is_initialize( false )
{
    this->setSize( width, height );
}

PaintImageObject::PaintImageObject( const std::string& filename ) :
    kvs::ObjectBase(),
    m_is_initialize( false )
{
    if ( !this->read( filename ) )
    {
        this->setSize( 512, 512 );
    }
}

PaintImageObject::~PaintImageObject( void )
{
}

const kvs::ObjectBase::ObjectType PaintImageObject::objectType( void ) const
{
    return( kvs::ObjectBase::Image );
}

const size_t PaintImageObject::width( void ) const
{
    return( m_width );
}

const size_t PaintImageObject::height( void ) const
{
    return( m_height );
}

const kvs::ValueArray<float>& PaintImageObject::data( void ) const
{
    return( m_data );
}

kvs::ValueArray<float>& PaintImageObject::data( void )
{
    return( m_data );
}

float& PaintImageObject::value( const size_t x, const size_t y )
{
    KVS_ASSERT( x < m_width );
    KVS_ASSERT( y < m_height );

    return( m_data[ y * m_width + x ] );
}

const bool PaintImageObject::isInitilize( void ) const
{
    return( m_is_initialize );
}

kvs::Texture2D& PaintImageObject::texture( void )
{
    return( m_texture );
}

void PaintImageObject::setSize( const size_t width, const size_t height )
{
    m_width = width;
    m_height = height;
    m_data.allocate( width * height );
    m_data.fill( 0x00 );
}

const bool PaintImageObject::read( const std::string& filename )
{
    kvs::ImageObject* image = new kvs::ImageImporter( filename );
    if ( !image ) return( false );

    m_width = image->width();
    m_height = image->height();
    m_data.allocate( m_width * m_height );

    switch ( image->type() ) {
        case kvs::ImageObject::Gray8:
        {
            const unsigned char* data = image->data().pointer();
            for ( size_t i = 0; i < image->data().size(); i++ )
            {
                m_data[i] = static_cast<float>( data[i] / 255.0f );
            }
            break;
        }
        case kvs::ImageObject::Color24:
        {
            const unsigned char* data = image->data().pointer();
            for ( size_t i = 0; i < image->data().size() / 3; i++ )
            {
                const float r = static_cast<float>( data[ i * 3     ] / 255.0f );
                const float g = static_cast<float>( data[ i * 3 + 1 ] / 255.0f );
                const float b = static_cast<float>( data[ i * 3 + 2 ] / 255.0f );
                m_data[i] = ( r + g + b ) / 3.0f;
            }
            break;
        }
        case kvs::ImageObject::Color32:
        {
            const unsigned char* data = image->data().pointer();
            for ( size_t i = 0; i < image->data().size() / 4; i++ )
            {
                const float r = static_cast<float>( data[ i * 4     ] / 255.0f );
                const float g = static_cast<float>( data[ i * 4 + 1 ] / 255.0f );
                const float b = static_cast<float>( data[ i * 4 + 2 ] / 255.0f );
                m_data[i] = ( r + g + b ) / 3.0f;
            }
            break;
        }
        default:
        {
            kvsMessageError( "Unsupported image type." );
            delete image;
            return( false );
            break;
        }
    }

    delete image;

    return( true );
}

const bool PaintImageObject::write( const std::string& filename )
{
    kvs::ValueArray<unsigned char> data( 3 * m_width * m_height );
    for ( size_t i = 0; i < data.size() / 3; i++ )
    {
        const unsigned char value = static_cast<unsigned char>( m_data[i] * 255.0f );
        data[ i * 3     ] = value;
        data[ i * 3 + 1 ] = value;
        data[ i * 3 + 2 ] = value;
    }

    kvs::Bmp bmp( m_width, m_height, data );

    return( bmp.write( filename ) );
}

void PaintImageObject::create( void )
{
    m_texture.release();

    m_texture.setWrapS( GL_CLAMP_TO_EDGE );
    m_texture.setWrapT( GL_CLAMP_TO_EDGE );
    m_texture.setMagFilter( GL_LINEAR );
    m_texture.setMinFilter( GL_LINEAR );

    m_texture.setPixelFormat( GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT, GL_FLOAT );
    m_texture.create( m_width, m_height );
    {
        GLenum error = glGetError();
        if ( error != GL_NO_ERROR )
        {
            kvsMessageError( "depth buffer allocation failed: %s.", gluErrorString(error));
            exit( EXIT_FAILURE );
        }
    }
    m_texture.unbind();

    m_is_initialize = true;
}

void PaintImageObject::download( void )
{
    m_texture.download( m_width, m_height, m_data.pointer() );
}

void PaintImageObject::bind( void )
{
    m_texture.bind();
}

void PaintImageObject::unbind( void )
{
    m_texture.unbind();
}

}
