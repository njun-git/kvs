/*
 *  Timer.cpp
 *  
 *
 *  Created by njun on 11/07/09.
 *  Copyright 2011 Jun Nishimura. All rights reserved.
 *
 */

#include "Timer.h"
#include <map>
#include <kvs/DebugNew>
#include <kvs/glut/GLUT>


namespace
{
int counter = 0;
std::map<int,kvs::tdw::Timer*> context;
}

namespace kvs
{

namespace tdw
{

/*===========================================================================*/
/**
 *  @brief  Timer event function (GLUT's callback function).
 *  @param  id [in] timer ID
 */
/*===========================================================================*/
void TimerEventFunction( int id )
{
    ::context.find( id )->second->timerEvent();
}

/*===========================================================================*/
/**
 *  @brief  Constructs a new Timer class.
 *  @param  msec [in] interval time in milliseconds
 */
/*===========================================================================*/
Timer::Timer( int msec ):
    m_id( ::counter ),
    m_interval( msec ),
    m_is_stopped( true ),
    m_time_event( new kvs::TimeEvent ),
    m_event_listener( 0 ),
    m_event_handler( 0 ),
    m_send_renderer( true ),
    m_sender( NULL )
{
    ::counter++;
    ::context.insert( std::make_pair( m_id, this ) );
}

/*===========================================================================*/
/**
 *  @brief  Constructs a new Timer class.
 *  @param  listener [in] pointer to a event listener
 *  @param  msec [in] interval time in milliseconds
 */
/*===========================================================================*/
Timer::Timer( kvs::EventListener* listener, int msec ):
    m_id( ::counter ),
    m_interval( msec ),
    m_is_stopped( true ),
    m_time_event( new kvs::TimeEvent ),
    m_event_listener( listener ),
    m_event_handler( 0 ),
    m_send_renderer( true ),
    m_sender( NULL )
{
    ::counter++;
    ::context.insert( std::make_pair( m_id, this ) );
}

/*===========================================================================*/
/**
 *  @brief  Constructs a new Timer class.
 *  @param  handler [in] pointer to a event handler
 *  @param  msec [in] interval time in milliseconds
 */
/*===========================================================================*/
Timer::Timer( kvs::EventHandler* handler, int msec ):
    m_id( ::counter ),
    m_interval( msec ),
    m_is_stopped( true ),
    m_time_event( new kvs::TimeEvent ),
    m_event_listener( 0 ),
    m_event_handler( handler ),
    m_send_renderer( true ),
    m_sender( NULL )
{
    ::counter++;
    ::context.insert( std::make_pair( m_id, this ) );
}

/*===========================================================================*/
/**
 *  @brief  Destructs the Timer class.
 */
/*===========================================================================*/
Timer::~Timer( void )
{
    ::context.erase( m_id );
    if ( m_time_event ) delete m_time_event;
}

/*===========================================================================*/
/**
 *  @brief  Starts the timer.
 *  @param  msec [in] interval time in milliseconds
 */
/*===========================================================================*/
void Timer::start( int msec )
{
    if ( m_is_stopped )
    {
        m_interval = msec;
        m_is_stopped = false;
        glutTimerFunc( m_interval, TimerEventFunction, m_id );
    }
}

/*===========================================================================*/
/**
 *  @brief  Starts the timer.
 */
/*===========================================================================*/
void Timer::start( void )
{
    this->start( m_interval );
}

/*===========================================================================*/
/**
 *  @brief  Stops the timer.
 */
/*===========================================================================*/
void Timer::stop( void )
{
    /* WARNING: Cannot stop the timer due to the GLUT restriction.
     */
    m_is_stopped = true;
}

/*===========================================================================*/
/**
 *  @brief  Sets a interval time.
 *  @param  msec [in] interval time in milliseconds
 */
/*===========================================================================*/
void Timer::setInterval( int msec )
{
    m_interval = msec;
}

/*===========================================================================*/
/**
 *  @brief  Sets an event listener.
 *  @param  listener [in] pointer to the event listener
 */
/*===========================================================================*/
void Timer::setEventListener( kvs::EventListener* listener )
{
    m_event_listener = listener;
}

/*===========================================================================*/
/**
 *  @brief  Sets an event handler.
 *  @param  handler [in] pointer to the event handler
 */
/*===========================================================================*/
void Timer::setEventHandler( kvs::EventHandler* handler )
{
    m_event_handler = handler;
}

void Timer::setMasterMessageSender( kvs::tdw::MasterMessageSender* sender )
{
    m_sender = sender;
}

void Timer::setSendRenderer( const bool send_renderer )
{
    m_send_renderer = send_renderer;
}

const bool Timer::sendRenderer( void ) const
{
    return( m_send_renderer );
}

/*===========================================================================*/
/**
 *  @brief  Checks whether the timer is stopped or not.
 *  @return true, if the timer is stopped
 */
/*===========================================================================*/
const bool Timer::isStopped( void ) const
{
    return( m_is_stopped );
}

/*===========================================================================*/
/**
 *  @brief  Returns the interval time.
 *  @return interval time
 */
/*===========================================================================*/
const int Timer::interval( void ) const
{
    return( m_interval );
}

/*===========================================================================*/
/**
 *  @brief  Timer event.
 */
/*===========================================================================*/
void Timer::timerEvent( void )
{
    if ( !m_is_stopped )
    {
        if ( m_event_listener ) m_event_listener->onEvent( m_time_event );
        else if ( m_event_handler ) m_event_handler->notify( m_time_event );
        glutTimerFunc( m_interval, TimerEventFunction, m_id );

        if ( m_sender && m_send_renderer )
        {
            m_sender->sendTimerEvent( m_time_event );
        }
    }
    else
    {
        this->stop();
    }
}

} // end of namespace glut

} // end of namespace kvs
