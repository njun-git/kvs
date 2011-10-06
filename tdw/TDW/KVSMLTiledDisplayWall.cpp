/*
 *  KVSMLTiledDisplayWall.cpp
 *  
 *
 *  Created by njun on 11/07/09.
 *  Copyright 2011 Jun Nishimura. All rights reserved.
 *
 */

#include "KVSMLTiledDisplayWall.h"

#include <kvs/File>
#include <kvs/XMLDocument>
#include <kvs/XMLDeclaration>
#include <kvs/XMLElement>
#include <kvs/XMLComment>
#include "TiledDisplayWallTag.h"
#include "MasterTag.h"
#include "RendererTag.h"

namespace kvs
{

KVSMLTiledDisplayWall::KVSMLTiledDisplayWall( void ) :
    m_nrenderers( 0 ),
    m_is_fullscreen( false )
{
}

KVSMLTiledDisplayWall::KVSMLTiledDisplayWall( const std::string& filename ) :
    m_nrenderers( 0 ),
    m_is_fullscreen( false )
{
    if ( this->read( filename ) ) { m_is_success = true; }
    else { m_is_success = false; }
}

KVSMLTiledDisplayWall::~KVSMLTiledDisplayWall( void )
{
}

const size_t KVSMLTiledDisplayWall::nrenderers( void ) const
{
    return( m_nrenderers );
}

const bool KVSMLTiledDisplayWall::isFullscreen( void ) const
{
    return( m_is_fullscreen );
}

const kvs::tdw::Master& KVSMLTiledDisplayWall::master( void ) const
{
    return( m_master );
}

const std::vector<kvs::tdw::Renderer>& KVSMLTiledDisplayWall::rendererList( void ) const
{
    return( m_renderers );
}

void KVSMLTiledDisplayWall::setFilename( const std::string& filename )
{
    m_filename = filename;
}

const bool KVSMLTiledDisplayWall::read( const std::string& filename )
{
    m_filename = filename;
    m_renderers.clear();

    // XML document
    kvs::XMLDocument document;
    if ( !document.read( filename ) )
    {
        kvsMessageError( "%s", document.ErrorDesc().c_str() );
        return( false );
    }

    // <KVSML>
    if ( !m_kvsml_tag.read( &document ) )
    {
        kvsMessageError( "Cannot read <%s>.", m_kvsml_tag.name().c_str() );
        return( false );
    }

    // <TiledDisplayWall>
    kvs::kvsml::TiledDisplayWallTag tdw_tag;
    if ( !tdw_tag.read( m_kvsml_tag.node() ) )
    {
        kvsMessageError( "Cannot read <%s>.", tdw_tag.name().c_str() );
        return( false );
    }
    m_nrenderers = static_cast<size_t>( tdw_tag.nrenderers() );
    m_master.setResolution( tdw_tag.fullResolution() );
    m_is_fullscreen = tdw_tag.isFullscreen();

    // <Master>
    kvs::kvsml::MasterTag master_tag;
    if ( !master_tag.read( tdw_tag.node() ) )
    {
        kvsMessageError( "Cannot read <%s>.", master_tag.name().c_str() );
        return( false );
    }
    m_master.setSocket( kvs::SocketAddress( kvs::IPAddress( master_tag.ip().c_str() ), master_tag.port() ) );
    if ( master_tag.hasRemotePort() )
    {
        m_master.setRemotePort( master_tag.remotePort() );
    }

    // <Renderer>
    kvs::kvsml::RendererTag renderer_tag;
    kvs::XMLNode::SuperClass* node = kvs::XMLNode::FindChildNode( tdw_tag.node(), renderer_tag.name() );
    size_t counter = 0;
    while ( node )
    {
        if ( !renderer_tag.read( kvs::XMLNode::ToElement( node ) ) )
        {
            kvsMessageError( "Cannot read <%s>.", renderer_tag.name().c_str() );
            return( false );
        }

        if ( counter < m_nrenderers )
        {
            kvs::tdw::Renderer renderer;
            renderer.setName( renderer_tag.nodeName() );
            renderer.setSocket( kvs::SocketAddress( kvs::IPAddress( renderer_tag.ip().c_str() ), renderer_tag.port() ) );
            renderer.setOrigin( renderer_tag.origin() );
            renderer.setSize( renderer_tag.size() );

            m_renderers.push_back( renderer );
            counter++;
        }

        node = tdw_tag.node()->IterateChildren( renderer_tag.name(), node );
    }

    return( true );
}

const bool KVSMLTiledDisplayWall::write( const std::string& filename )
{
    m_filename = filename;

    return( true );
}

const bool KVSMLTiledDisplayWall::CheckFileExtension( const std::string& filename )
{
    const kvs::File file( filename );
    if ( file.extension() == "kvsml" ||
         file.extension() == "KVSML" ||
         file.extension() == "xml"   ||
         file.extension() == "XML" )
    {
        return( true );
    }

    return( false );
}

const bool KVSMLTiledDisplayWall::CheckFileFormat( const std::string& filename )
{
    kvs::XMLDocument document;
    if ( !document.read( filename ) ) return( false );

    // <KVSML>
    kvs::kvsml::KVSMLTag kvsml_tag;
    if ( !kvsml_tag.read( &document ) ) return( false );

    // <TiledDisplayWall>
    kvs::kvsml::TiledDisplayWallTag tdw_tag;
    if ( !tdw_tag.read( kvsml_tag.node() ) ) return( false );

    return( true );
}

std::ostream& operator << ( std::ostream& os, const KVSMLTiledDisplayWall& rhs )
{
    os << "Number of Renderers : " << rhs.m_nrenderers << std::endl;
    os << "Resolution          : " << rhs.m_master.resolution() << std::endl;
    os << "Master Address      : " << rhs.m_master.socket() << std::endl;

    for ( size_t i = 0; i < rhs.m_nrenderers; i++ )
    {
        os << "Renderer Name : " << rhs.m_renderers.at(i).name() << ", ";
        os << "Renderer Address : " << rhs.m_renderers.at(i).socket() << std::endl;
    }

    return( os );
}

} // end of namespace kvs
