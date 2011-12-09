//
//  StochasticRenderer.h
//  
//
//  Created by Jun Nishimura on 11/4/10.
//  Copyright 2011 Jun Nishimura. All rights reserved.
//

#ifndef KVS__GLEW__STOCHASTIC_RENDERER_H_INCLUDE
#define KVS__GLEW__STOCHASTIC_RENDERER_H_INCLUDE

#include <kvs/ClassName>
#include <kvs/Module>
#include <kvs/ObjectBase>
#include <kvs/RendererBase>

#include <kvs/glew/FrameBufferObject>
#include <kvs/glew/EnsembleAverageBuffer>

#include "StochasticRendererBase.h"

#include <vector>

namespace kvs
{

namespace glew
{

class StochasticRenderer : public kvs::RendererBase
{

    // Class name.
    kvsClassName( kvs::glew::StochasticRenderer );

    // Module information.
    kvsModuleCategory( Renderer );
    kvsModuleBaseClass( kvs::RendererBase );

public:

    typedef std::vector<kvs::glew::StochasticRendererBase*> RendererList;

protected:

    // Renderer image size.
    size_t      m_width;
    size_t      m_height;

    // for Repetition process.
    size_t      m_repeat_level;
    size_t      m_coarse_level;

    // Buffers for Repetition process.
    kvs::glew::EnsembleAverageBuffer    m_ensemble_buffer;
    kvs::glew::FrameBufferObject        m_framebuffer;
    kvs::Texture2D                      m_color_texture;
    kvs::Texture2D                      m_depth_texture;

    // for Random texture.
    size_t          m_random_texture_size;
    kvs::Texture2D  m_random_texture;

    // for LOD rendering.
    bool    m_enable_lod;
    float   m_modelview_matrix[16];
    bool    m_clear_ensemble_buffer;

    // Registered list.
    RendererList    m_renderer_list;

public:

    StochasticRenderer( void );

    StochasticRenderer( const size_t repeat_level = 1 );

    virtual ~StochasticRenderer( void );

public:

    void exec( kvs::ObjectBase* object, kvs::Camera* camera, kvs::Light* light );

public:

    void initialize( void );

    void clear( void );

    void registerRenderer( kvs::glew::StochasticRendererBase* renderer );

    void changeObject(
        kvs::ObjectBase* object,
        kvs::glew::StochasticRendererBase* renderer,
        const bool is_delete = true );

    void clearEnsembleBuffer( void );

    void setRepeatLevel( const size_t repeat_level );

    void enableLODControl( const size_t coarse_level = 1 );

    void disableLODControl( void );

private:

    void create_image( const kvs::Camera* camera, const kvs::Light* light );

    void initialize_opengl( void );

    void initialize_framebuffer_texture( void );

    void create_texture(
        kvs::Texture2D& texture,
        kvs::glew::FrameBufferObject& framebuffer,
        GLint internal_format,
        GLenum external_format,
        GLenum external_type,
        GLenum attachment );

    void initialize_random_texture( void );

};

}

}

#endif
