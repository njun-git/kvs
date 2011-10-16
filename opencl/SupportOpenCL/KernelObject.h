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

#include "ProgramObject.h"
#include "CommandQueue.h"

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

    void run(
        kvs::cl::CommandQueue& queue,
        const size_t work_size_dim,
        const size_t* work_size );

    void release( void );

};

}

}

#endif
