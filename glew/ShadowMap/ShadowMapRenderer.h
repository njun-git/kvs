//
//  ShadowMapRenderer.h
//  
//
//  Created by Jun Nishimura on 11/9/10.
//  Copyright 2011 Jun Nishimura. All rights reserved.
//

#ifndef KVS__GLEW__SHADOW_MAP_RENDERER_H_INCLUDE
#define KVS__GLEW__SHADOW_MAP_RENDERER_H_INCLUDE

#include <kvs/OpenGL>
#include <kvs/RendererBase>
#include <kvs/ClassName>
#include <kvs/Module>
#include <kvs/PolygonObject>
#include <kvs/Shader>
#include <kvs/Texture2D>
#include <kvs/ScreenBase>

#include <kvs/glew/FrameBufferObject>
#include <kvs/glew/VertexBufferObject>
#include <kvs/glew/IndexBufferObject>
#include <kvs/glew/VertexShader>
#include <kvs/glew/FragmentShader>
#include <kvs/glew/ProgramObject>

namespace kvs
{

namespace glew
{

class ShadowMapRenderer : public kvs::RendererBase
{

    // Class name.
    kvsClassName( kvs::glew::ShadowMapRenderer );

    // Module information.
    kvsModuleCategory( Renderer );
    kvsModuleBaseClass( kvs::RendererBase );

protected:

    typedef GLfloat  CoordType;
    typedef GLubyte  ColorType;
    typedef GLbyte   NormalType;
    typedef GLuint   ConnectType;

    // Reference only.
    kvs::ScreenBase*            m_ref_screen;
    const kvs::PolygonObject*   m_ref_polygon;

    size_t  m_width;
    size_t  m_height;

    size_t  m_shadow_width;
    size_t  m_shadow_height;

    kvs::glew::VertexBufferObject   m_vbo;
    kvs::glew::IndexBufferObject    m_ibo;
    kvs::glew::ProgramObject        m_shadow_map_shader;
    kvs::Shader::shader_type*       m_shader;

    // Offset for VBO drawing.
    size_t  m_off_coord;
    size_t  m_off_color;
    size_t  m_off_normal;

    kvs::glew::FrameBufferObject    m_framebuffer;
    kvs::Texture2D                  m_shadow_texture;

    int     m_shadow_level;
    float   m_shadow_strength;

    bool    m_is_degug_draw;

public:

    ShadowMapRenderer( kvs::ScreenBase* screen );

    virtual ~ShadowMapRenderer( void );

public:

    void initialize( void );

    void clear( void );

    void attachPolygonObject( const kvs::PolygonObject* polygon );

    template <typename ShadingType>
    void setShader( const ShadingType shader );

    void setShadowSize( const size_t width, const size_t height );

    void setShadowLevel( const int level );

    void setShadowStrength( const float strength );

    void enableDebugDraw( void );

    void disableDebugDraw( void );

public:

    void exec( kvs::ObjectBase* object, kvs::Camera* camera, kvs::Light* light );

protected:

    void create_image( kvs::PolygonObject* polygon, kvs::Camera* camera, kvs::Light* light );

    void download_buffer_object( void );

    void draw_buffer_object( void );

    void draw_shadow( GLfloat* modelview_matrix );

    void draw_buffer_object_shadow( void );

    void initialize_shaders( void );

    void initialize_framebuffer( void );

    void create_shaders(
        kvs::glew::ProgramObject& program_object,
        const kvs::glew::ShaderSource& vertex_source,
        const kvs::glew::ShaderSource& fragment_source );

    void setup_shader_parameter( void );

};

template <typename ShadingType>
inline void ShadowMapRenderer::setShader( const ShadingType shader )
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
