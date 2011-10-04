//
//  StochasticRendererBase.cpp
//  
//
//  Created by Jun Nishimura on 11/4/10.
//  Copyright 2011 Jun Nishimura. All rights reserved.
//

#include "StochasticRendererBase.h"

namespace kvs
{

namespace glew
{

StochasticRendererBase::StochasticRendererBase( void )
{
    this->initialize();
}

StochasticRendererBase::~StochasticRendererBase( void )
{
    this->clear();
}

void StochasticRendererBase::initialize( void )
{
    m_width = 0;
    m_height = 0;

    m_repeat_count = 0;
    m_repeat_level = 1;

    m_enable_shading = true;
    m_shader = NULL;
}

void StochasticRendererBase::clear( void )
{
    if ( m_shader )
    {
        delete m_shader;
        m_shader = NULL;
    }
}

void StochasticRendererBase::setRenderSize( const size_t width, const size_t height )
{
    m_width = width;
    m_height = height;
}

void StochasticRendererBase::setRepeatLevel( const size_t repeat_level )
{
    m_repeat_level = repeat_level;
}

void StochasticRendererBase::enableShading( void )
{
    m_enable_shading = true;
}

void StochasticRendererBase::disableShading( void )
{
    m_enable_shading = false;
}

void StochasticRendererBase::setRandomTexture( const kvs::Texture2D& random_texture )
{
    m_random_texture = random_texture;
}

const bool StochasticRendererBase::isEnabledShading( void ) const
{
    return( m_enable_shading );
}

void StochasticRendererBase::create_shaders(
    kvs::glew::ProgramObject& program_object,
    const kvs::glew::ShaderSource& vertex_source,
    const kvs::glew::ShaderSource& fragment_source )
{
    // Vertex shader.
    kvs::glew::VertexShader vertex_shader;
    if ( !vertex_shader.create( vertex_source ) )
    {
        GLenum error = glGetError();
        kvsMessageError( "VertexShader compile failed: %s(%d)¥n", gluErrorString(error), error );
        std::cout << "error log:" << std::endl;
        std::cout << vertex_shader.log() << std::endl;
    }

    // Fragment shader.
    kvs::glew::FragmentShader fragment_shader;
    if ( !fragment_shader.create( fragment_source ) )
    {
        GLenum error = glGetError();
        kvsMessageError( "FragmentShader compile failed: %s(%d)¥n", gluErrorString(error), error );
        std::cout << "error log:" << std::endl;
        std::cout << fragment_shader.log() << std::endl;
    }

    // Link the shaders.
    if ( !program_object.link( vertex_shader, fragment_shader ) )
    {
        GLenum error = glGetError();
        kvsMessageError( "ShaderProgram link failed: %s(%d)¥n", gluErrorString(error), error );
        std::cout << "error log:" << std::endl;
        std::cout << program_object.log() << std::endl;
        exit(1);
    }
}

}

}
