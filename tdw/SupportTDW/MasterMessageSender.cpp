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

void MasterMessageSender::sendMessage( const kvs::MessageBlock& message )
{
    for ( size_t i = 0; i < m_addresses.size(); i++ )
    {
        kvs::Connector connector( m_addresses[i].ip(), m_addresses[i].port(), 50 );
        connector.send( message );
    }
}

void MasterMessageSender::sendPaintEvent( kvs::ScreenBase* screen )
{
    kvs::tdw::MessageConverter converter;
    this->sendMessage( converter.paintEventMessage( screen ) );
}

void MasterMessageSender::sendKeyEvent( kvs::KeyEvent* event )
{
    kvs::tdw::MessageConverter converter;
    this->sendMessage( converter.keyEventMessage( event ) );
}

void MasterMessageSender::sendTimerEvent( kvs::TimeEvent* event )
{
    kvs::tdw::MessageConverter converter;
    this->sendMessage( converter.timerEventMessage( event ) );
}

}

}
