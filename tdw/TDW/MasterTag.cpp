/*
 *  MasterTag.cpp
 *  
 *
 *  Created by njun on 11/07/09.
 *  Copyright 2011 Jun Nishimura. All rights reserved.
 *
 */

#include "MasterTag.h"

#include <kvs/Message>
#include <kvs/String>
#include <kvs/Tokenizer>
#include <kvs/XMLElement>

namespace kvs
{

namespace kvsml
{

MasterTag::MasterTag( void ) :
    kvs::kvsml::TagBase( "Master" ),
    m_has_socket( false ),
    m_ip( "" ),
    m_port( 0 ),
    m_has_remote( false ),
    m_remote_port( 0 )
{
}

MasterTag::~MasterTag( void )
{
}

const bool MasterTag::hasSocket( void ) const
{
    return( m_has_socket );
}

const std::string& MasterTag::ip( void ) const
{
    return( m_ip );
}

const int MasterTag::port( void ) const
{
    return( m_port );
}

const bool MasterTag::hasRemotePort( void ) const
{
    return( m_has_remote );
}

const int MasterTag::remotePort( void ) const
{
    return( m_remote_port );
}

void MasterTag::setSocket( const std::string& ip, const int port )
{
    m_has_socket = true;
    m_ip = ip;
    m_port = port;
}

void MasterTag::setRemotePort( const int port )
{
    m_has_remote = true;
    m_remote_port = port;
}

const bool MasterTag::read( const kvs::XMLNode::SuperClass* parent )
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

    // remote_port="xxx"
    const std::string remote_port = kvs::XMLElement::AttributeValue( element, "remote_port" );
    if ( remote_port != "" )
    {
        m_has_remote = true;
        m_remote_port = static_cast<int>( atoi( remote_port.c_str() ) );
    }

    return( true );
}

const bool MasterTag::write( kvs::XMLNode::SuperClass* parent )
{
    const std::string tag_name = BaseClass::name();
    kvs::XMLElement element( tag_name );

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

    if ( m_has_remote )
    {
        const std::string name( "remote_port" );
        const std::string value( kvs::String( m_remote_port ).toStdString() );
        element.setAttribute( name, value );
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
