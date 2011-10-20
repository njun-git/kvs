//
//  Context.cpp
//  
//
//  Created by Jun Nishimura on 10/14/11.
//  Copyright (c) 2011 Jun Nishimura. All rights reserved.
//

#include "Context.h"

#ifdef KVS_PLATFORM_MACOSX
#include <OpenGL/OpenGL.h>
#endif

#include <kvs/Message>
#include "ErrorString.h"

namespace kvs
{

namespace cl
{

Context::Context( void ) :
    m_is_created( false )
{
}

Context::Context( const kvs::cl::Device& device ) :
    m_is_created( false )
{
    this->create( device );
}

Context::~Context( void )
{
    this->release();
}

const bool Context::isCreated( void ) const
{
    return( m_is_created );
}

const CLContext& Context::context( void ) const
{
    return( m_context );
}

void Context::create( const kvs::cl::Device& device, const bool use_gl_context )
{
    if ( m_is_created )
    {
        kvsMessageError( "Context is already created." );
        return;
    }

#ifdef KVS_PLATFORM_MACOSX
    cl_context_properties* properties = NULL;
    if ( use_gl_context )
    {
        properties = new cl_context_properties[3];
        properties[0] = CL_CONTEXT_PROPERTY_USE_CGL_SHAREGROUP_APPLE;
        properties[1] = (cl_context_properties)CGLGetShareGroup(CGLGetCurrentContext());
        properties[2] = 0;
    }
#else
    cl_context_properties* properties = NULL;
#endif

    cl_int result = CL_SUCCESS;
    m_context = clCreateContext(
        properties, device.ndevices(), device.devices(), NULL, NULL, &result );
    if ( result != CL_SUCCESS )
    {
        kvsMessageError( "OpenCL; %s.", kvs::cl::ErrorString( result ) );
        return;
    }

    if ( properties ) delete [] properties;

    m_is_created = true;
}

void Context::release( void )
{
    if ( m_is_created )
    {
        clReleaseContext( m_context );
        m_is_created = false;
    }
}

}

}
