//
//  GeneratorBase.cpp
//  
//
//  Created by Jun Nishimura on 10/18/11.
//  Copyright (c) 2011 Jun Nishimura. All rights reserved.
//

#include "GeneratorBase.h"

namespace kvs
{

namespace ni
{

GeneratorBase::GeneratorBase( void ) :
    m_is_created( false )
{
}

GeneratorBase::~GeneratorBase( void )
{
}

const bool GeneratorBase::isCreated( void ) const
{
    return( m_is_created );
}

}

}
