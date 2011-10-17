/*
 *  Renderer.h
 *  
 *
 *  Created by njun on 11/07/09.
 *  Copyright 2011 Jun Nishimura. All rights reserved.
 *
 */

#ifndef KVS__TDW_RENDERER_H_INCLUDE
#define KVS__TDW_RENDERER_H_INCLUDE

#include <kvs/SocketAddress>
#include <kvs/Vector2>

namespace kvs
{

namespace tdw
{

class Renderer
{
    kvsClassName( kvs::tdw::Renderer );

protected:

    std::string         m_name;
    kvs::SocketAddress  m_socket;
    kvs::Vector2i       m_origin;
    kvs::Vector2i       m_size;

public:

    Renderer( void );

    virtual ~Renderer( void );

public:

    const std::string& name( void ) const;

    const kvs::SocketAddress& socket( void ) const;

    const kvs::Vector2i& origin( void ) const;

    const kvs::Vector2i& size( void ) const;

public:

    void setName( const std::string& name );

    void setSocket( const kvs::SocketAddress& socket );

    void setOrigin( const kvs::Vector2i& origin );

    void setSize( const kvs::Vector2i& size );

};

} // end of namespace tdw

} // end of namespace kvs

#endif
