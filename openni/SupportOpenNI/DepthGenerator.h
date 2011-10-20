//
//  DepthGenerator.h
//  
//
//  Created by Jun Nishimura on 10/18/11.
//  Copyright (c) 2011 Jun Nishimura. All rights reserved.
//

#ifndef KVS__OPENNI__DEPTH_GENERATOR_H_INCLUDE
#define KVS__OPENNI__DEPTH_GENERATOR_H_INCLUDE

#include <kvs/ClassName>
#include <kvs/Vector3>
#include <kvs/ValueArray>

#include "GeneratorBase.h"
#include "ImageGenerator.h"

namespace kvs
{

namespace ni
{

class DepthGenerator : public kvs::ni::GeneratorBase
{

    // Class name.
    kvsClassName( kvs::ni::DepthGenerator );

    typedef kvs::ni::GeneratorBase BaseClass;

protected:

    size_t              m_width;
    size_t              m_height;
    unsigned long       m_zpd;
    double              m_zpps;
    xn::DepthGenerator  m_generator;

public:

    DepthGenerator( void );

    virtual ~DepthGenerator( void );

public:

    const bool create( kvs::ni::Context& context );

    const bool adjustDepthScale( kvs::ni::ImageGenerator& image );

    const kvs::Vector3f realPosition( const kvs::Vector3f& pos );

    const kvs::Vector3f projectPosition( const kvs::Vector3f& pos );

public:

    xn::DepthGenerator& generator( void );

    const size_t width( void ) const;

    const size_t height( void ) const;

    const size_t npixels( void ) const;

    const size_t bytePerPixel( void ) const;

    const unsigned short maxDepth( void ) const;

    const unsigned long zeroPlaneDistance( void ) const;

    const double zeroPlanePixelSize( void ) const;

    const kvs::ValueArray<unsigned short> depth16( void ) const;

    const unsigned short* pointer( void ) const;

};

}

}

#endif
