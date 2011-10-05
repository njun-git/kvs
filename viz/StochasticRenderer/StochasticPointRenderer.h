//
//  StochasticPointRenderer.h
//  
//
//  Created by njun on 11/5/8.
//  Copyright 2011 Jun Nishimura. All rights reserved.
//

#ifndef KVS__GLEW__STOCHASTIC_POINT_RENDERER_H_INCLUDE
#define KVS__GLEW__STOCHASTIC_POINT_RENDERER_H_INCLUDE

#include <kvs/ClassName>
#include <kvs/Module>
#include <kvs/PointObject>

#include <kvs/glew/VertexBufferObject>

#include "StochasticRendererBase.h"

namespace kvs
{

namespace glew
{

class StochasticPointRenderer : public kvs::glew::StochasticRendererBase
{
    // Class name.
    kvsClassName( kvs::glew::StochasticPointRenderer );

    // Module information.
    kvsModuleCategory( Renderer );
    kvsModuleBaseClass( kvs::glew::StochasticRendererBase );

protected:

    class Particles;
    class Renderer;

protected:

    typedef GLushort IndexType;
    typedef GLfloat  CoordType;
    typedef GLubyte  ColorType;
    typedef GLbyte   NormalType;

    const kvs::PointObject* m_ref_particle;

    kvs::glew::VertexBufferObject* m_vbo; ///< vertex buffer object (VBO) for the volume

    size_t      m_loc_identifier; ///< loc indectifier
    Particles*  m_particles;
    Renderer*   m_renderer;

    float   m_zooming_factor; ///< zooming factor
    size_t  m_circle_threshold; ///< threshold for the shape of the particle
    float   m_point_size;

public:

    StochasticPointRenderer( void );

    StochasticPointRenderer( kvs::PointObject* point, const size_t repeat_level = 1 );

    virtual ~StochasticPointRenderer( void );

public:

    void exec( kvs::ObjectBase* object, kvs::Camera* camera, kvs::Light* light );

public:

    void initialize( void );

    void attachPointObject( const kvs::PointObject* point );

    void setPointSize( const float point_size );

public:

    const RendererType rendererType( void ) const;

    void initializeShader( void );

    void createVertexBuffer( void );

    void downloadVertexBuffer( void );

    void drawVertexBuffer( const float modelview_matrix[16] );

    void clearEnsembleBuffer( void );

private:

    void create_vertexbuffer_from_point( void );

    void calculate_zooming_factor( void );

    void setup_shader( const float modelview_matrix[16] );

};

class StochasticPointRenderer::Particles
{

private:

    size_t m_nvertices; ///< number of vertices
    StochasticPointRenderer::IndexType* m_indices; ///< index array
    StochasticPointRenderer::CoordType* m_coords; ///< coordinate value array
    StochasticPointRenderer::NormalType* m_normals; ///< normal vector array
    StochasticPointRenderer::ColorType* m_colors; ///< color value array

public:

    Particles( void );

    ~Particles( void );

public:

    void release( void );

    void create( const size_t nvertices, const bool has_normal );

public:

    const bool hasNormal( void ) const;

    const size_t nvertices( void ) const;

    const size_t byteSizePerVertex( void ) const;

    const size_t byteSize( void ) const;

    const StochasticPointRenderer::IndexType* indices( void ) const;

    StochasticPointRenderer::IndexType* indices( void );

    const StochasticPointRenderer::CoordType* coords( void ) const;

    StochasticPointRenderer::CoordType* coords( void );

    const StochasticPointRenderer::NormalType* normals( void ) const;

    StochasticPointRenderer::NormalType* normals( void );

    const StochasticPointRenderer::ColorType* colors( void ) const;

    StochasticPointRenderer::ColorType* colors( void );

};

class StochasticPointRenderer::Renderer
{
    const StochasticPointRenderer::Particles*  m_particles; //< pointer to the particles
    size_t  m_start; ///< start number of input vertices
    size_t  m_count; ///< number of vertices
    size_t  m_off_index; ///< offset bytes for the index array
    size_t  m_off_coord; ///< offset bytes for the coodinate value array
    size_t  m_off_normal; ///< offset bytes for the normal vector array
    size_t  m_off_color; ///< offset bytes for the color value array
    size_t  m_loc_identifier; ///< 

public:

    Renderer( void );

    void set(
        const StochasticPointRenderer::Particles* particles,
        const size_t start,
        const size_t count,
        const size_t loc_identifier );

    const bool download( kvs::glew::VertexBufferObject& vbo );

    void draw( void ) const;

};

} // end of namespace glew

} // end of namespace kvs

#endif
