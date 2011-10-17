//
//  MasterReceiver.h
//  
//
//  Created by Jun Nishimura on 11/7/10.
//  Copyright 2011 Jun Nishimura. All rights reserved.
//

#ifndef KVS__TDW__MASTER_RECEIVER_H_INCLUDE
#define KVS__TDW__MASTER_RECEIVER_H_INCLUDE

#include <kvs/Thread>

#include <kvs/Message>
#include <kvs/Acceptor>
#include <kvs/Connector>
#include <kvs/MessageBlock>

#include "Screen.h"
#include "MasterMessageSender.h"

namespace kvs
{

namespace tdw
{

class MasterReceiver : public kvs::Thread
{

    kvsClassName( kvs::tdw::MasterReceiver );

protected:

    kvs::Acceptor                   m_acceptor;

    // Reference only.
    kvs::ScreenBase*                m_screen;
    kvs::tdw::MasterMessageSender*  m_sender;
    kvs::EventHandler*              m_handler;

public:

    MasterReceiver( kvs::ScreenBase* screen );

    virtual ~MasterReceiver( void );

public:

    void attachMasterMessageSender( kvs::tdw::MasterMessageSender* sender );

    void attachEventHandler( kvs::EventHandler* handler );

public:

    void initialize( void );

    void run( void );

};

}

}

#endif
