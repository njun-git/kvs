//
//  KernelSource.h
//  
//
//  Created by Jun Nishimura on 10/15/11.
//  Copyright (c) 2011 Jun Nishimura. All rights reserved.
//

#ifndef KVS__OPENCL__KERNEL_SOURCE_H_INCLUDE
#define KVS__OPENCL__KERNEL_SOURCE_H_INCLUDE

#include <string>
#include <kvs/ClassName>

namespace kvs
{

namespace cl
{

class KernelSource
{

    // Class name.
    kvsClassName( kvs::cl::KernelSource );

protected:

    std::string m_code;

public:

    KernelSource( void );

    KernelSource( const std::string& filename );

    virtual ~KernelSource( void );

public:

    const std::string& code( void ) const;

    const size_t size( void ) const;

public:

    const bool read( const std::string& filename );

};

}

}

#endif
