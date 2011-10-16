//
//  BufferObject.h
//  
//
//  Created by Jun Nishimura on 10/15/11.
//  Copyright (c) 2011 Jun Nishimura. All rights reserved.
//

#ifndef KVS__OPENCL__BUFFER_OBJECT_H_INCLUDE
#define KVS__OPENCL__BUFFER_OBJECT_H_INCLUDE

#include <kvs/ClassName>
#include <kvs/Texture2D>

#ifdef KVS_SUPPORT_GLEW
#include <kvs/glew/VertexBufferObject>
#endif

#include "Context.h"
#include "CommandQueue.h"

namespace kvs
{

namespace cl
{

typedef cl_mem CLMemory;

class BufferObject
{

    // Class name.
    kvsClassName( kvs::cl::KernelObject );

public:

    enum AccessType
    {
        ReadWrite           = CL_MEM_READ_WRITE,
        WriteOnly           = CL_MEM_WRITE_ONLY,
        ReadOnly            = CL_MEM_READ_ONLY,
        UseHostPointer      = CL_MEM_USE_HOST_PTR,
        AllocateHostPointer = CL_MEM_ALLOC_HOST_PTR,
        CopyHostPointer     = CL_MEM_COPY_HOST_PTR,
    };

protected:

    bool        m_is_created;
    AccessType  m_access_type;
    CLMemory    m_memory;
    size_t      m_size;

public:

    BufferObject( void );

    BufferObject( const AccessType& access_type );

    virtual ~BufferObject( void );

public:

    const bool isCreated( void ) const;

    const AccessType& accessType( void ) const;

    const CLMemory& memory( void ) const;

    const size_t size( void ) const;

public:

    void setAccessType( const AccessType& access_type );

    void addAccessType( const AccessType& access_type );

public:

    void create(
        const kvs::cl::Context& context,
        const size_t size,
        void* pointer = NULL );

#ifdef KVS_SUPPORT_GLEW
    void create(
        const kvs::cl::Context& context,
        const kvs::glew::VertexBufferObject& vbo );
#endif

    void acquireGL( const kvs::cl::CommandQueue& queue );

    void releaseGL( const kvs::cl::CommandQueue& queue );

    void create(
        const kvs::cl::Context& context,
        const kvs::Texture2D& texture );

    // Device to Host
    const bool readBuffer(
        const kvs::cl::CommandQueue& queue,
        const size_t size,
        void* pointer,
        const size_t offset = 0,
        const bool is_blocking = true );

    // Host to Device
    const bool writeBuffer(
        const kvs::cl::CommandQueue& queue,
        const size_t size,
        const void* pointer,
        const size_t offset = 0,
        const bool is_blocking = true );

    // Device to Device
    const bool copyFromBuffer(
        const kvs::cl::CommandQueue& queue,
        const kvs::cl::BufferObject& src,
        const size_t size,
        const size_t src_offset = 0,
        const size_t self_offset = 0 );

    // Device to Device
    const bool copyToBuffer(
        const kvs::cl::CommandQueue& queue,
        const kvs::cl::BufferObject& dst,
        const size_t size,
        const size_t dst_offset = 0,
        const size_t self_offset = 0 );

    void release( void );

};

}

}

#endif
