//
//  Configuration.cpp
//  
//
//  Created by Jun Nishimura on 11/13/9.
//  Copyright 2011 Jun Nishimura. All rights reserved.
//

#include "Configuration.h"

#include <kvs/Message>
#include <kvs/File>

#define DEFAULT_CONF_PATH "conf.kvsml"

namespace kvs
{

namespace tdw
{

bool                             Configuration::m_is_fullsreen;
kvs::tdw::Master                 Configuration::m_master;
std::vector<kvs::tdw::Renderer>  Configuration::m_renderers;

Configuration::Configuration( void )
{
    this->initialize_parameters();
}

Configuration::~Configuration( void )
{
}

bool Configuration::IsFullScreen( void )
{
    return( m_is_fullsreen );
}

kvs::tdw::Master Configuration::Master( void )
{
    return( m_master );
}

kvs::tdw::Renderer Configuration::Renderer( void )
{
    std::string name( getenv( "KVS_TDW_RENDERER" ) );
    if ( name != "" )
    {
        for ( size_t i = 0; i < m_renderers.size(); i++ )
        {
            if ( name == m_renderers[i].name() ) return( m_renderers[i] );
        }
        kvsMessageError( "<%s> is not defined in conf.kvsml.", name.c_str() );
    }
    else
    {
        kvsMessageError( "This proccess is not renderer." );
    }

    return( kvs::tdw::Renderer() );
}

std::vector<kvs::tdw::Renderer> Configuration::RendererList( void )
{
    return( m_renderers );
}

kvs::Vector2i Configuration::FullResolution( void )
{
    return( m_master.resolution() );
}

kvs::SocketAddress Configuration::MasterAddress( void )
{
    return( m_master.socket() );
}

bool Configuration::HasRemote( void )
{
    return( m_master.hasRemote() );
}

int Configuration::RemotePort( void )
{
    return( m_master.remotePort() );
}

size_t Configuration::RendererCount( void )
{
    return( m_renderers.size() );
}

std::string Configuration::RendererName( void )
{
    std::string name( getenv( "KVS_TDW_RENDERER" ) );
    return( name );
}

std::string Configuration::RendererName( const size_t index )
{
    KVS_ASSERT( index < m_renderers.size() );
    return( m_renderers[ index ].name() );
}

kvs::SocketAddress Configuration::RendererAddress( const std::string name )
{
    for ( size_t i = 0; i < m_renderers.size(); i++ )
    {
        if ( name == m_renderers[i].name() ) return( m_renderers[i].socket() );
    }
    return( kvs::SocketAddress() );
}

kvs::Vector2i Configuration::RendererOrigin( const std::string name )
{
    for ( size_t i = 0; i < m_renderers.size(); i++ )
    {
        if ( name == m_renderers[i].name() ) return( m_renderers[i].origin() );
    }
    return( kvs::Vector2i( 0, 0 ) );
}

kvs::Vector2i Configuration::RendererSize( const std::string name )
{
    for ( size_t i = 0; i < m_renderers.size(); i++ )
    {
        if ( name == m_renderers[i].name() ) return( m_renderers[i].size() );
    }
    return( kvs::Vector2i( 0, 0 ) );
}

void Configuration::initialize_parameters( void )
{
    kvs::File default_file( DEFAULT_CONF_PATH );
    if ( default_file.isExisted() )
    {
        kvs::KVSMLTiledDisplayWall* kvsml = new kvs::KVSMLTiledDisplayWall( DEFAULT_CONF_PATH );
        if ( !kvsml->isSuccess() ) exit( EXIT_FAILURE );
        m_is_fullsreen = kvsml->isFullscreen();
        m_master = kvsml->master();
        m_renderers = kvsml->rendererList();
        delete kvsml;
        return;
    }

    const char* env_path = getenv( "KVS_TDW_CONF_PATH" );
    if ( !env_path )
    {
        kvsMessageError( "Configuration file is not set to KVS_TDW_CONF_PATH." );
        exit( EXIT_FAILURE );
    }

    kvs::File env_file( env_path );
    if ( env_file.isExisted() )
    {
        kvs::KVSMLTiledDisplayWall* kvsml = new kvs::KVSMLTiledDisplayWall( env_path );
        if ( !kvsml->isSuccess() ) exit( EXIT_FAILURE );
        m_is_fullsreen = kvsml->isFullscreen();
        m_master = kvsml->master();
        m_renderers = kvsml->rendererList();
        delete kvsml;
        return;
    }

    kvsMessageError( "Configuration file is not existed." );
    exit( EXIT_FAILURE );
}

}

}
