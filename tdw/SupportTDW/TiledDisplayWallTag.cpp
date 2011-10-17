/*
 *  TiledDisplayWallTag.cpp
 *  
 *
 *  Created by njun on 11/07/09.
 *  Copyright 2011 Jun Nishimura. All rights reserved.
 *
 */

#include "TiledDisplayWallTag.h"

#include <kvs/Message>
#include <kvs/String>
#include <kvs/Tokenizer>
#include <kvs/XMLElement>

namespace kvs
{

namespace kvsml
{

TiledDisplayWallTag::TiledDisplayWallTag( void ) :
    kvs::kvsml::TagBase( "TiledDisplayWall" ),
    m_has_nrenderers( false ),
    m_nrenderers( 0 ),
    m_has_full_resolution( false ),
    m_full_resolution( 512, 512 ),
    m_is_fullscreen( false )
{
}

TiledDisplayWallTag::~TiledDisplayWallTag( void )
{
}

const bool TiledDisplayWallTag::hasNRenderers( void ) const
{
    return( m_has_nrenderers );
}

const int TiledDisplayWallTag::nrenderers( void ) const
{
    return( m_nrenderers );
}

const bool TiledDisplayWallTag::hasFullResolution( void ) const
{
    return( m_has_full_resolution );
}

const kvs::Vector2i TiledDisplayWallTag::fullResolution( void ) const
{
    return( m_full_resolution );
}

const bool TiledDisplayWallTag::isFullscreen( void ) const
{
    return( m_is_fullscreen );
}

void TiledDisplayWallTag::setNRenderers( const int nrenderers )
{
    m_has_nrenderers = true;
    m_nrenderers = nrenderers;
}

void TiledDisplayWallTag::setFullResolution( const kvs::Vector2i& full_resolution )
{
    m_has_full_resolution = true;
    m_full_resolution = full_resolution;
}

void TiledDisplayWallTag::setFullscreen( const bool fullscreen )
{
    m_is_fullscreen = fullscreen;
}

const bool TiledDisplayWallTag::read( const kvs::XMLNode::SuperClass* parent )
{
    const std::string tag_name = BaseClass::name();

    BaseClass::m_node = kvs::XMLNode::FindChildNode( parent, tag_name );
    if ( !BaseClass::m_node )
    {
        kvsMessageError( "Cannot find <%s>.", tag_name.c_str() );
        return( false );
    }

    // Element
    const kvs::XMLElement::SuperClass* element = kvs::XMLNode::ToElement( BaseClass::m_node );

    // nrenderers="xxx"
    const std::string nrenderers = kvs::XMLElement::AttributeValue( element, "nrenderers" );
    if ( nrenderers != "" )
    {
        m_has_nrenderers = true;
        m_nrenderers = static_cast<int>( atoi( nrenderers.c_str() ) );
    }

    // resolution="xxx xxx"
    const std::string resolution = kvs::XMLElement::AttributeValue( element, "resolution" );
    if ( resolution != "" )
    {
        const std::string delim(" \n");
        kvs::Tokenizer t( resolution, delim );

        int values[2];
        for ( size_t i = 0; i < 2; i++ )
        {
            if ( t.isLast() )
            {
                kvsMessageError( "2 components are required for 'resolution' in <%s>", tag_name.c_str() );
                return( false );
            }
            values[i] = static_cast<int>( atoi( t.token().c_str() ) );
        }
        m_has_full_resolution = true;
        m_full_resolution = kvs::Vector2i( values[0], values[1] );
    }

    // fullscreen="0 or 1"
    const std::string fullscreen = kvs::XMLElement::AttributeValue( element, "fullscreen" );
    if ( fullscreen != "" )
    {
        m_is_fullscreen = ( static_cast<int>( atoi( fullscreen.c_str() ) ) > 0 );
    }

    return( true );
}

const bool TiledDisplayWallTag::write( kvs::XMLNode::SuperClass* parent )
{
    const std::string tag_name = BaseClass::name();
    kvs::XMLElement element( tag_name );

    if ( m_has_nrenderers )
    {
        const std::string name( "nrenderers" );
        const std::string value( kvs::String( m_nrenderers ).toStdString() );
        element.setAttribute( name, value );
    }
    else
    {
        kvsMessageError( "'nrenderers' is not specified in <%s>.", tag_name.c_str() );
        return( false );
    }

    if ( m_has_full_resolution )
    {
        const std::string name( "resolution" );
        const std::string x( kvs::String( m_full_resolution.x() ).toStdString() );
        const std::string y( kvs::String( m_full_resolution.y() ).toStdString() );
        const std::string value( x + " " + y );
        element.setAttribute( name, value );
    }
    else
    {
        kvsMessageError( "'resolution' is not specified in <%s>.", tag_name.c_str() );
        return( false );
    }

    BaseClass::m_node = parent->InsertEndChild( element );
    if( !BaseClass::m_node )
    {
        kvsMessageError( "Cannot insert <%s>.", tag_name.c_str() );
        return( false );
    }

    return( true );
}

} // end of namespace kvsml

} // end of namespace kvs
