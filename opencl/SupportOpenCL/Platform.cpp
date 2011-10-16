//
//  Platform.cpp
//  
//
//  Created by Jun Nishimura on 10/14/11.
//  Copyright (c) 2011 Jun Nishimura. All rights reserved.
//

#include "Platform.h"

#include <kvs/Assert>
#include <kvs/Message>
#include "ErrorString.h"

namespace kvs
{

namespace cl
{

Platform::Platform( void ) :
    m_nplatforms( 0 ),
    m_platforms( NULL )
{
    this->initialize();
}

Platform::~Platform( void )
{
    this->clear();
}

void Platform::initialize( void )
{
    cl_int result = clGetPlatformIDs( 0, NULL, &m_nplatforms );
    if ( result != CL_SUCCESS )
    {
        kvsMessageError( "OpenCL; %s.", kvs::cl::ErrorString( result ) );
        return;
    }

    if ( m_nplatforms > 0 )
    {
        m_platforms = new CLPlatform[ m_nplatforms ];
        result = clGetPlatformIDs( m_nplatforms, m_platforms, NULL );
        if ( result != CL_SUCCESS )
        {
            kvsMessageError( "OpenCL; %s.", kvs::cl::ErrorString( result ) );
            this->clear();
        }
    }
}

void Platform::clear( void )
{
    if ( m_platforms )
    {
        delete [] m_platforms;
        m_platforms = NULL;
    }

    m_nplatforms = 0;
}

const size_t Platform::nplatforms( void ) const
{
    return( m_nplatforms );
}

const CLPlatform& Platform::platform( const size_t index ) const
{
    KVS_ASSERT( index < static_cast<size_t>( m_nplatforms ) );

    return( m_platforms[index] );
}

const std::string Platform::profile( const size_t index ) const
{
    return( this->get_platform_info( index, CL_PLATFORM_PROFILE ) );
}

const std::string Platform::version( const size_t index ) const
{
    return( this->get_platform_info( index, CL_PLATFORM_VERSION ) );
}

const std::string Platform::name( const size_t index ) const
{
    return( this->get_platform_info( index, CL_PLATFORM_NAME ) );
}

const std::string Platform::vendor( const size_t index ) const
{
    return( this->get_platform_info( index, CL_PLATFORM_VENDOR ) );
}

const std::string Platform::extensions( const size_t index ) const
{
    return( this->get_platform_info( index, CL_PLATFORM_EXTENSIONS ) );
}

const std::string Platform::get_platform_info( const size_t index, const cl_platform_info info ) const
{
    KVS_ASSERT( index < static_cast<size_t>( m_nplatforms ) );

    char buf[1024];
    cl_int result = clGetPlatformInfo( m_platforms[index], info, sizeof(buf), buf, NULL );
    if ( result != CL_SUCCESS )
    {
        kvsMessageError( "OpenCL; %s.", kvs::cl::ErrorString( result ) );
    }
    return( std::string( buf ) );
}

}

}
