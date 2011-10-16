//
//  BufferObject.cpp
//  
//
//  Created by Jun Nishimura on 10/15/11.
//  Copyright (c) 2011 Jun Nishimura. All rights reserved.
//

#include "BufferObject.h"

#include <kvs/Message>
#include "ErrorString.h"

namespace kvs
{

namespace cl
{

BufferObject::BufferObject( void ) :
    m_is_created( false ),
    m_access_type( ReadWrite ),
    m_size( 0 )
{
}

BufferObject::BufferObject( const AccessType& access_type ) :
    m_is_created( false ),
    m_access_type( access_type ),
    m_size( 0 )
{
}

BufferObject::~BufferObject( void )
{
    this->release();
}

const bool BufferObject::isCreated( void ) const
{
    return( m_is_created );
}

const BufferObject::AccessType& BufferObject::accessType( void ) const
{
    return( m_access_type );
}

const CLMemory& BufferObject::memory( void ) const
{
    return( m_memory );
}

const size_t BufferObject::size( void ) const
{
    return( m_size );
}

void BufferObject::setAccessType( const AccessType& access_type )
{
    m_access_type = access_type;
}

void BufferObject::addAccessType( const AccessType& access_type )
{
    int type = static_cast<int>( m_access_type ) | static_cast<int>( access_type );
    m_access_type = static_cast<AccessType>( type );
}

void BufferObject::create(
    const kvs::cl::Context& context,
    const size_t size,
    void* pointer )
{
    if ( m_is_created )
    {
        kvsMessageError( "Buffer is already created." );
        return;
    }

    cl_int result = CL_SUCCESS;
    const cl_mem_flags flags = static_cast<cl_mem_flags>( m_access_type );
    m_memory = clCreateBuffer(
        context.context(), flags, size, pointer, &result );
    if ( result != CL_SUCCESS )
    {
        kvsMessageError( "OpenCL; %s.", kvs::cl::ErrorString( result ) );
        return;
    }

    m_is_created = true;
    m_size = size;
}

#ifdef KVS_SUPPORT_GLEW
void BufferObject::create(
    const kvs::cl::Context& context,
    const kvs::glew::VertexBufferObject& vbo )
{
    if ( m_is_created )
    {
        kvsMessageError( "Buffer is already created." );
        return;
    }

    cl_int result = CL_SUCCESS;
    const cl_mem_flags flags = static_cast<cl_mem_flags>( m_access_type );
    m_memory = clCreateFromGLBuffer(
        context.context(), flags, vbo.id(), &result );
    if ( result != CL_SUCCESS )
    {
        kvsMessageError( "OpenCL; %s.", kvs::cl::ErrorString( result ) );
        return;
    }

    m_is_created = true;
    m_size = 0;
}
#endif

void BufferObject::acquireGL( const kvs::cl::CommandQueue& queue )
{
    if ( !m_is_created )
    {
        kvsMessageError( "Buffer is not created yet." );
        return;
    }

    const cl_int result = clEnqueueAcquireGLObjects(
        queue.queue(), 1, &m_memory, 0, NULL, NULL );
    if ( result != CL_SUCCESS )
    {
        kvsMessageError( "OpenCL; %s.", kvs::cl::ErrorString( result ) );
    }
}

void BufferObject::releaseGL( const kvs::cl::CommandQueue& queue )
{
    if ( !m_is_created )
    {
        kvsMessageError( "Buffer is not created yet." );
        return;
    }

    const cl_int result = clEnqueueReleaseGLObjects(
        queue.queue(), 1, &m_memory, 0, NULL, NULL );
    if ( result != CL_SUCCESS )
    {
        kvsMessageError( "OpenCL; %s.", kvs::cl::ErrorString( result ) );
    }
}

void BufferObject::create(
    const kvs::cl::Context& context,
    const kvs::Texture2D& texture )
{
    if ( m_is_created )
    {
        kvsMessageError( "Buffer is already created." );
        return;
    }

    cl_int result = CL_SUCCESS;
    const cl_mem_flags flags = static_cast<cl_mem_flags>( m_access_type );
    m_memory = clCreateFromGLTexture2D(
        context.context(), flags, GL_TEXTURE_2D, 0, texture.id(), &result );
    if ( result != CL_SUCCESS )
    {
        kvsMessageError( "OpenCL; %s.", kvs::cl::ErrorString( result ) );
        return;
    }

    m_is_created = true;
    m_size = 0;
}

const bool BufferObject::readBuffer(
    const kvs::cl::CommandQueue& queue,
    const size_t size,
    void* pointer,
    const size_t offset,
    const bool is_blocking )
{
    if ( !m_is_created )
    {
        kvsMessageError( "Buffer is not created yet." );
        return( false );
    }

    const cl_bool blocking = static_cast<cl_bool>( is_blocking );
    const cl_int result = clEnqueueReadBuffer(
        queue.queue(), m_memory, blocking, offset, size, pointer, 0, NULL, NULL );
    if ( result != CL_SUCCESS )
    {
        kvsMessageError( "OpenCL; %s.", kvs::cl::ErrorString( result ) );
        return( false );
    }
    return( true );
}

const bool BufferObject::writeBuffer(
    const kvs::cl::CommandQueue& queue,
    const size_t size,
    const void* pointer,
    const size_t offset,
    const bool is_blocking )
{
    if ( !m_is_created )
    {
        kvsMessageError( "Buffer is not created yet." );
        return( false );
    }

    const cl_bool blocking = static_cast<cl_bool>( is_blocking );
    const cl_int result = clEnqueueWriteBuffer(
        queue.queue(), m_memory, blocking, offset, size, pointer, 0, NULL, NULL );
    if ( result != CL_SUCCESS )
    {
        kvsMessageError( "OpenCL; %s.", kvs::cl::ErrorString( result ) );
        return( false );
    }
    return( true );
}

const bool BufferObject::copyFromBuffer(
    const kvs::cl::CommandQueue& queue,
    const kvs::cl::BufferObject& src,
    const size_t size,
    const size_t src_offset,
    const size_t self_offset )
{
    if ( !m_is_created )
    {
        kvsMessageError( "Buffer is not created yet." );
        return( false );
    }

    const cl_int result = clEnqueueCopyBuffer(
        queue.queue(), src.memory(), m_memory, src_offset, self_offset, size, 0, NULL, NULL );
    if ( result != CL_SUCCESS )
    {
        kvsMessageError( "OpenCL; %s.", kvs::cl::ErrorString( result ) );
        return( false );
    }
    return( true );
}

const bool BufferObject::copyToBuffer(
    const kvs::cl::CommandQueue& queue,
    const kvs::cl::BufferObject& dst,
    const size_t size,
    const size_t dst_offset,
    const size_t self_offset )
{
    if ( !m_is_created )
    {
        kvsMessageError( "Buffer is not created yet." );
        return( false );
    }

    const cl_int result = clEnqueueCopyBuffer(
        queue.queue(), m_memory, dst.memory(), self_offset, dst_offset, size, 0, NULL, NULL );
    if ( result != CL_SUCCESS )
    {
        kvsMessageError( "OpenCL; %s.", kvs::cl::ErrorString( result ) );
        return( false );
    }
    return( true );
}

void BufferObject::release( void )
{
    if ( m_is_created )
    {
        clReleaseMemObject( m_memory );
        m_is_created = false;
        m_size = 0;
    }
}

}

}
