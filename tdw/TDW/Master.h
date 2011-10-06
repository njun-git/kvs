/*
 *  Master.h
 *  
 *
 *  Created by njun on 11/07/09.
 *  Copyright 2011 Jun Nishimura. All rights reserved.
 *
 */

#ifndef KVS__TDW_MASTER_H_INCLUDE
#define KVS__TDW_MASTER_H_INCLUDE

#include <kvs/SocketAddress>
#include <kvs/Vector2>

namespace kvs
{

namespace tdw
{

class Master
{
    kvsClassName( kvs::tdw::Master );

protected:

    kvs::SocketAddress  m_socket;
    kvs::Vector2i       m_resolution;
    bool                m_has_remote;
    int                 m_remote_port;

public:

    Master( void );

    virtual ~Master( void );

public:

    const kvs::SocketAddress& socket( void ) const;

    const kvs::Vector2i& resolution( void ) const;

    const bool hasRemote( void ) const;

    const int remotePort( void ) const;

public:

    void setSocket( const kvs::SocketAddress& socket );

    void setResolution( const kvs::Vector2i& resolution );

    void setRemotePort( const int port );

};

} // end of namespace tdw

} // end of namespace kvs

#endif
