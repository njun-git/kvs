//
//  ProgramObject.h
//  
//
//  Created by Jun Nishimura on 10/15/11.
//  Copyright (c) 2011 Jun Nishimura. All rights reserved.
//

#ifndef KVS__OPENCL__PROGRAM_OBJECT_H_INCLUDE
#define KVS__OPENCL__PROGRAM_OBJECT_H_INCLUDE

#include <kvs/ClassName>

#include "OpenCL.h"
#include "Context.h"
#include "KernelSource.h"

namespace kvs
{

namespace cl
{

typedef cl_program CLProgram;

class ProgramObject
{

    // Class name.
    kvsClassName( kvs::cl::ProgramObject );

protected:

    bool        m_is_created;
    CLProgram   m_program;

public:

    ProgramObject( void );

    ProgramObject(
        const kvs::cl::Context& context,
        const kvs::cl::KernelSource& source );

    virtual ~ProgramObject( void );

public:

    const bool isCreated( void ) const;

    const CLProgram& program( void ) const;

public:

    void create(
        const kvs::cl::Context& context,
        const kvs::cl::KernelSource& source );

    const bool build( void );

    void release( void );

};

}

}

#endif
