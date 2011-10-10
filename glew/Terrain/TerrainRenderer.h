/*
 *  TerrainRenderer.h
 *  
 *
 *  Created by njun on 11/10/11.
 *  Copyright 2011 Jun Nishimura. All rights reserved.
 *
 */

#ifndef KVS__TERRAIN_RENDERER_H_INCLUDE
#define KVS__TERRAIN_RENDERER_H_INCLUDE

#include <kvs/OpenGL>
#include <kvs/RendererBase>
#include <kvs/ClassName>
#include <kvs/Module>

#include "PaintImageObject.h"
#include "TerrainObject.h"

#include <kvs/Shader>
#include <kvs/Texture2D>

#include <kvs/glew/VertexBufferObject>
#include <kvs/glew/IndexBufferObject>
#include <kvs/glew/VertexShader>
#include <kvs/glew/GeometryShader>
#include <kvs/glew/FragmentShader>
#include <kvs/glew/ProgramObject>

namespace kvs
{

namespace glew
{

class TerrainRenderer : public kvs::RendererBase
{

    // Class name.
    kvsClassName( kvs::TerrainRenderer );

    // Module information.
    kvsModuleCategory( Renderer );
    kvsModuleBaseClass( kvs::RendererBase );

protected:

    typedef GLfloat  CoordType;
    typedef GLuint   ConnectType;

    // Reference only.
    const kvs::TerrainObject*       m_ref_terrain;
    kvs::PaintImageObject*          m_image;

    bool    m_is_initialize;
    bool    m_is_downloaded;

    kvs::Texture2D                  m_texture;
    kvs::glew::VertexBufferObject   m_vbo;
    kvs::glew::IndexBufferObject    m_ibo;
    kvs::glew::ProgramObject        m_terrain_shader;
    kvs::Shader::shader_type*       m_shader;

public:

    TerrainRenderer( void );

    virtual ~TerrainRenderer( void );

public:

    void initialize( void );

    void clear( void );

    void attachTerrainObject( const kvs::TerrainObject* terrain );

    void attachImage( kvs::PaintImageObject* image );

    template <typename ShadingType>
    void setShader( const ShadingType shader );

public:

    void exec( kvs::ObjectBase* object, kvs::Camera* camera, kvs::Light* light );

protected:

    void download_buffer_object( void );

    void draw_buffer_object( void );

    void initialize_shader( void );

    void create_shaders(
        kvs::glew::ProgramObject& program_object,
        const kvs::glew::ShaderSource& vertex_source,
        const kvs::glew::ShaderSource& geometry_source,
        const kvs::glew::ShaderSource& fragment_source );

    void setup_shader_parameter( void );

};

template <typename ShadingType>
inline void TerrainRenderer::setShader( const ShadingType shader )
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
