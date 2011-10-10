/*
 *  PaintScreen.h
 *  
 *
 *  Created by njun on 11/10/10.
 *  Copyright 2011 Jun Nishimura. All rights reserved.
 *
 */

#ifndef KVS__PAINT_SCREEN_H_INCLUDE
#define KVS__PAINT_SCREEN_H_INCLUDE

#include <kvs/glut/Application>
#include <kvs/glut/Screen>

#include <kvs/ClassName>
#include <kvs/Module>

#include "PaintImageObject.h"
#include "PaintImageRenderer.h"

namespace kvs
{

class PaintScreen : public kvs::glut::Screen
{

    // Class name.
    kvsClassName( kvs::PaintScreen );

    // Module information.
    kvsModuleBaseClass( kvs::glut::Screen );

protected:

    kvs::ScreenBase*            m_other_screen;
    kvs::PaintImageObject*      m_image;
    kvs::PaintImageRenderer*    m_renderer;

public:

    PaintScreen( kvs::glut::Application* app );

    virtual ~PaintScreen( void );

public:

    void initialize( kvs::glut::Application* app );

    kvs::PaintImageObject* image( void );

    void attachScreen( kvs::ScreenBase* screen );

public:

    void keyPressEvent( kvs::KeyEvent* event );

    void mousePressEvent( kvs::MouseEvent* event );

    void mouseMoveEvent( kvs::MouseEvent* event );

    void mouseReleaseEvent( kvs::MouseEvent* event );

};

}

#endif
