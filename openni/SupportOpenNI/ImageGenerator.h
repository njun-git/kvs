//
//  ImageGenerator.h
//  
//
//  Created by Jun Nishimura on 10/18/11.
//  Copyright (c) 2011 Jun Nishimura. All rights reserved.
//

#ifndef KVS__OPENNI__IMAGE_GENERATOR_H_INCLUDE
#define KVS__OPENNI__IMAGE_GENERATOR_H_INCLUDE

#include <kvs/ClassName>
#include <kvs/ValueArray>

#include "GeneratorBase.h"

namespace kvs
{

namespace ni
{

class ImageGenerator : public kvs::ni::GeneratorBase
{

    // Class name.
    kvsClassName( kvs::ni::ImageGenerator );

    typedef kvs::ni::GeneratorBase BaseClass;

public:

    enum PixelFormat
    {
        RGB24       = XN_PIXEL_FORMAT_RGB24,
        GrayScale8  = XN_PIXEL_FORMAT_GRAYSCALE_8_BIT,
        GrayScale16 = XN_PIXEL_FORMAT_GRAYSCALE_16_BIT
    };

protected:

    size_t              m_width;
    size_t              m_height;
    PixelFormat         m_format;
    xn::ImageGenerator  m_generator;

public:

    ImageGenerator( void );

    virtual ~ImageGenerator( void );

public:

    const bool create( kvs::ni::Context& context );

public:

    xn::ImageGenerator& generator( void );

    const size_t width( void ) const;

    const size_t height( void ) const;

    const size_t npixels( void ) const;

    const size_t bytePerPixel( void ) const;

    const PixelFormat& pixelFormat( void ) const;

    const kvs::ValueArray<unsigned char> color24( void ) const;

    const kvs::ValueArray<unsigned char> gray8( void ) const;

    const kvs::ValueArray<unsigned short> gray16( void ) const;

    const unsigned char* pointer( void ) const;

public:

    const bool setPixelFormat( const PixelFormat& format );

};

}

}

#endif
