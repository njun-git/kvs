//
//  DigitalClockRenderer.cpp
//  
//
//  Created by Jun Nishimura on 11/24/8.
//  Copyright 2011 Jun Nishimura. All rights reserved.
//

#include "DigitalClockRenderer.h"

#include <kvs/OpenGL>
#include <kvs/Camera>
#include <kvs/Light>
#include <kvs/ObjectBase>
#include <kvs/IgnoreUnusedVariable>

namespace kvs
{

DigitalClockRenderer::DigitalClockRenderer( void ) :
    m_enable_anti_aliasing( true ),
    m_enable_multisample_anti_aliasing( true ),
    m_enable_two_side_lighting( true )
{
}

DigitalClockRenderer::~DigitalClockRenderer( void )
{
}

void DigitalClockRenderer::exec( kvs::ObjectBase* object, kvs::Camera* camera, kvs::Light* light )
{
    kvs::IgnoreUnusedVariable( light );
    kvs::IgnoreUnusedVariable( camera );

    kvs::DigitalClockObject* clock = reinterpret_cast<kvs::DigitalClockObject*>( object );

    glPushAttrib( GL_CURRENT_BIT | GL_ENABLE_BIT );

    RendererBase::initialize();
    clock->applyMaterial();

    // Anti-aliasing.
    if ( m_enable_anti_aliasing )
    {
#if defined ( GL_MULTISAMPLE )
        if ( m_enable_multisample_anti_aliasing )
        {
            GLint buffers = 0;
            GLint samples = 0;
            glGetIntegerv( GL_SAMPLE_BUFFERS, &buffers );
            glGetIntegerv( GL_SAMPLES, &samples );
            if ( buffers > 0 && samples > 1 ) glEnable( GL_MULTISAMPLE );
        }
        else
#endif
        {
            glEnable( GL_POLYGON_SMOOTH );
            glEnable( GL_BLEND );
            glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
            glHint( GL_POLYGON_SMOOTH_HINT, GL_NICEST );
        }
    }

    glEnable( GL_DEPTH_TEST );
    {
        kvs::RGBColor on_color = clock->onColor();
        kvs::RGBColor off_color = clock->offColor();
        glNormal3f( 0.0f, 0.0f, 1.0f );
        for ( size_t i = 0; i < 4; i++ )
        {
            const kvs::SevenSegmentLED led = clock->led(i);
            for ( int j = 0; j < 7; j++ )
            {
                if ( led.segment(j).isOn() )
                {
                    glColor4ub( on_color.r(), on_color.g(), on_color.b(), 255 );
                }
                else
                {
                    glColor4ub( off_color.r(), off_color.g(), off_color.b(), 255 );
                }
                glEnableClientState( GL_VERTEX_ARRAY );
                glVertexPointer( 3, GL_FLOAT, 0, led.segment(j).coords().pointer() );
                glDrawArrays( GL_TRIANGLE_STRIP, 0, 6 );
                glDisableClientState( GL_VERTEX_ARRAY );
            }
        }
        if ( clock->colon().isOn() )
        {
            glColor4ub( on_color.r(), on_color.g(), on_color.b(), 255 );
        }
        else
        {
            glColor4ub( off_color.r(), off_color.g(), off_color.b(), 255 );
        }
        glEnableClientState( GL_VERTEX_ARRAY );
        glVertexPointer( 3, GL_FLOAT, 0, clock->colon().coords().pointer() );
        glDrawArrays( GL_QUADS, 0, 8 );
        glDisableClientState( GL_VERTEX_ARRAY );
    }
    glDisable( GL_DEPTH_TEST );

    glPopAttrib();
}

void DigitalClockRenderer::enableAntiAliasing( const bool multisample ) const
{
    m_enable_anti_aliasing = true;
    m_enable_multisample_anti_aliasing = multisample;
}

void DigitalClockRenderer::disableAntiAliasing( void ) const
{
    m_enable_anti_aliasing = false;
    m_enable_multisample_anti_aliasing = false;
}

void DigitalClockRenderer::enableTwoSideLighting( void ) const
{
    m_enable_two_side_lighting = true;
}

void DigitalClockRenderer::disableTwoSideLighting( void ) const
{
    m_enable_two_side_lighting = false;
}

const bool DigitalClockRenderer::isTwoSideLighting( void ) const
{
    return( m_enable_two_side_lighting );
}

void DigitalClockRenderer::initialize_projection( void )
{
   glMatrixMode( GL_PROJECTION );

   glMatrixMode( GL_MODELVIEW );
}

void DigitalClockRenderer::initialize_modelview( void )
{
    glDisable( GL_LINE_SMOOTH );

    glEnable( GL_BLEND );
    glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

    glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );

    glShadeModel( GL_SMOOTH );

    glColorMaterial( GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE );
    glEnable( GL_COLOR_MATERIAL );
/*
    if( !this->isShading() )
    {
        glDisable( GL_NORMALIZE );
        glDisable( GL_LIGHTING );
    }
    else
    {
        glEnable( GL_NORMALIZE );
        glEnable( GL_LIGHTING );
    }
*/
    glDisable( GL_LIGHTING );

    kvs::Light::setModelTwoSide( this->isTwoSideLighting() );
}

}
