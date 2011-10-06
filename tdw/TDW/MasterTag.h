/*
 *  MasterTag.h
 *  
 *
 *  Created by njun on 11/07/09.
 *  Copyright 2011 Jun Nishimura. All rights reserved.
 *
 */

#ifndef KVS__KVSML__MASTER_TAG_H_INCLUDE
#define KVS__KVSML__MASTER_TAG_H_INCLUDE

#include <kvs/XMLNode>
#include <Core/FileFormat/KVSML/TagBase.h>

namespace kvs
{

namespace kvsml
{

class MasterTag : public kvs::kvsml::TagBase
{
public:

    typedef kvs::kvsml::TagBase BaseClass;

protected:

    bool m_has_socket;
    std::string m_ip;
    int m_port;

    bool m_has_remote;
    int m_remote_port;

public:

    MasterTag( void );

    virtual ~MasterTag( void );

public:

    const bool hasSocket( void ) const;

    const std::string& ip( void ) const;

    const int port( void ) const;

    const bool hasRemotePort( void ) const;

    const int remotePort( void ) const;

public:

    void setSocket( const std::string& ip, const int port );

    void setRemotePort( const int port );

public:

    const bool read( const kvs::XMLNode::SuperClass* parent );

    const bool write( kvs::XMLNode::SuperClass* parent );
};

} // end of namespace kvsml

} // end of namespace kvs

#endif
