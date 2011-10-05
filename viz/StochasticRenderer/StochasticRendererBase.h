//
//  StochasticRendererBase.h
//  
//
//  Created by Jun Nishimura on 11/4/10.
//  Copyright 2011 Jun Nishimura. All rights reserved.
//

#ifndef KVS__GLEW__STOCHASTIC_RENDERER_BASE_H_INCLUDE
#define KVS__GLEW__STOCHASTIC_RENDERER_BASE_H_INCLUDE

#include <kvs/ClassName>
#include <kvs/Module>
#include <kvs/RendererBase>
#include <kvs/Shader>

#include <kvs/glew/VertexShader>
#include <kvs/glew/FragmentShader>
#include <kvs/glew/ProgramObject>
#include <kvs/Texture2D>

namespace kvs
{

namespace glew
{

class StochasticRendererBase : public kvs::RendererBase
{

    // Class name.
    kvsClassName( kvs::glew::StochasticRendererBase );

    // Module information.
    kvsModuleCategory( Renderer );
    kvsModuleBaseClass( kvs::RendererBase );

public:

    enum RendererType
    {
        Point = 0,
        Line,
        Polygon,
        Volume,
        Unknown
    };

protected:

    // Renderer image size.
    size_t      m_width;
    size_t      m_height;

    // for Repetition process.
    size_t      m_repeat_count;
    size_t      m_repeat_level;

    // for Shading.
    bool                        m_enable_shading;
    kvs::Shader::shader_type*   m_shader;

    // Random texture for stochastic rendering.
    kvs::Texture2D  m_random_texture;

    // GLSL shader program.
    kvs::glew::ProgramObject    m_shader_program;

public:

    StochasticRendererBase( void );

    virtual ~StochasticRendererBase( void );

public:

    virtual void exec(
        kvs::ObjectBase* object,
        kvs::Camera* camera = NULL,
        kvs::Light* light = NULL ) = 0;

public:

    void initialize( void );

    void clear( void );

    void setRenderSize( const size_t width, const size_t height );

    void setRepeatLevel( const size_t repeat_level );

    template <typename ShadingType>
    void setShader( const ShadingType shader );

    void enableShading( void );

    void disableShading( void );

    void setRandomTexture( const kvs::Texture2D& random_texture );

public:

    const bool isEnabledShading( void ) const;

public:

    virtual const RendererType rendererType( void ) const = 0;

    virtual void initializeShader( void ) = 0;

    virtual void createVertexBuffer( void ) = 0;

    virtual void downloadVertexBuffer( void ) = 0;

    virtual void drawVertexBuffer( const float modelview_matrix[16] ) = 0;

    virtual void clearEnsembleBuffer( void ) = 0;

protected:

    void create_shaders(
        kvs::glew::ProgramObject& program_object,
        const kvs::glew::ShaderSource& vertex_source,
        const kvs::glew::ShaderSource& fragment_source );

    virtual void setup_shader( const float modelview_matrix[16] ) = 0;

};

template <typename ShadingType>
inline void StochasticRendererBase::setShader( const ShadingType shader )
{
    if ( m_shader )
    {
        delete m_shader;
        m_shader = NULL;
    }

    m_shader = new ShadingType( shader );
    if ( !m_shader )
    {
        kvsMessageError( "Cannot create a specified shader." );
    }
};

}

}

#endif
