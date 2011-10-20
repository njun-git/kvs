//
//  ImageGenerator.cpp
//  
//
//  Created by Jun Nishimura on 10/18/11.
//  Copyright (c) 2011 Jun Nishimura. All rights reserved.
//

#include "ImageGenerator.h"

#include <kvs/Message>
#include "Context.h"

namespace kvs
{

namespace ni
{

ImageGenerator::ImageGenerator( void ) :
    kvs::ni::GeneratorBase()
{
    m_width = XN_VGA_X_RES;
    m_height = XN_VGA_Y_RES;
    m_format = RGB24;
}

ImageGenerator::~ImageGenerator( void )
{
}

const bool ImageGenerator::create( kvs::ni::Context& context )
{
    if ( m_is_created )
    {
        kvsMessageError( "ImageGenerator is already created." );
        return( false );
    }

    XnStatus status = m_generator.Create( context.context() );
    if ( status != XN_STATUS_OK )
    {
        kvsMessageError( "%s error: <%s>.",
            xnGetStatusName( status ),
            xnGetStatusString( status ) );
        return( false );
    }

    m_is_created = true;
    return( true );
}

xn::ImageGenerator& ImageGenerator::generator( void )
{
    return( m_generator );
}

const size_t ImageGenerator::width( void ) const
{
    return( m_width );
}

const size_t ImageGenerator::height( void ) const
{
    return( m_height );
}

const size_t ImageGenerator::npixels( void ) const
{
    return( m_width * m_height );
}

const size_t ImageGenerator::bytePerPixel( void ) const
{
    switch ( m_format )
    {
        case GrayScale8:  return( sizeof(unsigned char) ); break;
        case GrayScale16: return( sizeof(unsigned short) ); break;
        case RGB24:       return( 3 * sizeof(unsigned char) ); break;

        default: break;
    }
    return( sizeof(unsigned char) );
}

const ImageGenerator::PixelFormat& ImageGenerator::pixelFormat( void ) const
{
    return( m_format );
}

const kvs::ValueArray<unsigned char> ImageGenerator::color24( void ) const
{
    return( kvs::ValueArray<unsigned char>( this->pointer(), 3 * this->npixels() ) );
}

const kvs::ValueArray<unsigned char> ImageGenerator::gray8( void ) const
{
    return( kvs::ValueArray<unsigned char>( this->pointer(), this->npixels() ) );
}

const kvs::ValueArray<unsigned short> ImageGenerator::gray16( void ) const
{
    return( kvs::ValueArray<unsigned short>( reinterpret_cast<const unsigned short*>( this->pointer() ), this->npixels() ) );
}

const unsigned char* ImageGenerator::pointer( void ) const
{
    const unsigned char* p_image_map
        = static_cast<const unsigned char*>( m_generator.GetImageMap() );
    return( p_image_map );
}

const bool ImageGenerator::setPixelFormat( const PixelFormat& format )
{
    if ( !m_is_created )
    {
        kvsMessageError( "ImageGenerator is not created yet." );
        return( false );
    }

    XnStatus status = m_generator.SetPixelFormat( static_cast<XnPixelFormat>( m_format ) );
    if ( status != XN_STATUS_OK )
    {
        kvsMessageError( "%s error: <%s>.",
            xnGetStatusName( status ),
            xnGetStatusString( status ) );
        return( false );
    }

    m_format = format;
    return( true );
}

}

}
