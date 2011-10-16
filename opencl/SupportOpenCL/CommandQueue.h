//
//  CommandQueue.h
//  
//
//  Created by Jun Nishimura on 10/15/11.
//  Copyright (c) 2011 Jun Nishimura. All rights reserved.
//

#ifndef KVS__OPENCL__COMMAND_QUEUE_H_INCLUDE
#define KVS__OPENCL__COMMAND_QUEUE_H_INCLUDE

#include <kvs/ClassName>

#include "OpenCL.h"
#include "Device.h"
#include "Context.h"

namespace kvs
{

namespace cl
{

typedef cl_command_queue CLQueue;

class CommandQueue
{

    // Class name.
    kvsClassName( kvs::cl::CommandQueue );

protected:

    bool        m_is_created;
    CLQueue     m_queue;

public:

    CommandQueue( void );

    CommandQueue(
        const kvs::cl::Context& context,
        const kvs::cl::CLDevice& device );

    virtual ~CommandQueue( void );

public:

    const bool isCreated( void ) const;

    const CLQueue& queue( void ) const;

public:

    void create(
        const kvs::cl::Context& context,
        const kvs::cl::CLDevice& device );

    void finish( void );

    void flush( void );

    void release( void );

};

}

}

#endif
