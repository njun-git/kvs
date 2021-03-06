//
//  StochasticRenderer.cpp
//  
//
//  Created by Jun Nishimura on 11/4/10.
//  Copyright 2011 Jun Nishimura. All rights reserved.
//

#include "StochasticRenderer.h"

#include <algorithm>
#include <kvs/MersenneTwister>

#include <kvs/tdw/Application>
#include <kvs/tdw/Configuration>

namespace
{
    struct SortingByRendererType
    {
        bool operator () ( const kvs::glew::StochasticRendererBase* r1, const kvs::glew::StochasticRendererBase* r2 )
        {
            return( r1->rendererType() < r2->rendererType() );
        }
    };
}

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

    m_renderer_list.clear();
}

void StochasticRenderer::clear( void )
{
    for ( size_t i = 0; i < m_renderer_list.size(); i++ )
    {
        kvs::glew::StochasticRendererBase* renderer = m_renderer_list[i];
        if ( renderer )
        {
            kvs::ObjectBase* object = renderer->object();
            if ( object )
            {
                delete object;
                object = NULL;
            }
            delete renderer;
            renderer = NULL;
        }
    }
    m_renderer_list.clear();
}

void StochasticRenderer::registerRenderer( kvs::glew::StochasticRendererBase* renderer )
{
    bool has_renderer = false;
    for ( size_t i = 0; i < m_renderer_list.size(); i++ )
    {
        if ( m_renderer_list[i] == renderer ) has_renderer = true;
    }
    if ( !has_renderer ) m_renderer_list.push_back( renderer );

    std::sort( m_renderer_list.begin(), m_renderer_list.end(), ::SortingByRendererType() );
}

void StochasticRenderer::changeObject(
    kvs::ObjectBase* object,
    kvs::glew::StochasticRendererBase* renderer,
    const bool is_delete )
{
    bool has_renderer = false;
    for ( size_t i = 0; i < m_renderer_list.size(); i++ )
    {
        if ( m_renderer_list[i] == renderer ) has_renderer = true;
    }

    if ( has_renderer )
    {
        if ( is_delete )
        {
            kvs::ObjectBase* old_object = renderer->object();
            if ( old_object )
            {
                delete old_object;
                old_object = NULL;
            }
        }
        renderer->attachObject( object );
        renderer->enableUpdateFlag();
        this->clearEnsembleBuffer();
    }
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

    kvs::Vector2i screen_size;
    if ( kvs::tdw::Application::IsMaster() )
    {
        const kvs::Vector2i full_size = kvs::tdw::Configuration::Master().resolution();
        const float aspect = static_cast<float>( full_size.x() ) / full_size.y();
        if ( aspect >= 1.0f ) screen_size.set( 512, 512 / aspect );
        else                  screen_size.set( 512 * aspect, 512 );
    }
    else
    {
        screen_size = kvs::tdw::Configuration::Renderer().size();
    }

    // Following processes are executed when the window size is changed.
    if ( ( m_width  != static_cast<size_t>( screen_size.x() ) ) ||
         ( m_height != static_cast<size_t>( screen_size.y() ) ) )
    {
        m_width  = static_cast<size_t>( screen_size.x() );
        m_height = static_cast<size_t>( screen_size.y() );

        for ( size_t i = 0; i < m_renderer_list.size(); i++ )
        {
            m_renderer_list[i]->setRenderSize( m_width, m_height );
        }

        m_ensemble_buffer.create( m_width, m_height );

        this->initialize_framebuffer_texture();
        retain_ensemble_buffer = false;
    }

    for ( size_t i = 0; i < m_renderer_list.size(); i++ )
    {
        if ( m_renderer_list[i]->isUpdate() )
        {
            m_renderer_list[i]->createVertexBuffer();
            m_renderer_list[i]->disableUpdateFlag();
        }
    }

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

    // Draw objects.
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

    this->create_texture( m_color_texture, m_framebuffer, GL_RGB8, GL_RGB, GL_UNSIGNED_BYTE, GL_COLOR_ATTACHMENT0_EXT );
    this->create_texture( m_depth_texture, m_framebuffer, GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT, GL_FLOAT, GL_DEPTH_ATTACHMENT_EXT );

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
    GLint internal_format,
    GLenum external_format,
    GLenum external_type,
    GLenum attachment )
{
    texture.release();

    texture.setWrapS( GL_CLAMP_TO_EDGE );
    texture.setWrapT( GL_CLAMP_TO_EDGE );
    texture.setMagFilter( GL_LINEAR );
    texture.setMinFilter( GL_LINEAR );

    texture.setPixelFormat( internal_format, external_format, external_type );
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
