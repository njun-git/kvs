//
//  MasterReceiver.cpp
//  
//
//  Created by Jun Nishimura on 11/7/10.
//  Copyright 2011 Jun Nishimura. All rights reserved.
//

#include "MasterReceiver.h"

#include "Application.h"
#include "Configuration.h"
#include "MessageConverter.h"

namespace kvs
{

namespace tdw
{

MasterReceiver::MasterReceiver( kvs::ScreenBase* screen ) :
    m_screen( screen ),
    m_sender( NULL ),
    m_handler( NULL )
{
    this->initialize();
}

MasterReceiver::~MasterReceiver( void )
{
    m_screen = NULL;
    m_sender = NULL;
    m_handler = NULL;
}

void MasterReceiver::attachMasterMessageSender( kvs::tdw::MasterMessageSender* sender )
{
    m_sender = sender;
}

void MasterReceiver::attachEventHandler( kvs::EventHandler* handler )
{
    m_handler = handler;
}

void MasterReceiver::initialize( void )
{
    if ( !kvs::tdw::Application::IsMaster() )
    {
        kvsMessageError( "This node is not master." );
        exit( EXIT_FAILURE );
        return;
    }

    if ( !m_screen )
    {
        kvsMessageError( "kvs::ScreenBase* screen is not set to kvs::tdw::MasterReceiver." );
        exit( EXIT_FAILURE );
        return;
    }

    const int port = kvs::tdw::Configuration::RemotePort();
    if ( !m_acceptor.bind( port, 50 ) )
    {
        kvsMessageError( "Cannot bind port <%d>.", port );
        exit( EXIT_FAILURE );
    }
}

void MasterReceiver::run( void )
{
    kvs::MessageBlock message_block;
    for ( ; ; )
    {
        if ( m_acceptor.receive( &message_block ) )
        {
            kvs::tdw::MessageConverter converter;
            const std::string type = converter.messageType( message_block );
            if ( type == KVS_TDW_STACK_EVENT )
            {
                std::vector<kvs::MessageBlock> stack = converter.eventStack( message_block );
                for ( size_t i = 0; i < stack.size(); i++ )
                {
                    kvs::EventBase* event = NULL;
                    const std::string event_type = converter.messageType( stack[i] );
                    if ( event_type == KVS_TDW_PAINT_EVENT )      converter.applyPaintEvent( m_screen, stack[i] );
                    else if ( event_type == KVS_TDW_KEY_EVENT )   event = converter.keyEvent( stack[i] );
                    else if ( event_type == KVS_TDW_TIMER_EVENT ) event = converter.timerEvent( stack[i] );

                    if ( m_handler )
                    {
                        if ( event )
                        {
                            m_handler->notify( event );
                        }
                        else
                        {
                            m_handler->notify();
                        }
                    }
                    if ( event ) delete event;
                }
            }
            else
            {
                kvs::EventBase* event = NULL;
                if ( type == KVS_TDW_PAINT_EVENT )      converter.applyPaintEvent( m_screen, message_block );
                else if ( type == KVS_TDW_KEY_EVENT )   event = converter.keyEvent( message_block );
                else if ( type == KVS_TDW_TIMER_EVENT ) event = converter.timerEvent( message_block );

                if ( m_handler )
                {
                    if ( event )
                    {
                        m_handler->notify( event );
                    }
                    else
                    {
                        m_handler->notify();
                    }
                }
                if ( event ) delete event;
            }
            if ( m_sender ) m_sender->sendMessage( message_block );
        }
    }
}

}

}
