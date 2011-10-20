//
//  Context.h
//  
//
//  Created by Jun Nishimura on 10/14/11.
//  Copyright (c) 2011 Jun Nishimura. All rights reserved.
//

#ifndef KVS__OPENCL__CONTEXT_H_INCLUDE
#define KVS__OPENCL__CONTEXT_H_INCLUDE

#include <kvs/ClassName>

#include "OpenCL.h"
#include "Device.h"

namespace kvs
{

namespace cl
{

typedef cl_context CLContext;

class Context
{

    // Class name.
    kvsClassName( kvs::cl::Context );

protected:

    bool        m_is_created;
    CLContext   m_context;

public:

    Context( void );

    Context( const kvs::cl::Device& device, const bool use_gl_context );

    virtual ~Context( void );

public:

    const bool isCreated( void ) const;

    const CLContext& context( void ) const;

public:

    void create( const kvs::cl::Device& device );

    void release( void );

};

}

}

#endif
