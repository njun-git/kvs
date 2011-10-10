/*
 *  ControlScreen.cpp
 *  
 *
 *  Created by njun on 11/10/11.
 *  Copyright 2011 Jun Nishimura. All rights reserved.
 *
 */

#include "ControlScreen.h"

#include <kvs/Mouse>
#include <kvs/MouseButton>

namespace kvs
{

ControlScreen::ControlScreen( kvs::glut::Application* app ) :
    kvs::glut::Screen( app )
{
    this->initialize();
}

ControlScreen::~ControlScreen( void )
{
}

void ControlScreen::initialize( void )
{
    BaseClass::setTitle( "kvs::ControlScreen" );
    BaseClass::setGeometry( 512, 0, 512, 512 );

    m_screen = NULL;
    m_point = NULL;
}

void ControlScreen::attachMainScreen( kvs::MainScreen* screen )
{
    m_screen = screen;
    m_point = screen->seedPoint();
}

void ControlScreen::mousePressEvent( kvs::MouseEvent* event )
{
    m_mouse->setMode( kvs::Mouse::Translation );
    m_mouse->press( event->x(), event->y() );
}

void ControlScreen::mouseMoveEvent( kvs::MouseEvent* event )
{
    m_mouse->move( event->x(), event->y() );

    const kvs::Xform x = m_point->xform();
    const kvs::Vector3f coord( m_point->coords().pointer() );
    kvs::Vector3f translation = m_mouse->translation();

    if ( event->button() == kvs::MouseButton::Right ||
         event->modifiers() == kvs::Key::ShiftModifier )
    {
        translation.z() = - translation.y();
        translation.x() = 0.0f;
        translation.y() = 0.0f;
    }

    const kvs::Vector3f normalize = m_screen->objectManager()->normalize();
    translation.x() /= normalize.x() * x.scaling().x();
    translation.y() /= normalize.y() * x.scaling().y();
    translation.z() /= normalize.z() * x.scaling().z();

    const kvs::Vector3f new_coord = coord + translation * x.rotation();
    kvs::ValueArray<float> coords( 3 );
    coords[0] = new_coord.x();
    coords[1] = new_coord.y();
    coords[2] = new_coord.z();

    m_point->setCoords( coords );
    m_screen->updateStreamLine();

    BaseClass::redraw();
    m_screen->redraw();
}

void ControlScreen::mouseReleaseEvent( kvs::MouseEvent* event )
{
    m_mouse->release( event->x(), event->y() );
    BaseClass::redraw();
    m_screen->redraw();
}

}
