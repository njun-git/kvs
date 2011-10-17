/*
 *  Timer.h
 *  
 *
 *  Created by njun on 11/07/09.
 *  Copyright 2011 Jun Nishimura. All rights reserved.
 *
 */

#ifndef KVS__TDW__TIMER_H_INCLUDE
#define KVS__TDW__TIMER_H_INCLUDE

#include <kvs/ClassName>
#include <kvs/EventListener>
#include <kvs/EventHandler>
#include <kvs/TimeEvent>

#include "MasterMessageSender.h"

namespace kvs
{

namespace tdw
{

class Timer
{
    kvsClassName( kvs::tdw::Timer );

protected:

    int                 m_id;             ///< timer ID
    int                 m_interval;       ///< interval time in millisecond
    bool                m_is_stopped;     ///< checl flag whether the time is stopped
    kvs::TimeEvent*     m_time_event;     ///< time event
    kvs::EventListener* m_event_listener; ///< event listener
    kvs::EventHandler*  m_event_handler;  ///< event handler

    bool                m_send_renderer;
    kvs::tdw::MasterMessageSender*  m_sender; // Reference only.

public:

    Timer( int msec = 0 );

    Timer( kvs::EventListener* listener, int msec = 0 );

    Timer( kvs::EventHandler* handler, int msec = 0 );

    virtual ~Timer( void );

    void start( int msec );

    void start( void );

    void stop( void );

    void setInterval( int msec );

    void setEventListener( kvs::EventListener* listener );

    void setEventHandler( kvs::EventHandler* handler );

    void setMasterMessageSender( kvs::tdw::MasterMessageSender* sender );

    void setSendRenderer( const bool send_renderer );

    const bool sendRenderer( void ) const;

    const bool isStopped( void ) const;

    const int interval( void ) const;

protected:

    void timerEvent( void );

    friend void TimerEventFunction( int id );

};

}

}

#endif
