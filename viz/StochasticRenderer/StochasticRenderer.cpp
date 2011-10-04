//
//  StochasticRenderer.cpp
//  
//
//  Created by Jun Nishimura on 11/4/10.
//  Copyright 2011 Jun Nishimura. All rights reserved.
//

#include "StochasticRenderer.h"

#include <kvs/MersenneTwister>

namespace kvs
{

namespace glew
{

StochasticRenderer::StochasticRenderer( void )
{
    kvs::glew::Initialize();
    this->initialize();
}

StochasticRenderer::StochasticRenderer( const size_t repeat_level )
{
    kvs::glew::Initialize();

    this->initialize();
    this->setRepeatLevel( repeat_level );
}

StochasticRenderer::~StochasticRenderer( void )
{
    this->clear();
}

void StochasticRenderer::exec(
    kvs::ObjectBase* object,
    kvs::Camera*     camera,
    kvs::Light*      light )
{
    kvs::IgnoreUnusedVariable( object );
    kvs::IgnoreUnusedVariable( light );

    BaseClass::m_timer.start();
    this->create_image( camera, light );
    BaseClass::m_timer.stop();
}

void StochasticRenderer::initialize( void )
{
    m_width = 0;
    m_height = 0;

    m_repeat_level = 1;
    m_coarse_level = 1;

    m_random_texture_size = 1024;

    m_enable_lod = false;
    memset( m_modelview_matrix, 0, sizeof( m_modelview_matrix ) );

    m_clear_ensemble_buffer = false;

    m_object_list.clear();
    m_renderer_list.clear();
}

void StochasticRenderer::clear( void )
{
    for ( size_t i = 0; i < m_object_list.size(); i++ )
    {
        kvs::ObjectBase* object = m_object_list[i];
        if ( object )
        {
            delete object;
            object = NULL;
        }
        kvs::glew::StochasticRendererBase* renderer = m_renderer_list[i];
        if ( renderer )
        {
            delete renderer;
            renderer = NULL;
        }
    }
    m_object_list.clear();
    m_renderer_list.clear();
}

void StochasticRenderer::registerRenderer( kvs::ObjectBase* object, kvs::glew::StochasticRendererBase* renderer )
{
    m_object_list.push_back( object );
    m_renderer_list.push_back( renderer );
}

void StochasticRenderer::clearEnsembleBuffer( void )
{
    m_clear_ensemble_buffer = true;
}

void StochasticRenderer::setRepeatLevel( const size_t repeat_level )
{
    m_repeat_level = repeat_level;
    m_coarse_level = repeat_level;
}

void StochasticRenderer::enableLODControl( const size_t coarse_level )
{
    m_enable_lod = true;
    m_coarse_level = coarse_level;
}

void StochasticRenderer::disableLODControl( void )
{
    m_enable_lod = false;
}

void StochasticRenderer::create_image( const kvs::Camera* camera, const kvs::Light* light )
{
    glPushAttrib( GL_CURRENT_BIT | GL_ENABLE_BIT | GL_LIGHTING_BIT );

    RendererBase::initialize();

    bool retain_ensemble_buffer = true;

    // Following processes are executed once.
    if ( m_width == 0 && m_height == 0 )
    {
        m_ensemble_buffer.disableAccumulationBuffer();

        this->initialize_opengl();
        this->initialize_random_texture();

        for ( size_t i = 0; i < m_renderer_list.size(); i++ )
        {
            m_renderer_list[i]->setRandomTexture( m_random_texture );
            m_renderer_list[i]->initializeShader();
            m_renderer_list[i]->createVertexBuffer();
        }

        glGetFloatv( GL_MODELVIEW_MATRIX, m_modelview_matrix );
        retain_ensemble_buffer = false;
    }

    // Get the modelview matrix.
    float modelview_matrix[16];
    glGetFloatv( GL_MODELVIEW_MATRIX, modelview_matrix );
    static kvs::Vector3f light_position = light->position();

    // LOD control.
    size_t coarse_level = m_repeat_level;
    {
        if ( light_position != light->position() )
        {
            if ( m_enable_lod ) coarse_level = m_coarse_level;
            retain_ensemble_buffer = false;
        }
        else
        {
            for ( size_t i = 0; i < 16; i++ )
            {
                if ( m_modelview_matrix[i] != modelview_matrix[i] )
                {
                    if ( m_enable_lod ) coarse_level = m_coarse_level;
                    retain_ensemble_buffer = false;
                    break;
                }
            }
        }
        memcpy( m_modelview_matrix, modelview_matrix, sizeof( modelview_matrix ) );
    }

    // Following processes are executed when the window size is changed.
    if ( ( m_width  != camera->windowWidth() ) ||
         ( m_height != camera->windowHeight() ) )

    {
        m_width  = camera->windowWidth();
        m_height = camera->windowHeight();

        for ( size_t i = 0; i < m_renderer_list.size(); i++ )
        {
            m_renderer_list[i]->setRenderSize( m_width, m_height );
        }

        m_ensemble_buffer.create( m_width, m_height );

        this->initialize_framebuffer_texture();
        retain_ensemble_buffer = false;
    }

    // Draw objects.
    const size_t repeat_count = coarse_level;
    if ( !retain_ensemble_buffer || m_clear_ensemble_buffer )
    {
        glClear( GL_ACCUM_BUFFER_BIT );
        m_ensemble_buffer.clear();
        for ( size_t i = 0; i < m_renderer_list.size(); i++ )
        {
            m_renderer_list[i]->clearEnsembleBuffer();
        }
    }

    for ( size_t rp = 0; rp < repeat_count; rp++ )
    {
        m_framebuffer.bind();

        glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
        glEnable( GL_DEPTH_TEST );

        // draw vertex buffer.
        for ( size_t i = 0; i < m_renderer_list.size(); i++ )
        {
            m_renderer_list[i]->drawVertexBuffer( modelview_matrix );
        }

        // render to the screen.
        m_framebuffer.disable();
        m_ensemble_buffer.bind();

        glMatrixMode( GL_MODELVIEW );  glPushMatrix(); glLoadIdentity();
        glMatrixMode( GL_PROJECTION ); glPushMatrix(); glLoadIdentity();
        glOrtho( 0, 1, 0, 1, -1, 1 );

        m_color_texture.bind();

        glDisable( GL_DEPTH_TEST );
        glDisable( GL_LIGHTING );
        glBegin( GL_QUADS );
        glColor4f( 1.0, 1.0, 1.0, m_ensemble_buffer.opacity() );
        glTexCoord2f( 1, 1 );  glVertex2f( 1, 1 );
        glTexCoord2f( 0, 1 );  glVertex2f( 0, 1 );
        glTexCoord2f( 0, 0 );  glVertex2f( 0, 0 );
        glTexCoord2f( 1, 0 );  glVertex2f( 1, 0 );
        glEnd();

        m_color_texture.unbind();
        glPopMatrix(); // Pop PROJECTION matrix
        glMatrixMode( GL_MODELVIEW );
        glPopMatrix(); // Pop MODELVIEW matrix

        m_ensemble_buffer.add();
    }
    m_ensemble_buffer.draw();

    m_clear_ensemble_buffer = false;

    light_position = light->position();

    glPopAttrib();
    glFinish();
}

void StochasticRenderer::initialize_opengl( void )
{
    glHint( GL_PERSPECTIVE_CORRECTION_HINT, GL_FASTEST );

    glClearAccum( 0.0, 0.0, 0.0, 0.0 );
}

void StochasticRenderer::initialize_framebuffer_texture( void )
{
    m_framebuffer.create();
    m_framebuffer.bind();

    this->create_texture( m_color_texture, m_framebuffer, GL_RGB8, GL_COLOR_ATTACHMENT0_EXT );
    this->create_texture( m_depth_texture, m_framebuffer, GL_DEPTH_COMPONENT, GL_DEPTH_ATTACHMENT_EXT );

    m_framebuffer.disable();

    GLenum error = glGetError();
    if ( error != GL_NO_ERROR )
    {
        kvsMessageError( "framebuffer allocation failed: %s.", gluErrorString(error));
        exit( EXIT_FAILURE );
    }
}

void StochasticRenderer::create_texture(
    kvs::Texture2D& texture,
    kvs::glew::FrameBufferObject& framebuffer,
    GLint format,
    GLenum attachment )
{
    texture.release();

    texture.setWrapS( GL_CLAMP_TO_EDGE );
    texture.setWrapT( GL_CLAMP_TO_EDGE );
    texture.setMagFilter( GL_LINEAR );
    texture.setMinFilter( GL_LINEAR );

    const GLenum external_format = ( format == GL_DEPTH_COMPONENT ) ? GL_DEPTH_COMPONENT : GL_RGB;

    texture.setPixelFormat( format, external_format, GL_UNSIGNED_BYTE );
    texture.create( m_width, m_height );
    {
        GLenum error = glGetError();
        if ( error != GL_NO_ERROR )
        {
            kvsMessageError( "color buffer allocation failed: %s.", gluErrorString(error));
            exit( EXIT_FAILURE );
        }
    }

    glFramebufferTexture2DEXT( GL_FRAMEBUFFER_EXT, attachment, GL_TEXTURE_2D, texture.id(), 0 );

    texture.unbind();
}

void StochasticRenderer::initialize_random_texture( void )
{
    m_random_texture.release();
    m_random_texture.setWrapS( GL_REPEAT );
    m_random_texture.setWrapT( GL_REPEAT );
    m_random_texture.setMagFilter( GL_NEAREST );
    m_random_texture.setMinFilter( GL_NEAREST );
    m_random_texture.setPixelFormat( GL_INTENSITY8,  GL_LUMINANCE, GL_UNSIGNED_BYTE  );

    m_random_texture.create( m_random_texture_size, m_random_texture_size );
    {
        GLenum error = glGetError();
        if ( error != GL_NO_ERROR )
        {
            kvsMessageError( "color buffer allocation failed: %s.", gluErrorString(error) ); exit( EXIT_FAILURE );
        }
    }

    const size_t npixels = m_random_texture_size * m_random_texture_size;

    kvs::MersenneTwister random;
    GLubyte* pixels = new GLubyte[ npixels ];
    for ( size_t i = 0; i < npixels; i++ )
    {
        pixels[i] = static_cast<GLubyte>( random.randInteger() );
    }
    m_random_texture.download( m_random_texture_size, m_random_texture_size, pixels );
    delete [] pixels;
}

}

}
