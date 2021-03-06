//
//  RendererPaintEvent.cpp
//  
//
//  Created by Jun Nishimura on 11/14/9.
//  Copyright 2011 Jun Nishimura. All rights reserved.
//

#include "RendererPaintEvent.h"

#include "Application.h"
#include "Configuration.h"

namespace kvs
{

namespace tdw
{

RendererPaintEvent::RendererPaintEvent( void ) :
    kvs::PaintEventListener(),
    m_receiver( NULL )
{
    this->initialize();
}

RendererPaintEvent::~RendererPaintEvent( void )
{
    if ( m_receiver ) delete m_receiver;
}

void RendererPaintEvent::initialize( void )
{
    if ( !kvs::tdw::Application::IsRenderer() )
    {
        kvsMessageError( "This node is not renderer." );
        return;
    }

    m_receiver = new kvs::tdw::RendererReceiver();
    m_receiver->setMutex( &m_mutex );
    m_receiver->setMessages( &m_messages );
    m_receiver->start();
}

void RendererPaintEvent::attach( kvs::EventListener* listener )
{
    m_listeners.push_back( listener );
}

void RendererPaintEvent::clearMessage( void )
{
    m_messages.clear();
}

void RendererPaintEvent::update( void )
{
    bool has_message = !m_messages.empty();
    while ( !has_message )
    {
        if ( !m_messages.empty() ) has_message = true;
        usleep(1);
    }

    m_mutex.lock();
    if ( !m_messages.empty() )
    {
        std::list<kvs::MessageBlock>::iterator message_itr = m_messages.begin();
        std::list<kvs::MessageBlock>::iterator message_end = m_messages.end();
        while ( message_itr != message_end )
        {
            kvs::MessageBlock message = (*message_itr);

            kvs::tdw::MessageConverter converter;
            const std::string type = converter.messageType( message );
            if ( type == KVS_TDW_STACK_EVENT )
            {
                std::vector<kvs::MessageBlock> stack = converter.eventStack( message );
                for ( size_t i = 0; i < stack.size(); i++ )
                {
                    kvs::EventBase* event = NULL;
                    const std::string event_type = converter.messageType( stack[i] );
                    if ( event_type == KVS_TDW_PAINT_EVENT )      converter.applyPaintEvent( screen(), stack[i] );
                    else if ( event_type == KVS_TDW_KEY_EVENT )   event = converter.keyEvent( stack[i] );
                    else if ( event_type == KVS_TDW_TIMER_EVENT ) event = converter.timerEvent( stack[i] );

                    std::list<kvs::EventListener*>::iterator listener = m_listeners.begin();
                    std::list<kvs::EventListener*>::iterator end = m_listeners.end();

                    while ( listener != end )
                    {
                        if ( (*listener)->eventType() & kvs::EventBase::PaintEvent )
                        {
                            (*listener)->onEvent();
                        }
                        else if ( event )
                        {
                            if ( (*listener)->eventType() & event->type() )
                            {
                                (*listener)->onEvent( event );
                            }
                        }

                        ++listener;
                    }
                    if ( event ) delete event;
                }
            }
            else
            {
                kvs::EventBase* event = NULL;
                if ( type == KVS_TDW_PAINT_EVENT )      converter.applyPaintEvent( screen(), message );
                else if ( type == KVS_TDW_KEY_EVENT )   event = converter.keyEvent( message );
                else if ( type == KVS_TDW_TIMER_EVENT ) event = converter.timerEvent( message );

                std::list<kvs::EventListener*>::iterator listener = m_listeners.begin();
                std::list<kvs::EventListener*>::iterator end = m_listeners.end();

                while ( listener != end )
                {
                    if ( (*listener)->eventType() & kvs::EventBase::PaintEvent )
                    {
                        (*listener)->onEvent();
                    }
                    else if ( event )
                    {
                        if ( (*listener)->eventType() & event->type() )
                        {
                            (*listener)->onEvent( event );
                        }
                    }

                    ++listener;
                }
                if ( event ) delete event;
            }
            message_itr++;
        }
        this->clearMessage();
    }
    m_mutex.unlock();

    screen()->redraw();
}

}

}
