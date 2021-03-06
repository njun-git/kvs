//
//  KernelObject.cpp
//  
//
//  Created by Jun Nishimura on 10/15/11.
//  Copyright (c) 2011 Jun Nishimura. All rights reserved.
//

#include "KernelObject.h"

#include <kvs/Message>
#include "ErrorString.h"

namespace kvs
{

namespace cl
{

KernelObject::KernelObject( void ) :
    m_is_created( false )
{
}

KernelObject::KernelObject(
    const kvs::cl::ProgramObject& program,
    const std::string& name ) :
    m_is_created( false )
{
    this->create( program, name );
}

KernelObject::~KernelObject( void )
{
    this->release();
}

const bool KernelObject::isCreated( void ) const
{
    return( m_is_created );
}

const std::string& KernelObject::name( void ) const
{
    return( m_name );
}

const CLKernel& KernelObject::kernel( void ) const
{
    return( m_kernel );
}

void KernelObject::create(
    const kvs::cl::ProgramObject& program,
    const std::string& name )
{
    if ( m_is_created )
    {
        kvsMessageError( "Kernel is already created." );
        return;
    }

    cl_int result = CL_SUCCESS;
    m_kernel = clCreateKernel(
        program.program(), name.c_str(), &result );
    if ( result != CL_SUCCESS )
    {
        kvsMessageError( "OpenCL; %s.", kvs::cl::ErrorString( result ) );
        return;
    }

    m_is_created = true;
    m_name = name;
}

void KernelObject::setArgument(
    const size_t index,
    const size_t size,
    const void* value )
{
    if ( !m_is_created )
    {
        kvsMessageError( "Kernel is not created yet." );
        return;
    }

    const cl_uint arg_index = static_cast<cl_uint>( index );
    const cl_int result = clSetKernelArg(
        m_kernel, arg_index, size, value );
    if ( result != CL_SUCCESS )
    {
        kvsMessageError( "OpenCL; %s.", kvs::cl::ErrorString( result ) );
    }
}

void KernelObject::setArgument(
    const size_t index,
    const kvs::cl::BufferObject& buffer )
{
    this->setArgument( index, sizeof(kvs::cl::CLMemory), &buffer.memory() );
}

void KernelObject::setArgument( const size_t index, const int value )
{
    this->setArgument( index, sizeof(int), &value );
}

void KernelObject::setArgument( const size_t index, const unsigned int value )
{
    this->setArgument( index, sizeof(short), &value );
}

void KernelObject::setArgument( const size_t index, const float value )
{
    this->setArgument( index, sizeof(float), &value );
}

void KernelObject::setArgument( const size_t index, const kvs::Vector3i& vec3 )
{
    int values[3];
    values[0] = vec3[0];
    values[1] = vec3[1];
    values[2] = vec3[2];
    this->setArgument( index, sizeof(int) * 3, values );
}

void KernelObject::setArgument( const size_t index, const kvs::Vector3ui& vec3 )
{
    unsigned int values[3];
    values[0] = vec3[0];
    values[1] = vec3[1];
    values[2] = vec3[2];
    this->setArgument( index, sizeof(unsigned int) * 3, values );
}

void KernelObject::setArgument( const size_t index, const kvs::Vector3f& vec3 )
{
    float values[3];
    values[0] = vec3[0];
    values[1] = vec3[1];
    values[2] = vec3[2];
    this->setArgument( index, sizeof(float) * 3, values );
}

void KernelObject::run(
    kvs::cl::CommandQueue& queue,
    const size_t work_size_dim,
    const size_t* work_size )
{
    if ( !m_is_created )
    {
        kvsMessageError( "Kernel is not created yet." );
        return;
    }

    const cl_int result = clEnqueueNDRangeKernel(
        queue.queue(), m_kernel, work_size_dim, NULL, work_size, NULL, 0, NULL, NULL );
    if ( result != CL_SUCCESS )
    {
        kvsMessageError( "OpenCL; %s.", kvs::cl::ErrorString( result ) );
    }
}

void KernelObject::release( void )
{
    if ( m_is_created )
    {
        clReleaseKernel( m_kernel );
        m_is_created = false;
    }
}

}

}
