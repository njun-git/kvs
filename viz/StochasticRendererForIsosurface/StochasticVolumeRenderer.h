//
//  StochasticVolumeRenderer.h
//  
//
//  Created by Jun Nishimura on 11/4/10.
//  Copyright 2011 Jun Nishimura. All rights reserved.
//

#ifndef KVS__GLEW__STOCHASTIC_VOLUME_RENDERER_H_INCLUDE
#define KVS__GLEW__STOCHASTIC_VOLUME_RENDERER_H_INCLUDE

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

class StochasticVolumeRenderer : public kvs::glew::StochasticRendererBase
{

    // Class name.
    kvsClassName( kvs::glew::StochasticVolumeRenderer );

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
    kvs::UnstructuredVolumeObject*  m_ref_volume;

    // Transfer function for volume rendering.
    kvs::TransferFunction           m_tfunc;

    // Buffer object for volume.
    kvs::glew::VertexBufferObject   m_vbo;
    kvs::glew::IndexBufferObject    m_ibo;

    // Location for attribute.
    size_t      m_loc_identifier;
    size_t      m_loc_values;

    Volume*     m_volume;
    Renderer*   m_renderer;

    // Textures for SPT.
    kvs::glew::PreIntegrationTable  m_table;
    kvs::Texture2D                  m_decomposition_texture;
    kvs::Texture2D                  m_depth_texture;

    // Parameter for SPT.
    float       m_edge_size;

    // for time-varying volume data.
    size_t      m_nsteps;
    size_t      m_step;

public:

    StochasticVolumeRenderer( void );

    StochasticVolumeRenderer( kvs::UnstructuredVolumeObject* volume, const size_t nsteps = 1 );

    virtual ~StochasticVolumeRenderer( void );

public:

    void exec( kvs::ObjectBase* object, kvs::Camera* camera, kvs::Light* light );

public:

    void initialize( void );

    void attachVolumeObject( kvs::UnstructuredVolumeObject* volume );

    void setNSteps( const size_t nsteps );

    void setStep( const size_t step );

    void setEdgeSize( const float edge_size );

    void setTransferFunction( const kvs::TransferFunction& tfunc );

    void setDepthTexture( const kvs::Texture2D& depth_texture );

public:

    const kvs::TransferFunction& transferFunction( void ) const;

    kvs::TransferFunction& transferFunction( void );

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

    void initialize_decomposition_texture( void );

    void create_preintegration_table( void );

    template <typename T>
    void create_vertexbuffer_from_volume( void );

    void create_shaders(
        kvs::glew::ProgramObject& program_object,
        const kvs::glew::ShaderSource& vertex_source,
        const kvs::glew::ShaderSource& geometry_source,
        const kvs::glew::ShaderSource& fragment_source );

    void setup_shader( const float modelview_matrix[16] );

};

class StochasticVolumeRenderer::Volume
{

private:

    size_t m_nsteps;
    size_t m_nvertices; ///< number of vertices
    size_t m_ncells; ///< number of cells
    StochasticVolumeRenderer::IndexType* m_indices; ///< index array
    StochasticVolumeRenderer::CoordType* m_coords; ///< coordinate value array
    StochasticVolumeRenderer::ValueType* m_values; ///< color value array
    StochasticVolumeRenderer::NormalType* m_normals; ///< normal array
    StochasticVolumeRenderer::ConnectType* m_connections;

public:

    Volume( void );

    ~Volume( void );

public:

    void release( void );

    void create( const size_t nsteps, const size_t nvertices, const size_t ncells );

public:

    const size_t nvertices( void ) const;

    const size_t ncells( void ) const;

    const size_t byteSizePerVertex( void ) const;

    const size_t byteSizeOfVertex( void ) const;

    const size_t byteSizePerCell( void ) const;

    const size_t byteSizeOfCell( void ) const;

    const StochasticVolumeRenderer::IndexType* indices( void ) const;

    StochasticVolumeRenderer::IndexType* indices( void );

    const StochasticVolumeRenderer::CoordType* coords( void ) const;

    StochasticVolumeRenderer::CoordType* coords( void );

    const StochasticVolumeRenderer::ValueType* values( void ) const;

    StochasticVolumeRenderer::ValueType* values( void );

    const StochasticVolumeRenderer::NormalType* normals( void ) const;

    StochasticVolumeRenderer::NormalType* normals( void );

    const StochasticVolumeRenderer::ConnectType* connections( void ) const;

    StochasticVolumeRenderer::ConnectType* connections( void );

};

class StochasticVolumeRenderer::Renderer
{

    const StochasticVolumeRenderer::Volume* m_volume; //< pointer to the volume
    size_t m_nsteps;
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
        const StochasticVolumeRenderer::Volume* volume,
        const size_t nsteps,
        const size_t nvertices,
        const size_t ncells,
        const size_t loc_identifier,
        const size_t loc_values );

    const bool download(
        kvs::glew::VertexBufferObject& vbo,
        kvs::glew::IndexBufferObject& ibo );

    void draw( const size_t step ) const;

};

}

}

#endif
