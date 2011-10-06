//
//  MasterMessageSender.h
//  
//
//  Created by Jun Nishimura on 11/13/9.
//  Copyright 2011 Jun Nishimura. All rights reserved.
//

#ifndef KVS__TDW__MASTER_MESSAGE_SENDER_H_INCLUDE
#define KVS__TDW__MASTER_MESSAGE_SENDER_H_INCLUDE

#include <kvs/EventListener>
#include <kvs/Message>
#include <kvs/Connector>

#include "Application.h"
#include "Configuration.h"

namespace kvs
{

namespace tdw
{

class MasterMessageSender
{

    kvsClassName( kvs::tdw::MasterMessageSender );

protected:

    // Reference only.
    kvs::ScreenBase*    m_screen;

    // To send messages to renderers.
    std::vector<kvs::SocketAddress> m_addresses;

public:

    MasterMessageSender( kvs::ScreenBase* screen );

    virtual ~MasterMessageSender( void );

public:

    void initialize( void );

    void sendMessage( const kvs::MessageBlock& message );

    void sendPaintEvent( kvs::ScreenBase* screen = 0 );

    void sendKeyEvent( kvs::KeyEvent* event );

    void sendTimerEvent( kvs::TimeEvent* event );

};

}

}

#endif
