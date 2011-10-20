//
//  Context.h
//  
//
//  Created by Jun Nishimura on 10/18/11.
//  Copyright (c) 2011 Jun Nishimura. All rights reserved.
//

#ifndef KVS__OPENNI__CONTEXT_H_INCLUDE
#define KVS__OPENNI__CONTEXT_H_INCLUDE

#include <kvs/ClassName>

#include "OpenNI.h"
#include "GeneratorBase.h"

namespace kvs
{

namespace ni
{

class Context
{

    // Class name.
    kvsClassName( kvs::ni::Context );

protected:

    bool            m_is_initialized;
    xn::Context     m_context;

public:

    Context( void );

    virtual ~Context( void );

public:

    const bool initialize( void );

    void shutdown( void );

public:

    xn::Context& context( void );

    const bool isMirror( void );

public:

    const bool setMirror( const bool is_mirror );

    const bool startGeneration( void );

    const bool stopGeneration( void );

    const bool update( kvs::ni::GeneratorBase& generator );

};

}

}

#endif
