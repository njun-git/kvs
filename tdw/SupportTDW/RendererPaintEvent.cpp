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
    m_mutex.lock();
    m_messages.clear();
    m_mutex.unlock();
}

void RendererPaintEvent::update( void )
{
    bool has_message = !m_messages.empty();
    while ( !has_message )
    {
        if ( !m_messages.empty() ) has_message = true;
        usleep(1);
    }

    if ( !m_messages.empty() )
    {
        std::list<kvs::MessageBlock>::iterator message_itr = m_messages.begin();
        std::list<kvs::MessageBlock>::iterator message_end = m_messages.end();
        while ( message_itr != message_end )
        {
            kvs::MessageBlock message = (*message_itr);

            kvs::tdw::MessageConverter converter;
            const std::string type = converter.messageType( message );
            kvs::EventBase* event = NULL;
            if ( type == KVS_TDW_PAINT_EVENT )      converter.applyPaintEvent( screen(), message );
            else if ( type == KVS_TDW_KEY_EVENT )   event = converter.keyEvent( message );
            else if ( type == KVS_TDW_TIMER_ENENT ) event = converter.timerEvent( message );

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
            message_itr++;
        }
        this->clearMessage();
    }

    screen()->redraw();
}

}

}
