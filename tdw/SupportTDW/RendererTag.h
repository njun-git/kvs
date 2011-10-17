/*
 *  RendererTag.h
 *  
 *
 *  Created by njun on 11/07/09.
 *  Copyright 2011 Jun Nishimura. All rights reserved.
 *
 */

#ifndef KVS__KVSML__RENDERER_TAG_H_INCLUDE
#define KVS__KVSML__RENDERER_TAG_H_INCLUDE

#include <kvs/XMLNode>
#include <kvs/XMLElement>
#include <Core/FileFormat/KVSML/TagBase.h>
#include <kvs/Vector2>

namespace kvs
{

namespace kvsml
{

class RendererTag : public kvs::kvsml::TagBase
{
public:

    typedef kvs::kvsml::TagBase BaseClass;

protected:

    bool m_has_name;
    std::string m_node_name;

    bool m_has_socket;
    std::string m_ip;
    int m_port;

    bool m_has_origin;
    kvs::Vector2i m_origin;

    bool m_has_size;
    kvs::Vector2i m_size;

public:

    RendererTag( void );

    virtual ~RendererTag( void );

public:

    const bool hasName( void ) const;

    const std::string& nodeName( void ) const;

    const bool hasSocket( void ) const;

    const std::string& ip( void ) const;

    const int port( void ) const;

    const bool hasOrigin( void ) const;

    const kvs::Vector2i& origin( void ) const;

    const bool hasSize( void ) const;

    const kvs::Vector2i& size( void ) const;

public:

    void setNodeName( const std::string& name );

    void setSocket( const std::string& ip, const int port );

    void setOrigin( const kvs::Vector2i& origin );

    void setSize( const kvs::Vector2i& size );

public:

    const bool read( const kvs::XMLNode::SuperClass* parent );

    const bool read( const kvs::XMLElement::SuperClass* element );

    const bool write( kvs::XMLNode::SuperClass* parent );
};

} // end of namespace kvsml

} // end of namespace kvs

#endif
