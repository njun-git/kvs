/****************************************************************************/
/**
 *  @file Screen.h
 */
/*----------------------------------------------------------------------------
 *
 *  Copyright (c) Visualization Laboratory, Kyoto University.
 *  All rights reserved.
 *  See http://www.viz.media.kyoto-u.ac.jp/kvs/copyright/ for details.
 *
 *  $Id: Screen.h 634 2010-10-13 07:04:05Z naohisa.sakamoto $
 */
/****************************************************************************/
#ifndef KVS__TDW__SCREEN_H_INCLUDE
#define KVS__TDW__SCREEN_H_INCLUDE

#include <kvs/ClassName>
#include <kvs/ScreenBase>
#include <kvs/InitializeEventListener>
#include <kvs/PaintEventListener>
#include <kvs/ResizeEventListener>
#include <kvs/MousePressEventListener>
#include <kvs/MouseMoveEventListener>
#include <kvs/MouseReleaseEventListener>
#include <kvs/MouseDoubleClickEventListener>
#include <kvs/WheelEventListener>
#include <kvs/KeyPressEventListener>
#include <kvs/TimerEventListener>
#include <kvs/EventHandler>
#include <kvs/MouseEvent>
#include <kvs/KeyEvent>

#include "Timer.h"
#include "Application.h"


namespace kvs
{

namespace tdw
{

/*===========================================================================*/
/**
 *  @brief  GLUT screen class.
 */
/*===========================================================================*/
//class Screen : public kvs::glut::Window, public kvs::ScreenBase
class Screen : public kvs::ScreenBase
{
    typedef kvs::ScreenBase BaseClass;

    typedef void (Screen::*PaintEventFunction)( void );
    typedef void (Screen::*ResizeEventFunction)( int, int );
    typedef void (Screen::*MousePressEventFunction)( kvs::MouseEvent* );
    typedef void (Screen::*MouseMoveEventFunction)( kvs::MouseEvent* );
    typedef void (Screen::*MouseReleaseEventFunction)( kvs::MouseEvent* );
    typedef void (Screen::*WheelEventFunction)( kvs::WheelEvent* );
    typedef void (Screen::*KeyPressEventFunction)( kvs::KeyEvent* );

    kvsClassName( kvs::tdw::Screen );

protected:

    PaintEventFunction        m_paint_event_func;
    ResizeEventFunction       m_resize_event_func;
    MousePressEventFunction   m_mouse_press_event_func;
    MouseMoveEventFunction    m_mouse_move_event_func;
    MouseReleaseEventFunction m_mouse_release_event_func;
    WheelEventFunction        m_wheel_event_func;
    KeyPressEventFunction     m_key_press_event_func;

    kvs::tdw::Timer*             m_idle_mouse_timer;
    kvs::TimerEventListener*     m_idle_mouse_event_listener;
    std::list<kvs::tdw::Timer*>  m_timer_event_handler;
    kvs::EventHandler*           m_initialize_event_handler;

public:

    Screen( kvs::tdw::Application* application = 0 );

    virtual ~Screen( void );

public:

    void setPosition( const int x, const int y );

    void setSize( const int width, const int height );

    void setGeometry( const int x, const int y, const int width, const int height );

private:

    void set_position( const int x, const int y );

    void set_size( const int width, const int height );

public:

    virtual void createWindow( void );

    virtual void showFullScreen( void );

    virtual void showNormal( void );

    virtual void popUp( void );

    virtual void pushDown( void );

    virtual void hide( void );

    virtual void showWindow( void );

    virtual void redraw( void );

    virtual void resize( int width, int height );

public:

    virtual void initializeEvent( void );

    virtual void paintEvent( void );

    virtual void resizeEvent( int width, int height );

    virtual void mousePressEvent( kvs::MouseEvent* event );

    virtual void mouseMoveEvent( kvs::MouseEvent* event );

    virtual void mouseReleaseEvent( kvs::MouseEvent* event );

    virtual void mouseDoubleClickEvent( kvs::MouseEvent* event );

    virtual void wheelEvent( kvs::WheelEvent* event );

    virtual void keyPressEvent( kvs::KeyEvent* event );

public:

    virtual const std::pair<int,int> registerObject( kvs::ObjectBase* object, kvs::RendererBase* renderer = 0 );

    virtual const std::pair<int,int> registerObject( kvs::VisualizationPipeline* pipeline );


public:

    void setPaintEvent( kvs::PaintEventListener* event );

    void setResizeEvent( kvs::ResizeEventListener* event );

    void setMousePressEvent( kvs::MousePressEventListener* event );

    void setMouseMoveEvent( kvs::MouseMoveEventListener* event );

    void setMouseReleaseEvent( kvs::MouseReleaseEventListener* event );

    void setMouseDoubleClickEvent( kvs::MouseDoubleClickEventListener* event );

    void setWheelEvent( kvs::WheelEventListener* event );

    void setKeyPressEvent( kvs::KeyPressEventListener* event );

public:

    void addInitializeEvent( kvs::InitializeEventListener* event );

    void addPaintEvent( kvs::PaintEventListener* event );

    void addResizeEvent( kvs::ResizeEventListener* event );

    void addMousePressEvent( kvs::MousePressEventListener* event );

    void addMouseMoveEvent( kvs::MouseMoveEventListener* event );

    void addMouseReleaseEvent( kvs::MouseReleaseEventListener* event );

    void addMouseDoubleClickEvent( kvs::MouseDoubleClickEventListener* event );

    void addWheelEvent( kvs::WheelEventListener* event );

    void addKeyPressEvent( kvs::KeyPressEventListener* event );

    void addTimerEvent( kvs::TimerEventListener* event, kvs::tdw::Timer* timer );

public:

    int show( void );

public:

    void default_paint_event( void );

    void default_resize_event( int width, int height );

    void default_mouse_press_event( kvs::MouseEvent* event );

    void default_mouse_move_event( kvs::MouseEvent* event );

    void default_mouse_release_event( kvs::MouseEvent* event );

    void default_wheel_event( kvs::WheelEvent* event );

    void default_key_press_event( kvs::KeyEvent* event );

public:

    void idleMouseEvent( void );

private:

    // Callback functions for GLUT.

    friend void DisplayFunction( void );

    friend void ResizeFunction( int width, int height );

    friend void MouseFunction( int button, int state, int x, int y );

    friend void MouseMoveFunction( int x, int y );

    friend void KeyPressFunction( unsigned char key, int x, int y );

    friend void SpecialKeyPressFunction( int key, int x, int y );

};

} // end of namespace tdw

} // end of namespace kvs

#endif // KVS__TDW__SCREEN_H_INCLUDE
