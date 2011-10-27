//
//  TextureRenderer.cpp
//  
//
//  Created by Jun Nishimura on 10/27/11.
//  Copyright (c) 2011 Jun Nishimura. All rights reserved.
//

#include "TextureRenderer.h"

#include <kvs/Camera>
#include <kvs/Message>

namespace kvs
{

namespace ni
{

TextureRenderer::TextureRenderer( const TextureRenderer::Type& type ) :
    kvs::RendererBase()
{
    m_initial_aspect_ratio = 640.0f / 480.0f;
    m_left = 0.0f;
    m_right = 1.0f;
    m_bottom = 0.0f;
    m_top = 1.0f;
    m_scale = 1.0f;
    m_type = type;

    m_coords.allocate( 2 * 4 );
    m_texcoords.allocate( 2 * 4 );
    m_texcoords[0] = 0.0f; m_texcoords[1] = 0.0f;
    m_texcoords[2] = 0.0f; m_texcoords[3] = 1.0f;
    m_texcoords[4] = 1.0f; m_texcoords[5] = 1.0f;
    m_texcoords[6] = 1.0f; m_texcoords[7] = 0.0f;
}

TextureRenderer::~TextureRenderer( void )
{
}

const float TextureRenderer::scale( void ) const
{
    return( m_scale );
}

void TextureRenderer::setType( const TextureRenderer::Type& type )
{
    m_type = type;
}

void TextureRenderer::setScale( const float scale )
{
    if ( scale > 0.0f ) m_scale = scale;
}

void TextureRenderer::exec( kvs::ObjectBase* object, kvs::Camera* camera, kvs::Light* light )
{
    kvs::ni::TextureObject* image = reinterpret_cast<kvs::ni::TextureObject*>( object );

    glPushAttrib( GL_CURRENT_BIT | GL_ENABLE_BIT );

    glDisable( GL_DEPTH_TEST );
    glEnable( GL_TEXTURE_2D );
    glEnable( GL_ALPHA_TEST );
    glEnable( GL_BLEND );
    glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

    this->centering( camera->windowWidth(), camera->windowHeight() );
    this->calculate_vertices();
    this->create_texture( image );

    glMatrixMode( GL_MODELVIEW );
    glPushMatrix();
    {
        glLoadIdentity();
        glMatrixMode( GL_PROJECTION );
        glPushMatrix();
        {
            glLoadIdentity();
            glOrtho( m_left, m_right, m_bottom, m_top, - 1, 1 );

            glEnableClientState( GL_VERTEX_ARRAY );
            glVertexPointer( 2, GL_FLOAT, 0, m_coords.pointer() );

            glEnableClientState( GL_TEXTURE_COORD_ARRAY );
            glTexCoordPointer( 2, GL_FLOAT, 0, m_texcoords.pointer() );

            glDrawArrays( GL_QUADS, 0, 4 );

            glDisableClientState( GL_TEXTURE_COORD_ARRAY );
            glDisableClientState( GL_VERTEX_ARRAY );
        }
        glPopMatrix();
        glMatrixMode( GL_MODELVIEW );
    }
    glPopMatrix();
    m_texture.unbind();

    glDisable( GL_ALPHA_TEST );
    glEnable( GL_DEPTH_TEST );
    glDisable( GL_TEXTURE_2D );

    glPopAttrib();
}

void TextureRenderer::create_texture( const kvs::ni::TextureObject* object )
{
    if ( !glIsTexture( m_texture.id() ) )
    {
        m_texture.setPixelFormat( 4, 1 );
        m_texture.create( object->width(), object->height() );
        glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE );
    }
    m_texture.bind();
    m_texture.download( object->width(), object->height(), object->data().pointer() );
}

void TextureRenderer::centering( const float width, const float height )
{
    switch( m_type )
    {
    case Stretch:
    {
        m_left = 0.0f;
        m_right = 1.0f;
        m_bottom = 0.0f;
        m_top = 1.0f;
        break;
    }
    default: break;
    }

    float current_aspect_ratio = width / height;
    float aspect_ratio = current_aspect_ratio / m_initial_aspect_ratio;
    if( aspect_ratio >= 1.0 )
    {
        m_left   = ( 1.0 - aspect_ratio ) * 0.5;
        m_right  = ( 1.0 + aspect_ratio ) * 0.5;
        m_bottom = 0.0;
        m_top    = 1.0;
    }
    else
    {
        m_left   = 0.0;
        m_right  = 1.0;
        m_bottom = ( 1.0 - 1.0 / aspect_ratio ) * 0.5;
        m_top    = ( 1.0 + 1.0 / aspect_ratio ) * 0.5;
    }
}

void TextureRenderer::calculate_vertices( void )
{
    switch ( m_type )
    {
    case Center:
    {
        const float top = 0.5f + m_scale * 0.5f;
        const float bottom = 0.5f - m_scale * 0.5f;
        m_coords[0] = bottom;   m_coords[1] = top;
        m_coords[2] = bottom;   m_coords[3] = bottom;
        m_coords[4] = top;      m_coords[5] = bottom;
        m_coords[6] = top;      m_coords[7] = top;
        break;
    }
    case LeftTop:
    {
        m_coords[0] = m_left;           m_coords[1] = m_top;
        m_coords[2] = m_left;           m_coords[3] = m_top - m_scale;
        m_coords[4] = m_left + m_scale; m_coords[5] = m_top - m_scale;
        m_coords[6] = m_left + m_scale; m_coords[7] = m_top;
        break;
    }
    case LeftBottom:
    {
        m_coords[0] = m_left;           m_coords[1] = m_bottom + m_scale;
        m_coords[2] = m_left;           m_coords[3] = m_bottom;
        m_coords[4] = m_left + m_scale; m_coords[5] = m_bottom;
        m_coords[6] = m_left + m_scale; m_coords[7] = m_bottom + m_scale;
        break;
    }
    case RightTop:
    {
        m_coords[0] = m_right - m_scale; m_coords[1] = m_top;
        m_coords[2] = m_right - m_scale; m_coords[3] = m_top - m_scale;
        m_coords[4] = m_right;           m_coords[5] = m_top - m_scale;
        m_coords[6] = m_right;           m_coords[7] = m_top;
        break;
    }
    case RightBottom:
    {
        m_coords[0] = m_right - m_scale; m_coords[1] = m_bottom + m_scale;
        m_coords[2] = m_right - m_scale; m_coords[3] = m_bottom;
        m_coords[4] = m_right;           m_coords[5] = m_bottom;
        m_coords[6] = m_right;           m_coords[7] = m_bottom + m_scale;
        break;
    }
    default:
    {
        m_coords[0] = 0.0f; m_coords[1] = 1.0f;
        m_coords[2] = 0.0f; m_coords[3] = 0.0f;
        m_coords[4] = 1.0f; m_coords[5] = 0.0f;
        m_coords[6] = 1.0f; m_coords[7] = 1.0f;
        break;
    }
    }
}

}

}
