/*
 *  Renderer.cpp
 *  
 *
 *  Created by njun on 11/07/09.
 *  Copyright 2011 Jun Nishimura. All rights reserved.
 *
 */

#include "Renderer.h"

namespace kvs
{

namespace tdw
{

Renderer::Renderer( void ) :
    m_name( "" ),
    m_origin( 0, 0 ),
    m_size( 0, 0 )
{
}

Renderer::~Renderer( void )
{
}

const std::string& Renderer::name( void ) const
{
    return( m_name );
}

const kvs::SocketAddress& Renderer::socket( void ) const
{
    return( m_socket );
}

const kvs::Vector2i& Renderer::origin( void ) const
{
    return( m_origin );
}

const kvs::Vector2i& Renderer::size( void ) const
{
    return( m_size );
}

void Renderer::setName( const std::string& name )
{
    m_name = name;
}

void Renderer::setSocket( const kvs::SocketAddress& socket )
{
    m_socket = socket;
}

void Renderer::setOrigin( const kvs::Vector2i& origin )
{
    m_origin = origin;
}

void Renderer::setSize( const kvs::Vector2i& size )
{
    m_size = size;
}

} // end of namespace tdw

} // end of namespace kvs
