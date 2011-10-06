//
//  RendererReceiver.cpp
//  
//
//  Created by Jun Nishimura on 11/14/9.
//  Copyright 2011 Jun Nishimura. All rights reserved.
//

#include "RendererReceiver.h"

#include "Application.h"
#include "Configuration.h"

namespace kvs
{

namespace tdw
{

RendererReceiver::RendererReceiver( void ) :
    m_mutex( NULL ),
    m_messages( NULL )
{
    this->initialize();
}

RendererReceiver::~RendererReceiver( void )
{
}

void RendererReceiver::setMutex( kvs::Mutex* mutex )
{
    m_mutex = mutex;
}

void RendererReceiver::setMessages( std::list<kvs::MessageBlock>* messages )
{
    m_messages = messages;
}

void RendererReceiver::initialize( void )
{
    if ( !kvs::tdw::Application::IsRenderer() )
    {
        kvsMessageError( "This node is not renderer." );
        exit( EXIT_FAILURE );
        return;
    }

    const int port = kvs::tdw::Configuration::Renderer().socket().port();
    if ( !m_acceptor.bind( port, 50 ) )
    {
        kvsMessageError( "Bind failed." );
        exit( EXIT_FAILURE );
    }
}

void RendererReceiver::run( void )
{
    kvs::MessageBlock message_block;
    for ( ; ; )
    {
        if ( m_acceptor.receive( &message_block ) )
        {
            m_mutex->lock();
            //if ( m_messages->empty() )
            m_messages->push_back( message_block );
            //if ( m_messages->size() > 3 ) m_messages->pop_front();
            m_mutex->unlock();
        }
    }
}

}

}
