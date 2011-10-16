//
//  OpenCL.h
//  
//
//  Created by Jun Nishimura on 10/14/11.
//  Copyright (c) 2011 Jun Nishimura. All rights reserved.
//

#ifndef KVS_SUPPORT_OPENCL_OPENCL_H_INCLUDE
#define KVS_SUPPORT_OPENCL_OPENCL_H_INCLUDE

#include <string>
#include <kvs/Platform>

#ifdef KVS_SUPPORT_GLEW
#include <kvs/glew/VertexBufferObject>
#endif

#ifdef KVS_PLATFORM_MACOSX
#include <OpenCL/OpenCL.h>
#else
#include <CL/opencl.h>
#endif

namespace kvs
{

namespace cl
{

inline const std::string Description( void )
{
    const std::string description( "OpenCL - Open Computing Language" );
    return( description );
}

}

}

#endif
