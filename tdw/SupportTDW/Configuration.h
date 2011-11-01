//
//  Configuration.h
//  
//
//  Created by Jun Nishimura on 11/13/9.
//  Copyright 2011 Jun Nishimura. All rights reserved.
//

#ifndef KVS__TDW__CONFIGURATION_H_INCLUDE
#define KVS__TDW__CONFIGURATION_H_INCLUDE

#include <kvs/Assert>
#include "KVSMLTiledDisplayWall.h"

namespace kvs
{

namespace tdw
{

class Configuration
{

    kvsClassName( kvs::tdw::Configuration );

protected:

    static bool                             m_is_fullsreen;
    static bool                             m_is_sync;
    static bool                             m_is_rendering;
    static kvs::tdw::Master                 m_master;
    static std::vector<kvs::tdw::Renderer>  m_renderers;

public:

    Configuration( void );

    virtual ~Configuration( void );

public:

    static bool IsFullScreen( void );

    static bool IsSync( void );

    static bool IsRendering( void );

    static void setIsRendering( const bool rendering );

    static kvs::tdw::Master Master( void );

    static kvs::tdw::Renderer Renderer( void );

    static std::vector<kvs::tdw::Renderer> RendererList( void );

public:

    static kvs::Vector2i FullResolution( void );

    static kvs::SocketAddress MasterAddress( void );

    static bool HasRemote( void );

    static int RemotePort( void );

    static size_t RendererCount( void );

    static std::string RendererName( void );

    static std::string RendererName( const size_t index );

    static kvs::SocketAddress RendererAddress( const std::string name );

    static kvs::Vector2i RendererOrigin( const std::string name );

    static kvs::Vector2i RendererSize( const std::string name );

protected:

    static void initialize_parameters( void );

};

}

}

#endif
