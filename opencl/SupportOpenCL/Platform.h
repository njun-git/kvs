//
//  Platform.h
//  
//
//  Created by Jun Nishimura on 10/14/11.
//  Copyright (c) 2011 Jun Nishimura. All rights reserved.
//

#ifndef KVS__OPENCL__PLATFORM_H_INCLUDE
#define KVS__OPENCL__PLATFORM_H_INCLUDE

#include <kvs/ClassName>

#include "OpenCL.h"

namespace kvs
{

namespace cl
{

typedef cl_platform_id CLPlatform;

class Platform
{

    // Class name.
    kvsClassName( kvs::cl::Platform );

protected:

    cl_uint     m_nplatforms;
    CLPlatform* m_platforms;

public:

    Platform( void );

    virtual ~Platform( void );

public:

    void initialize( void );

    void clear( void );

public:

    const size_t nplatforms( void ) const;

    const CLPlatform& platform( const size_t index = 0 ) const;

public:

    const std::string profile( const size_t index = 0 ) const;

    const std::string version( const size_t index = 0 ) const;

    const std::string name( const size_t index = 0 ) const;

    const std::string vendor( const size_t index = 0 ) const;

    const std::string extensions( const size_t index = 0 ) const;

private:

    const std::string get_platform_info( const size_t index, const cl_platform_info info ) const;

};

}

}

#endif
