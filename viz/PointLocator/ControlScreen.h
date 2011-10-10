/*
 *  ControlScreen.h
 *  
 *
 *  Created by njun on 11/10/11.
 *  Copyright 2011 Jun Nishimura. All rights reserved.
 *
 */

#ifndef KVS__CONTROL_SCREEN_H_INCLUDE
#define KVS__CONTROL_SCREEN_H_INCLUDE

#include <kvs/glut/Application>
#include <kvs/glut/Screen>

#include "MainScreen.h"

namespace kvs
{

class ControlScreen : public kvs::glut::Screen
{

    // Class name.
    kvsClassName( kvs::ControlScreen );

    // Module information.
    kvsModuleBaseClass( kvs::glut::Screen );

protected:

    // Reference only.
    kvs::MainScreen*    m_screen;
    kvs::PointObject*   m_point;

public:

    ControlScreen( kvs::glut::Application* app );

    virtual ~ControlScreen( void );

public:

    void initialize( void );

    void attachMainScreen( kvs::MainScreen* screen );

public:

    void mousePressEvent( kvs::MouseEvent* event );

    void mouseMoveEvent( kvs::MouseEvent* event );

    void mouseReleaseEvent( kvs::MouseEvent* event );

};

}

#endif
