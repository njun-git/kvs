//
//  TextureObject.cpp
//  
//
//  Created by Jun Nishimura on 10/27/11.
//  Copyright (c) 2011 Jun Nishimura. All rights reserved.
//

#include "TextureObject.h"

#include <kvs/ColorMap>

namespace kvs
{

namespace ni
{

TextureObject::TextureObject( void ) :
    kvs::ObjectBase()
{
    m_width = XN_VGA_X_RES;
    m_height = XN_VGA_Y_RES;
    m_data.allocate( 4 * m_width * m_height );

    const kvs::Vector3f min_coord(
        - 0.001f * m_width / 2.0f, - 0.001f * m_height / 2.0f, - 4.0f );
    const kvs::Vector3f max_coord(
          0.001f * m_width / 2.0f,   0.001f * m_height / 2.0f, - 2.0f );

    BaseClass::setMinMaxObjectCoords( min_coord, max_coord );
    BaseClass::setMinMaxExternalCoords( min_coord, max_coord );
}

TextureObject::~TextureObject( void )
{
}

const kvs::ObjectBase::ObjectType TextureObject::objectType( void ) const
{
    return( kvs::ObjectBase::Image );
}

const size_t TextureObject::width( void ) const
{
    return( m_width );
}

const size_t TextureObject::height( void ) const
{
    return( m_height );
}

const kvs::ValueArray<unsigned char>& TextureObject::data( void ) const
{
    return( m_data );
}

void TextureObject::update( kvs::ni::ImageGenerator& image )
{
    kvs::ni::ImageGenerator::PixelFormat format = image.pixelFormat();
    const unsigned char* p_data = image.pointer();
    if ( format == kvs::ni::ImageGenerator::RGB24 )
    {
        for ( size_t y = 0; y < m_height; y++ )
        {
            for ( size_t x = 0; x < m_width; x++ )
            {
                const size_t index = x + y * m_width;
                m_data[ index * 4     ] = p_data[ index * 3     ];
                m_data[ index * 4 + 1 ] = p_data[ index * 3 + 1 ];
                m_data[ index * 4 + 2 ] = p_data[ index * 3 + 2 ];
                m_data[ index * 4 + 3 ] = 255;
            }
        }
    }
    else if ( format == kvs::ni::ImageGenerator::GrayScale8 )
    {
        for ( size_t y = 0; y < m_height; y++ )
        {
            for ( size_t x = 0; x < m_width; x++ )
            {
                const size_t index = x + y * m_width;
                m_data[ index * 4     ] = p_data[ index ];
                m_data[ index * 4 + 1 ] = p_data[ index ];
                m_data[ index * 4 + 2 ] = p_data[ index ];
                m_data[ index * 4 + 3 ] = 255;
            }
        }
    }
    else
    {
        kvsMessageError( "Not support pixel format of GrayScale16." );
    }
}

void TextureObject::update( kvs::ni::DepthGenerator& depth )
{
    const unsigned short* p_data = depth.pointer();
    const unsigned short max_depth = depth.maxDepth();
    for ( size_t y = 0; y < m_height; y++ )
    {
        for ( size_t x = 0; x < m_width; x++ )
        {
            const size_t index = x + y * m_width;
            const float depth = static_cast<float>( max_depth - p_data[ index ] ) / static_cast<float>( max_depth + 1 );
            unsigned char c_depth = static_cast<unsigned char>( depth * 255.0f );
            m_data[ index * 4     ] = c_depth;
            m_data[ index * 4 + 1 ] = c_depth;
            m_data[ index * 4 + 2 ] = c_depth;
            m_data[ index * 4 + 3 ] = 255;
        }
    }
}

void TextureObject::update( kvs::ni::UserGenerator& user )
{
    kvs::ColorMap color_map( 5 );
    color_map.create();

    kvs::ValueArray<unsigned int> users = user.users();
    m_data.fill( 0x00 );

    for ( size_t i = 0; i < users.size(); i++ )
    {
        const unsigned short* p_user = user.pointer( users[i] );
        const size_t color_index = users[i] % 5;
        kvs::RGBColor color = color_map[ color_index ];
        for ( size_t y = 0; y < m_height; y++ )
        {
            for ( size_t x = 0; x < m_width; x++ )
            {
                const size_t index = x + y * m_width;
                if ( p_user[ index ] == users[i] )
                {
                    m_data[ index * 4     ] = color.r();
                    m_data[ index * 4 + 1 ] = color.g();
                    m_data[ index * 4 + 2 ] = color.b();
                    m_data[ index * 4 + 3 ] = 255;
                }
            }
        }
    }
}

void TextureObject::update(
    kvs::ni::DepthGenerator& depth,
    kvs::ni::UserGenerator& user )
{
    const unsigned short* p_depth = depth.pointer();

    kvs::ColorMap color_map( 5 );
    color_map.create();

    kvs::ValueArray<unsigned int> users = user.users();
    m_data.fill( 0x00 );

    for ( size_t i = 0; i < users.size(); i++ )
    {
        const unsigned short* p_user = user.pointer( users[i] );
        kvs::Vector3f com = user.centerOfMass( users[i] );
        const unsigned short max_depth = static_cast<unsigned short>( 2 * com.z() );
        const size_t color_index = users[i] % 5;
        kvs::RGBColor color = color_map[ color_index ];
        for ( size_t y = 0; y < m_height; y++ )
        {
            for ( size_t x = 0; x < m_width; x++ )
            {
                const size_t index = x + y * m_width;
                if ( p_user[ index ] == users[i] )
                {
                    const float depth = static_cast<float>( max_depth - p_depth[ index ] ) / static_cast<float>( max_depth );
                    m_data[ index * 4     ] = static_cast<unsigned char>( color.r() * depth );
                    m_data[ index * 4 + 1 ] = static_cast<unsigned char>( color.g() * depth );
                    m_data[ index * 4 + 2 ] = static_cast<unsigned char>( color.b() * depth );
                    m_data[ index * 4 + 3 ] = 255;
                }
            }
        }
    }
}

}

}
