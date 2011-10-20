//
//  DepthGenerator.cpp
//  
//
//  Created by Jun Nishimura on 10/18/11.
//  Copyright (c) 2011 Jun Nishimura. All rights reserved.
//

#include "DepthGenerator.h"

#include <kvs/Message>
#include "Context.h"

namespace kvs
{

namespace ni
{

DepthGenerator::DepthGenerator( void ) :
    kvs::ni::GeneratorBase()
{
    m_width = XN_VGA_X_RES;
    m_height = XN_VGA_Y_RES;
    m_zpd = 0;
    m_zpps = 0.0;
}

DepthGenerator::~DepthGenerator( void )
{
}

const bool DepthGenerator::create( kvs::ni::Context& context )
{
    if ( m_is_created )
    {
        kvsMessageError( "DepthGenerator is already created." );
        return( false );
    }

    {
        XnStatus status = m_generator.Create( context.context() );
        if ( status != XN_STATUS_OK )
        {
            kvsMessageError( "%s error: <%s>.",
                xnGetStatusName( status ),
                xnGetStatusString( status ) );
            return( false );
        }
    }
    {
        XnUInt64 zpd;
        XnStatus status = m_generator.GetIntProperty( "ZPD", zpd );
        if ( status != XN_STATUS_OK )
        {
            kvsMessageError( "%s error: <%s>.",
                xnGetStatusName( status ),
                xnGetStatusString( status ) );
            return( false );
        }
        m_zpd = static_cast<unsigned long>( zpd );
    }
    {
        XnDouble zpps;
        XnStatus status = m_generator.GetRealProperty( "ZPPS", zpps );
        if ( status != XN_STATUS_OK )
        {
            kvsMessageError( "%s error: <%s>.",
                xnGetStatusName( status ),
                xnGetStatusString( status ) );
            return( false );
        }
        m_zpps = static_cast<double>( zpps );
    }

    m_is_created = true;
    return( true );
}

const bool DepthGenerator::adjustDepthScale( kvs::ni::ImageGenerator& image )
{
    if ( !m_is_created )
    {
        kvsMessageError( "DepthGenerator is not created yet." );
        return( false );
    }

    XnStatus status = m_generator.GetAlternativeViewPointCap().SetViewPoint( image.generator() );
    if ( status != XN_STATUS_OK )
    {
        kvsMessageError( "%s error: <%s>.",
            xnGetStatusName( status ),
            xnGetStatusString( status ) );
        return( false );
    }

    return( true );
}

const kvs::Vector3f DepthGenerator::realPosition( const kvs::Vector3f& pos )
{
    if ( !m_is_created )
    {
        kvsMessageError( "DepthGenerator is not created yet." );
        return( pos );
    }

    XnPoint3D proj;
    XnPoint3D real;
    proj.X = pos.x();
    proj.Y = pos.y();
    proj.Z = pos.z();

    XnStatus status = m_generator.ConvertProjectiveToRealWorld( 1, &proj, &real );
    if ( status != XN_STATUS_OK )
    {
        kvsMessageError( "%s error: <%s>.",
            xnGetStatusName( status ),
            xnGetStatusString( status ) );
        return( pos );
    }

    return( kvs::Vector3f( real.X, real.Y, real.Z ) );
}

const kvs::Vector3f DepthGenerator::projectPosition( const kvs::Vector3f& pos )
{
    if ( !m_is_created )
    {
        kvsMessageError( "DepthGenerator is not created yet." );
        return( pos );
    }

    XnPoint3D real;
    XnPoint3D proj;
    real.X = pos.x();
    real.Y = pos.y();
    real.Z = pos.z();

    XnStatus status = m_generator.ConvertRealWorldToProjective( 1, &real, &proj );
    if ( status != XN_STATUS_OK )
    {
        kvsMessageError( "%s error: <%s>.",
            xnGetStatusName( status ),
            xnGetStatusString( status ) );
        return( pos );
    }

    return( kvs::Vector3f( proj.X, proj.Y, proj.Z ) );
}

xn::DepthGenerator& DepthGenerator::generator( void )
{
    return( m_generator );
}

const size_t DepthGenerator::width( void ) const
{
    return( m_width );
}

const size_t DepthGenerator::height( void ) const
{
    return( m_height );
}

const size_t DepthGenerator::npixels( void ) const
{
    return( m_width * m_height );
}

const size_t DepthGenerator::bytePerPixel( void ) const
{
    return( sizeof(unsigned short) );
}

const unsigned short DepthGenerator::maxDepth( void ) const
{
    return( static_cast<unsigned short>( m_generator.GetDeviceMaxDepth() ) );
}

const unsigned long DepthGenerator::zeroPlaneDistance( void ) const
{
    return( m_zpd );
}

const double DepthGenerator::zeroPlanePixelSize( void ) const
{
    return( m_zpps );
}

const kvs::ValueArray<unsigned short> DepthGenerator::depth16( void ) const
{
    return( kvs::ValueArray<unsigned short>( this->pointer(), this->npixels() ) );
}

const unsigned short* DepthGenerator::pointer( void ) const
{
    const unsigned short* p_depth_map
        = static_cast<const unsigned short*>( m_generator.GetDepthMap() );
    return( p_depth_map );
}

}

}
