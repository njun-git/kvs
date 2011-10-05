//
//  StochasticPolygonRenderer.h
//  
//
//  Created by Jun Nishimura on 11/4/10.
//  Copyright 2011 Jun Nishimura. All rights reserved.
//

#ifndef KVS__GLEW__STOCHASTIC_POLYGON_RENDERER_H_INCLUDE
#define KVS__GLEW__STOCHASTIC_POLYGON_RENDERER_H_INCLUDE

#include "StochasticRendererBase.h"

#include <kvs/PolygonObject>

#include <kvs/glew/VertexBufferObject>
#include <kvs/glew/IndexBufferObject>

namespace kvs
{

namespace glew
{

class StochasticPolygonRenderer : public kvs::glew::StochasticRendererBase
{

    // Class name.
    kvsClassName( kvs::glew::StochasticPolygonRenderer );

    // Module information.
    kvsModuleCategory( Renderer );
    kvsModuleBaseClass( kvs::glew::StochasticRendererBase );

protected:

    typedef GLushort IndexType;
    typedef GLfloat  CoordType;
    typedef GLubyte  ColorType;
    typedef GLbyte   NormalType;
    typedef GLuint   ConnectType;

    // Reference only.
    const kvs::PolygonObject* m_ref_polygon;

    kvs::glew::VertexBufferObject   m_vbo;
    kvs::glew::IndexBufferObject    m_ibo;

    // Offset for VBO drawing.
    size_t  m_loc_identifier; ///< loc indectifier
    size_t  m_off_index;
    size_t  m_off_coord;
    size_t  m_off_color;
    size_t  m_off_normal;

    float   m_polygon_offset;

public:

    StochasticPolygonRenderer( void );

    StochasticPolygonRenderer( const kvs::PolygonObject* polygon );

    virtual ~StochasticPolygonRenderer( void );

public:

    void exec( kvs::ObjectBase* object, kvs::Camera* camera, kvs::Light* light );

public:

    void initialize( void );

    void attachPolygonObject( const kvs::PolygonObject* polygon );

    void setPolygonOffset( const float polygon_offset );

public:

    const RendererType rendererType( void ) const;

    void initializeShader( void );

    void createVertexBuffer( void );

    void downloadVertexBuffer( void );

    void drawVertexBuffer( const float modelview_matrix[16] );

    void clearEnsembleBuffer( void );

private:

    void setup_shader( const float modelview_matrix[16] );

};

}

}

#endif
