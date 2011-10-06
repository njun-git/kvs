//
//  RendererReceiver.h
//  
//
//  Created by Jun Nishimura on 11/14/9.
//  Copyright 2011 Jun Nishimura. All rights reserved.
//

#ifndef KVS__TDW__RENDERER_RECEIVER_H_INCLUDE
#define KVS__TDW__RENDERER_RECEIVER_H_INCLUDE

#include <kvs/Thread>
#include <kvs/Mutex>

#include <list>
#include <kvs/Message>
#include <kvs/Acceptor>
#include <kvs/Connector>
#include <kvs/MessageBlock>

namespace kvs
{

namespace tdw
{

class RendererReceiver : public kvs::Thread
{

    kvsClassName( kvs::tdw::RendererReceiver );

protected:

    kvs::Acceptor                   m_acceptor;

     // Reference only.
    kvs::Mutex*                     m_mutex;
    std::list<kvs::MessageBlock>*   m_messages;

public:

    RendererReceiver( void );

    virtual ~RendererReceiver( void );

public:

    void setMutex( kvs::Mutex* mutex );

    void setMessages( std::list<kvs::MessageBlock>* messages );

public:

    void initialize( void );

    void run( void );

};

}

}

#endif
