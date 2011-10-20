//
//  KernelObject.h
//  
//
//  Created by Jun Nishimura on 10/15/11.
//  Copyright (c) 2011 Jun Nishimura. All rights reserved.
//

#ifndef KVS__OPENCL__KERNEL_OBJECT_H_INCLUDE
#define KVS__OPENCL__KERNEL_OBJECT_H_INCLUDE

#include <string>
#include <kvs/ClassName>
#include <kvs/Vector3>

#include "ProgramObject.h"
#include "CommandQueue.h"
#include "BufferObject.h"

namespace kvs
{

namespace cl
{

typedef cl_kernel CLKernel;

class KernelObject
{

    // Class name.
    kvsClassName( kvs::cl::KernelObject );

protected:

    bool        m_is_created;
    std::string m_name;
    CLKernel    m_kernel;

public:

    KernelObject( void );

    KernelObject(
        const kvs::cl::ProgramObject& program,
        const std::string& name );

    virtual ~KernelObject( void );

public:

    const bool isCreated( void ) const;

    const std::string& name( void ) const;

    const CLKernel& kernel( void ) const;

public:

    void create(
        const kvs::cl::ProgramObject& program,
        const std::string& name );

    void setArgument(
        const size_t index,
        const size_t size,
        const void* value );

    void setArgument(
        const size_t index,
        const kvs::cl::BufferObject& buffer );

    void setArgument( const size_t index, const int value );
    void setArgument( const size_t index, const unsigned int value );
    void setArgument( const size_t index, const float value );

    void setArgument( const size_t index, const kvs::Vector3i& vec3 );
    void setArgument( const size_t index, const kvs::Vector3ui& vec3 );
    void setArgument( const size_t index, const kvs::Vector3f& vec3 );

    void run(
        kvs::cl::CommandQueue& queue,
        const size_t work_size_dim,
        const size_t* work_size );

    void release( void );

};

}

}

#endif
