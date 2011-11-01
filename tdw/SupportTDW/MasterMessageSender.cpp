//
//  MasterMessageSender.cpp
//  
//
//  Created by Jun Nishimura on 11/13/9.
//  Copyright 2011 Jun Nishimura. All rights reserved.
//

#include "MasterMessageSender.h"

#include "MessageConverter.h"

namespace kvs
{

namespace tdw
{

MasterMessageSender::MasterMessageSender( kvs::ScreenBase* screen ) :
    m_screen( screen )
{
    this->initialize();
}

MasterMessageSender::~MasterMessageSender( void )
{
}

void MasterMessageSender::initialize( void )
{
    if ( !kvs::tdw::Application::IsMaster() )
    {
        kvsMessageError( "This node is not master." );
        exit( EXIT_FAILURE );
    }

    std::vector<kvs::tdw::Renderer> list = kvs::tdw::Configuration::RendererList();
    m_addresses.clear();
    for ( size_t i = 0; i < list.size(); i++ )
    {
        m_addresses.push_back( list[i].socket() );
    }
}

kvs::Mutex& MasterMessageSender::mutex( void )
{
    return( m_mutex );
}

void MasterMessageSender::sendMessage( const kvs::MessageBlock& message )
{
    m_mutex.lock();
    const bool is_rendering = kvs::tdw::Configuration::IsRendering();
    m_mutex.unlock();
    if ( is_rendering && kvs::tdw::Configuration::IsSync() )
    {
        this->stackMessage( message );
    }
    else
    {
        this->send_message( message, true );
    }
}

void MasterMessageSender::sendStackMessage( void )
{
    if ( !m_stack.empty() )
    {
        kvs::tdw::MessageConverter converter;
        this->send_message( converter.eventStackMessage( m_stack ), false );
        this->clearStack();
    }
}

void MasterMessageSender::stackMessage( const kvs::MessageBlock& message )
{
    m_mutex.lock();
    m_stack.push_back( message );
    m_mutex.unlock();
}

void MasterMessageSender::clearStack( void )
{
    m_mutex.lock();
    m_stack.clear();
    m_mutex.unlock();
}

void MasterMessageSender::sendPaintEvent( kvs::ScreenBase* screen )
{
    kvs::tdw::MessageConverter converter;
    m_mutex.lock();
    const bool is_rendering = kvs::tdw::Configuration::IsRendering();
    m_mutex.unlock();
    if ( is_rendering && kvs::tdw::Configuration::IsSync() )
    {
        this->stackMessage( converter.paintEventMessage( screen ) );
    }
    else
    {
        this->sendMessage( converter.paintEventMessage( screen ) );
    }
}

void MasterMessageSender::sendKeyEvent( kvs::KeyEvent* event )
{
    kvs::tdw::MessageConverter converter;
    m_mutex.lock();
    const bool is_rendering = kvs::tdw::Configuration::IsRendering();
    m_mutex.unlock();
    if ( is_rendering && kvs::tdw::Configuration::IsSync() )
    {
        this->stackMessage( converter.keyEventMessage( event ) );
    }
    else
    {
        this->sendMessage( converter.keyEventMessage( event ) );
    }
}

void MasterMessageSender::sendTimerEvent( kvs::TimeEvent* event )
{
    kvs::tdw::MessageConverter converter;
    m_mutex.lock();
    const bool is_rendering = kvs::tdw::Configuration::IsRendering();
    m_mutex.unlock();
    if ( is_rendering && kvs::tdw::Configuration::IsSync() )
    {
        this->stackMessage( converter.timerEventMessage( event ) );
    }
    else
    {
        this->sendMessage( converter.timerEventMessage( event ) );
    }
}

void MasterMessageSender::send_message( const kvs::MessageBlock& message, bool change )
{
    for ( size_t i = 0; i < m_addresses.size(); i++ )
    {
        kvs::Connector connector( m_addresses[i].ip(), m_addresses[i].port(), 1 );
        connector.send( message );
    }

    if ( change )
    {
        m_mutex.lock();
        kvs::tdw::Configuration::setIsRendering( true );
        m_mutex.unlock();
    }
}

}

}
