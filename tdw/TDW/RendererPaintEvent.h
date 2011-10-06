//
//  RendererPaintEvent.h
//  
//
//  Created by Jun Nishimura on 11/14/9.
//  Copyright 2011 Jun Nishimura. All rights reserved.
//

#ifndef KVS__TDW__RENDERER_PAINT_EVENT_H_INCLUDE
#define KVS__TDW__RENDERER_PAINT_EVENT_H_INCLUDE

#include <kvs/PaintEventListener>
#include <kvs/EventListener>
#include <kvs/Message>
#include <kvs/Mutex>
#include <list>

#include "MessageConverter.h"
#include "RendererReceiver.h"

namespace kvs
{

namespace tdw
{

class RendererPaintEvent : public kvs::PaintEventListener
{
    kvsClassName( kvs::tdw::RendererPaintEvent );

protected:

    std::list<kvs::EventListener*>  m_listeners;

    kvs::Mutex                      m_mutex;
    std::list<kvs::MessageBlock>    m_messages;

    kvs::tdw::RendererReceiver*     m_receiver;

public:

    RendererPaintEvent( void );

    virtual ~RendererPaintEvent( void );

public:

    void initialize( void );

    void attach( kvs::EventListener* listener );

    void clearMessage( void );

public:

    void update( void );

};

}

}

#endif
