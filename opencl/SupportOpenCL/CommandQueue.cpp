//
//  CommandQueue.cpp
//  
//
//  Created by Jun Nishimura on 10/15/11.
//  Copyright (c) 2011 Jun Nishimura. All rights reserved.
//

#include "CommandQueue.h"

#include <kvs/Message>
#include "ErrorString.h"

namespace kvs
{

namespace cl
{

CommandQueue::CommandQueue( void ) :
    m_is_created( false )
{
}

CommandQueue::CommandQueue(
    const kvs::cl::Context& context,
    const kvs::cl::CLDevice& device ) :
    m_is_created( false )
{
    this->create( context, device );
}

CommandQueue::~CommandQueue( void )
{
    this->release();
}

const bool CommandQueue::isCreated( void ) const
{
    return( m_is_created );
}

const CLQueue& CommandQueue::queue( void ) const
{
    return( m_queue );
}

void CommandQueue::create(
    const kvs::cl::Context& context,
    const kvs::cl::CLDevice& device )
{
    if ( m_is_created )
    {
        kvsMessageError( "Command queue is already created." );
        return;
    }

    cl_int result = CL_SUCCESS;
    m_queue = clCreateCommandQueue(
        context.context(), device, 0, &result );
    if ( result != CL_SUCCESS )
    {
        kvsMessageError( "OpenCL; %s.", kvs::cl::ErrorString( result ) );
        return;
    }

    m_is_created = true;
}

void CommandQueue::finish( void )
{
    const cl_int result = clFinish( m_queue );
    if ( result != CL_SUCCESS )
    {
        kvsMessageError( "OpenCL; %s.", kvs::cl::ErrorString( result ) );
    }
}

void CommandQueue::flush( void )
{
    const cl_int result = clFlush( m_queue );
    if ( result != CL_SUCCESS )
    {
        kvsMessageError( "OpenCL; %s.", kvs::cl::ErrorString( result ) );
    }
}

void CommandQueue::release( void )
{
    if ( m_is_created )
    {
        clReleaseCommandQueue( m_queue );
        m_is_created = false;
    }
}

}

}
