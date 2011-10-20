//
//  GeneratorBase.h
//  
//
//  Created by Jun Nishimura on 10/18/11.
//  Copyright (c) 2011 Jun Nishimura. All rights reserved.
//

#ifndef KVS__OPENNI__GENERATOR_BASE_H_INCLUDE
#define KVS__OPENNI__GENERATOR_BASE_H_INCLUDE

#include <kvs/ClassName>

#include "OpenNI.h"

namespace kvs
{

namespace ni
{

class Context;

class GeneratorBase
{

    // Class name.
    kvsClassName( kvs::ni::GeneratorBase );

protected:

    bool    m_is_created;

public:

    GeneratorBase( void );

    virtual ~GeneratorBase( void );

public:

    const bool isCreated( void ) const;

    virtual const bool create( kvs::ni::Context& context ) = 0;

public:

    virtual xn::Generator& generator( void ) = 0;

};

}

}

#endif
