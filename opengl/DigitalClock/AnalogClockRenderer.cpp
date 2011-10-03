//
//  AnalogClockRenderer.cpp
//  
//
//  Created by Jun Nishimura on 11/25/8.
//  Copyright 2011 Jun Nishimura. All rights reserved.
//

#include "AnalogClockRenderer.h"

#include <kvs/OpenGL>
#include <kvs/Camera>
#include <kvs/Light>
#include <kvs/ObjectBase>
#include <kvs/IgnoreUnusedVariable>

namespace kvs
{

AnalogClockRenderer::AnalogClockRenderer( void ) :
    m_enable_anti_aliasing( true ),
    m_enable_multisample_anti_aliasing( true ),
    m_enable_two_side_lighting( true )
{
}

AnalogClockRenderer::~AnalogClockRenderer( void )
{
}

void AnalogClockRenderer::exec( kvs::ObjectBase* object, kvs::Camera* camera, kvs::Light* light )
{
    kvs::IgnoreUnusedVariable( light );
    kvs::IgnoreUnusedVariable( camera );

    kvs::AnalogClockObject* clock = reinterpret_cast<kvs::AnalogClockObject*>( object );

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
        kvs::RGBColor color = clock->color();
        glNormal3f( 0.0f, 0.0f, 1.0f );
        glColor4ub( color.r(), color.g(), color.b(), 255 );
        for ( size_t i = 0; i < 60; i++ )
        {
            const kvs::PolygonParticle particle = clock->particle(i);

            glEnableClientState( GL_VERTEX_ARRAY );
            glVertexPointer( 3, GL_FLOAT, 0, particle.coords().pointer() );
            glDrawArrays( GL_TRIANGLE_FAN, 0, particle.coords().size() / 3 );
            glDisableClientState( GL_VERTEX_ARRAY );
        }
    }
    glDisable( GL_DEPTH_TEST );

    glPopAttrib();
}

void AnalogClockRenderer::enableAntiAliasing( const bool multisample ) const
{
    m_enable_anti_aliasing = true;
    m_enable_multisample_anti_aliasing = multisample;
}

void AnalogClockRenderer::disableAntiAliasing( void ) const
{
    m_enable_anti_aliasing = false;
    m_enable_multisample_anti_aliasing = false;
}

void AnalogClockRenderer::enableTwoSideLighting( void ) const
{
    m_enable_two_side_lighting = true;
}

void AnalogClockRenderer::disableTwoSideLighting( void ) const
{
    m_enable_two_side_lighting = false;
}

const bool AnalogClockRenderer::isTwoSideLighting( void ) const
{
    return( m_enable_two_side_lighting );
}

void AnalogClockRenderer::initialize_projection( void )
{
   glMatrixMode( GL_PROJECTION );

   glMatrixMode( GL_MODELVIEW );
}

void AnalogClockRenderer::initialize_modelview( void )
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
