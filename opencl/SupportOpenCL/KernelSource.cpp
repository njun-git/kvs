//
//  KernelSource.cpp
//  
//
//  Created by Jun Nishimura on 10/15/11.
//  Copyright (c) 2011 Jun Nishimura. All rights reserved.
//

#include "KernelSource.h"

#include <kvs/Message>
#include <sstream>
#include <fstream>

namespace kvs
{

namespace cl
{

KernelSource::KernelSource( void )
{
}

KernelSource::KernelSource( const std::string& filename )
{
    this->read( filename );
}

KernelSource::~KernelSource( void )
{
}

const std::string& KernelSource::code( void ) const
{
    return( m_code );
}

const size_t KernelSource::size( void ) const
{
    return( m_code.length() );
}

const bool KernelSource::read( const std::string& filename )
{
    std::ifstream ifs( filename.c_str() );
    if ( ifs.fail() )
    {
        kvsMessageError( "Cannot open <%s>.", filename.c_str() );
        return( false );
    }

    std::ostringstream buffer;
    buffer << ifs.rdbuf();

    m_code = std::string( buffer.str() );
    ifs.close();

    return( true );
}

}

}
