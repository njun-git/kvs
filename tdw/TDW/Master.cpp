/*
 *  Master.cpp
 *  
 *
 *  Created by njun on 11/07/09.
 *  Copyright 2011 Jun Nishimura. All rights reserved.
 *
 */

#include "Master.h"

namespace kvs
{

namespace tdw
{

Master::Master( void ) :
    m_resolution( 0, 0 ),
    m_has_remote( false ),
    m_remote_port( 0 )
{
}

Master::~Master( void )
{
}

const kvs::SocketAddress& Master::socket( void ) const
{
    return( m_socket );
}

const kvs::Vector2i& Master::resolution( void ) const
{
    return( m_resolution );
}

const bool Master::hasRemote( void ) const
{
    return( m_has_remote );
}

const int Master::remotePort( void ) const
{
    return( m_remote_port );
}

void Master::setSocket( const kvs::SocketAddress& socket )
{
    m_socket = socket;
}

void Master::setResolution( const kvs::Vector2i& resolution )
{
    m_resolution = resolution;
}

void Master::setRemotePort( const int port )
{
    m_has_remote = true;
    m_remote_port = port;
}

} // end of namespace tdw

} // end of namespace kvs
