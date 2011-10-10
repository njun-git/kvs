/*
 *  PaintScreen.cpp
 *  
 *
 *  Created by njun on 11/10/10.
 *  Copyright 2011 Jun Nishimura. All rights reserved.
 *
 */

#include "PaintScreen.h"

#include <kvs/Key>
#include <kvs/MouseButton>

namespace kvs
{

PaintScreen::PaintScreen( kvs::glut::Application* app ) :
    kvs::glut::Screen( app )
{
    this->initialize( app );
}

PaintScreen::~PaintScreen( void )
{
}

void PaintScreen::initialize( kvs::glut::Application* app )
{
    BaseClass::setTitle( "kvs::PaintScreen" );

    m_other_screen = NULL;

    int argc = app->argc();
    char** argv = app->argv();

    if ( argc > 1 ) m_image = new kvs::PaintImageObject( std::string( argv[1] ) );
    else            m_image = new kvs::PaintImageObject( 512, 512 );
    m_renderer = new kvs::PaintImageRenderer();
    BaseClass::registerObject( m_image, m_renderer );

    BaseClass::setGeometry( 512, 0, m_image->width(), m_image->height() );
}

kvs::PaintImageObject* PaintScreen::image( void )
{
    return( m_image );
}

void PaintScreen::attachScreen( kvs::ScreenBase* screen )
{
    m_other_screen = screen;
}

void PaintScreen::keyPressEvent( kvs::KeyEvent* event )
{
    BaseClass::keyPressEvent( event );

    switch ( event->key() )
    {
        case kvs::Key::s:
        {
            if ( m_image->write( "image.bmp" ) )
            {
                std::cout << "Write image to image.bmp" << std::endl;
            }
            break;
        }
        default: break;
    }
}

void PaintScreen::mousePressEvent( kvs::MouseEvent* event )
{
}

void PaintScreen::mouseMoveEvent( kvs::MouseEvent* event )
{
    kvs::Vector2f factor(
        (float)m_image->width() / BaseClass::width(),
        (float)m_image->height() / BaseClass::height() );
    int x = event->x() * factor.x();
    int y = event->y() * factor.y();
    if ( x < 0 ) x = 0; if ( x >= (int)m_image->width() )  x = (int)m_image->width() - 1;
    if ( y < 0 ) y = 0; if ( y >= (int)m_image->height() ) y = (int)m_image->height() - 1;

    float alpha = 0.125f * 0.125f;
    if ( event->button() == kvs::MouseButton::Left )
    {
        switch ( event->modifiers() )
        {
            case kvs::Key::ShiftModifier:
                alpha *= - 1.0f; break;
            case kvs::Key::ControlModifier:
                alpha *= - 1.0f; break;
            default: break;
        }
    }
    else
    {
        alpha *= -1.0f;
    }

    int radius = 500;
    for ( int xx = - radius; xx < radius; xx++ )
    {
        for ( int yy = - radius; yy < radius; yy++ )
        {
            if ( x + xx >= 0 && x + xx < (int)m_image->width() &&
                 y + yy >= 0 && y + yy < (int)m_image->height() )
            {
                kvs::Vector2i l( xx, yy );
                const float length = l.length();
                if ( length < (float)radius )
                {
                    m_image->value( x + xx, y + yy ) += radius * alpha / ( radius + length * length );
                    m_image->value( x + xx, y + yy ) = kvs::Math::Clamp( m_image->value( x + xx, y + yy ), 0.0f, 1.0f );
                }
            }
        }
    }

    BaseClass::redraw();
    if ( m_other_screen ) m_other_screen->redraw();
}

void PaintScreen::mouseReleaseEvent( kvs::MouseEvent* event )
{
}


}
