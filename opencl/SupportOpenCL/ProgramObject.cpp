//
//  ProgramObject.cpp
//  
//
//  Created by Jun Nishimura on 10/15/11.
//  Copyright (c) 2011 Jun Nishimura. All rights reserved.
//

#include "ProgramObject.h"

#include <kvs/Message>
#include "ErrorString.h"

namespace kvs
{

namespace cl
{

ProgramObject::ProgramObject( void ) :
    m_is_created( false )
{
}

ProgramObject::ProgramObject(
    const kvs::cl::Context& context,
    const kvs::cl::KernelSource& source ) :
    m_is_created( false )
{
    this->create( context, source );
}

ProgramObject::~ProgramObject( void )
{
    this->release();
}

const bool ProgramObject::isCreated( void ) const
{
    return( m_is_created );
}

const CLProgram& ProgramObject::program( void ) const
{
    return( m_program );
}

void ProgramObject::create(
    const kvs::cl::Context& context,
    const kvs::cl::KernelSource& source )
{
    if ( m_is_created )
    {
        kvsMessageError( "Program is already created." );
        return;
    }

    cl_int result = CL_SUCCESS;
    const char* char_source = source.code().c_str();
    const size_t source_size = source.size();
    m_program = clCreateProgramWithSource(
        context.context(), 1, &char_source, &source_size, &result );
    if ( result != CL_SUCCESS )
    {
        kvsMessageError( "OpenCL; %s.", kvs::cl::ErrorString( result ) );
        return;
    }

    m_is_created = true;
}

const bool ProgramObject::build( void )
{
    cl_int result = clBuildProgram( m_program, 0, NULL, NULL, NULL, NULL );
    if ( result != CL_SUCCESS )
    {
        kvsMessageError( "OpenCL; %s.", kvs::cl::ErrorString( result ) );
        exit( EXIT_FAILURE );
    }

    return( true );
}

void ProgramObject::release( void )
{
    if ( m_is_created )
    {
        clReleaseProgram( m_program );
        m_is_created = false;
    }
}

}

}
