//
//  StochasticVolume2Renderer.h
//  
//
//  Created by Jun Nishimura on 11/4/10.
//  Copyright 2011 Jun Nishimura. All rights reserved.
//

#ifndef KVS__GLEW__STOCHASTIC_VOLUME2_RENDERER_H_INCLUDE
#define KVS__GLEW__STOCHASTIC_VOLUME2_RENDERER_H_INCLUDE

#include "StochasticRendererBase.h"
#include "PreIntegrationTable.h"

#include <kvs/UnstructuredVolumeObject>
#include <kvs/TransferFunction>

#include <kvs/glew/GeometryShader>

#include <kvs/glew/VertexBufferObject>
#include <kvs/glew/IndexBufferObject>

namespace kvs
{

namespace glew
{

class StochasticVolume2Renderer : public kvs::glew::StochasticRendererBase
{

    // Class name.
    kvsClassName( kvs::glew::StochasticVolume2Renderer );

    // Module information.
    kvsModuleCategory( Renderer );
    kvsModuleBaseClass( kvs::glew::StochasticRendererBase );

protected:

    class Volume;
    class Renderer;

protected:

    typedef GLushort IndexType;
    typedef GLfloat  CoordType;
    typedef GLfloat  ValueType;
    typedef GLbyte   NormalType;
    typedef GLuint   ConnectType;

    // Reference only.
    kvs::UnstructuredVolumeObject*  m_ref_volume[2];

    // Transfer function for volume rendering.
    kvs::TransferFunction           m_tfunc[2];

    // Buffer object for volume.
    kvs::glew::VertexBufferObject   m_vbo[2];
    kvs::glew::IndexBufferObject    m_ibo[2];

    // Location for attribute.
    size_t      m_loc_identifier[2];
    size_t      m_loc_values[2];

    Volume*     m_volume;
    Renderer*   m_renderer;

    // Shader program.
    kvs::glew::ProgramObject        m_program[2];

    // Textures for SPT.
    kvs::glew::PreIntegrationTable  m_table[2];
    kvs::Texture2D                  m_decomposition_texture;
    kvs::Texture2D                  m_depth_texture;
    kvs::Texture2D                  m_extra_texture;

    // Parameter for SPT.
    float       m_edge_size;

public:

    StochasticVolume2Renderer( void );

    StochasticVolume2Renderer(
        kvs::UnstructuredVolumeObject* volume1,
        kvs::UnstructuredVolumeObject* volume2 );

    virtual ~StochasticVolume2Renderer( void );

public:

    void exec( kvs::ObjectBase* object, kvs::Camera* camera, kvs::Light* light );

public:

    void initialize( void );

    void attachVolumeObject( kvs::UnstructuredVolumeObject* volume, const size_t index = 0 );

    void setEdgeSize( const float edge_size );

    void setTransferFunction( const kvs::TransferFunction& tfunc, const size_t index = 0 );

    void setDepthTexture( const kvs::Texture2D& depth );

    void setExtraTexture( const kvs::Texture2D& extra );

public:

    const kvs::TransferFunction& transferFunction( const size_t index = 0 ) const;

    kvs::TransferFunction& transferFunction( const size_t index = 0 );

public:

    kvs::ObjectBase* object( void ) const;

    void attachObject( kvs::ObjectBase* object );

    const RendererType rendererType( void ) const;

    void initializeShader( void );

    void createVertexBuffer( void );

    void downloadVertexBuffer( void );

    void drawVertexBuffer( const float modelview_matrix[16] );

    void clearEnsembleBuffer( void );

private:

    void initialize_shader_of( const size_t index );

    void initialize_decomposition_texture( void );

    void create_preintegration_table( void );

    template <typename T>
    void create_vertexbuffer_from_volume( void );

    template <typename T>
    void create_vertexbuffer_from_volume_of( const size_t index );

    void create_shaders(
        kvs::glew::ProgramObject& program_object,
        const kvs::glew::ShaderSource& vertex_source,
        const kvs::glew::ShaderSource& geometry_source,
        const kvs::glew::ShaderSource& fragment_source );

    void setup_shader( const float modelview_matrix[16] );

    void setup_shader_of( const size_t index );

};

class StochasticVolume2Renderer::Volume
{

private:

    size_t m_nvertices; ///< number of vertices
    size_t m_ncells; ///< number of cells
    StochasticVolume2Renderer::IndexType* m_indices; ///< index array
    StochasticVolume2Renderer::CoordType* m_coords; ///< coordinate value array
    StochasticVolume2Renderer::ValueType* m_values; ///< color value array
    StochasticVolume2Renderer::NormalType* m_normals; ///< normal array
    StochasticVolume2Renderer::ConnectType* m_connections;

public:

    Volume( void );

    ~Volume( void );

public:

    void release( void );

    void create( const size_t nvertices, const size_t ncells );

public:

    const size_t nvertices( void ) const;

    const size_t ncells( void ) const;

    const size_t byteSizePerVertex( void ) const;

    const size_t byteSizeOfVertex( void ) const;

    const size_t byteSizePerCell( void ) const;

    const size_t byteSizeOfCell( void ) const;

    const StochasticVolume2Renderer::IndexType* indices( void ) const;

    StochasticVolume2Renderer::IndexType* indices( void );

    const StochasticVolume2Renderer::CoordType* coords( void ) const;

    StochasticVolume2Renderer::CoordType* coords( void );

    const StochasticVolume2Renderer::ValueType* values( void ) const;

    StochasticVolume2Renderer::ValueType* values( void );

    const StochasticVolume2Renderer::NormalType* normals( void ) const;

    StochasticVolume2Renderer::NormalType* normals( void );

    const StochasticVolume2Renderer::ConnectType* connections( void ) const;

    StochasticVolume2Renderer::ConnectType* connections( void );

};

class StochasticVolume2Renderer::Renderer
{

    const StochasticVolume2Renderer::Volume* m_volume; //< pointer to the volume
    size_t m_nvertices; ///< number of vertices
    size_t m_ncells; ///< number of cells
    size_t m_off_index; ///< offset bytes for the index array
    size_t m_off_coord; ///< offset bytes for the coodinate value array
    size_t m_off_value; ///< offset bytes for the value array
    size_t m_off_normal; ///< offset bytes for the normal array
    size_t m_loc_identifier; ///< loc identifier
    size_t m_loc_values; ///< loc values

public:

    Renderer( void );

    void set(
        const StochasticVolume2Renderer::Volume* volume,
        const size_t nvertices,
        const size_t ncells,
        const size_t loc_identifier,
        const size_t loc_values );

    const bool download(
        kvs::glew::VertexBufferObject& vbo,
        kvs::glew::IndexBufferObject& ibo );

    void draw( void ) const;

};

}

}

#endif
