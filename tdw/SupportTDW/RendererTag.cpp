/*
 *  RendererTag.cpp
 *  
 *
 *  Created by njun on 11/07/09.
 *  Copyright 2011 Jun Nishimura. All rights reserved.
 *
 */

#include "RendererTag.h"

#include <kvs/Message>
#include <kvs/String>
#include <kvs/Tokenizer>
#include <kvs/XMLElement>

namespace kvs
{

namespace kvsml
{

RendererTag::RendererTag( void ) :
    kvs::kvsml::TagBase( "Renderer" ),
    m_has_name( false ),
    m_node_name( "" ),
    m_has_socket( false ),
    m_ip( "" ),
    m_port( 0 ),
    m_has_origin( false ),
    m_origin( 0, 0 ),
    m_has_size( false ),
    m_size( 0, 0 )
{
}

RendererTag::~RendererTag( void )
{
}

const bool RendererTag::hasName( void ) const
{
    return( m_has_name );
}

const std::string& RendererTag::nodeName( void ) const
{
    return( m_node_name );
}

const bool RendererTag::hasSocket( void ) const
{
    return( m_has_socket );
}

const std::string& RendererTag::ip( void ) const
{
    return( m_ip );
}

const int RendererTag::port( void ) const
{
    return( m_port );
}

const bool RendererTag::hasOrigin( void ) const
{
    return( m_has_origin );
}

const kvs::Vector2i& RendererTag::origin( void ) const
{
    return( m_origin );
}

const bool RendererTag::hasSize( void ) const
{
    return( m_has_size );
}

const kvs::Vector2i& RendererTag::size( void ) const
{
    return( m_size );
}

void RendererTag::setNodeName( const std::string& name )
{
    m_has_name = true;
    m_node_name = name;
}

void RendererTag::setSocket( const std::string& ip, const int port )
{
    m_has_socket = true;
    m_ip = ip;
    m_port = port;
}

void RendererTag::setOrigin( const kvs::Vector2i& origin )
{
    m_has_origin = true;
    m_origin = origin;
}

void RendererTag::setSize( const kvs::Vector2i& size )
{
    m_has_size = true;
    m_size = size;
}

const bool RendererTag::read( const kvs::XMLNode::SuperClass* parent )
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

    // name="xxx"
    const std::string name = kvs::XMLElement::AttributeValue( element, "name" );
    if ( name != "" )
    {
        m_has_name = true;
        m_node_name = name;
    }

    // socket="xxx:xxx"
    const std::string socket = kvs::XMLElement::AttributeValue( element, "socket" );
    if ( socket != "" )
    {
        const std::string delim(":");
        kvs::Tokenizer t( socket, delim );
        if ( t.isLast() )
        {
            kvsMessageError( "2 components are required for 'socket' in <%s>", tag_name.c_str() );
            return( false );
        }
        m_ip = t.token();

        if ( t.isLast() )
        {
            kvsMessageError( "2 components are required for 'socket' in <%s>", tag_name.c_str() );
            return( false );
        }
        m_port = static_cast<int>( atoi( t.token().c_str() ) );

        m_has_socket = true;
    }

    // origin="xxx xxx"
    const std::string origin = kvs::XMLElement::AttributeValue( element, "origin" );
    if ( origin != "" )
    {
        const std::string delim(" \n");
        kvs::Tokenizer t( origin, delim );

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
        m_has_origin = true;
        m_origin = kvs::Vector2i( values[0], values[1] );
    }

    // size="xxx xxx"
    const std::string size = kvs::XMLElement::AttributeValue( element, "size" );
    if ( size != "" )
    {
        const std::string delim(" \n");
        kvs::Tokenizer t( size, delim );

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
        m_has_size = true;
        m_size = kvs::Vector2i( values[0], values[1] );
    }

    return( true );
}

const bool RendererTag::read( const kvs::XMLElement::SuperClass* element )
{
    const std::string tag_name = BaseClass::name();
    // name="xxx"
    const std::string name = kvs::XMLElement::AttributeValue( element, "name" );
    if ( name != "" )
    {
        m_has_name = true;
        m_node_name = name;
    }

    // socket="xxx:xxx"
    const std::string socket = kvs::XMLElement::AttributeValue( element, "socket" );
    if ( socket != "" )
    {
        const std::string delim(":");
        kvs::Tokenizer t( socket, delim );
        if ( t.isLast() )
        {
            kvsMessageError( "2 components are required for 'socket' in <%s>", tag_name.c_str() );
            return( false );
        }
        m_ip = t.token();

        if ( t.isLast() )
        {
            kvsMessageError( "2 components are required for 'socket' in <%s>", tag_name.c_str() );
            return( false );
        }
        m_port = static_cast<int>( atoi( t.token().c_str() ) );

        m_has_socket = true;
    }

    // origin="xxx xxx"
    const std::string origin = kvs::XMLElement::AttributeValue( element, "origin" );
    if ( origin != "" )
    {
        const std::string delim(" \n");
        kvs::Tokenizer t( origin, delim );

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
        m_has_origin = true;
        m_origin = kvs::Vector2i( values[0], values[1] );
    }

    // size="xxx xxx"
    const std::string size = kvs::XMLElement::AttributeValue( element, "size" );
    if ( size != "" )
    {
        const std::string delim(" \n");
        kvs::Tokenizer t( size, delim );

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
        m_has_size = true;
        m_size = kvs::Vector2i( values[0], values[1] );
    }

    return( true );
}

const bool RendererTag::write( kvs::XMLNode::SuperClass* parent )
{
    const std::string tag_name = BaseClass::name();
    kvs::XMLElement element( tag_name );

    if ( m_has_name )
    {
        const std::string name( "name" );
        const std::string value( m_node_name );
        element.setAttribute( name, value );
    }
    else
    {
        kvsMessageError( "'name' is not specified in <%s>.", tag_name.c_str() );
        return( false );
    }

    if ( m_has_socket )
    {
        const std::string name( "socket" );
        const std::string ip( m_ip );
        const std::string port( kvs::String( m_port ).toStdString() );
        const std::string value( ip + ":" + port );
        element.setAttribute( name, value );
    }
    else
    {
        kvsMessageError( "'socket' is not specified in <%s>.", tag_name.c_str() );
        return( false );
    }

    if ( m_has_origin )
    {
        const std::string name( "origin" );
        const std::string x( kvs::String( m_origin.x() ).toStdString() );
        const std::string y( kvs::String( m_origin.y() ).toStdString() );
        const std::string value( x + " " + y );
        element.setAttribute( name, value );
    }
    else
    {
        kvsMessageError( "'origin' is not specified in <%s>.", tag_name.c_str() );
        return( false );
    }

    if ( m_has_size )
    {
        const std::string name( "size" );
        const std::string x( kvs::String( m_size.x() ).toStdString() );
        const std::string y( kvs::String( m_size.y() ).toStdString() );
        const std::string value( x + " " + y );
        element.setAttribute( name, value );
    }
    else
    {
        kvsMessageError( "'size' is not specified in <%s>.", tag_name.c_str() );
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
