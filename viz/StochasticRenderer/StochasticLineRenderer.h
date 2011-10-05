//
//  StochasticLineRenderer.h
//  
//
//  Created by Jun Nishimura on 11/4/10.
//  Copyright 2011 Jun Nishimura. All rights reserved.
//

#ifndef KVS__GLEW__STOCHASTIC_LINE_RENDERER_H_INCLUDE
#define KVS__GLEW__STOCHASTIC_LINE_RENDERER_H_INCLUDE

#include "StochasticRendererBase.h"

#include <kvs/LineObject>

#include <kvs/glew/VertexBufferObject>
#include <kvs/glew/IndexBufferObject>

namespace kvs
{

namespace glew
{

class StochasticLineRenderer : public kvs::glew::StochasticRendererBase
{

    // Class name.
    kvsClassName( kvs::glew::StochasticLineRenderer );

    // Module information.
    kvsModuleCategory( Renderer );
    kvsModuleBaseClass( kvs::glew::StochasticRendererBase );

protected:

    typedef GLushort IndexType;
    typedef GLfloat  CoordType;
    typedef GLubyte  ColorType;
    typedef GLuint   ConnectType;

    // Reference only.
    const kvs::LineObject*  m_ref_line;

    kvs::glew::VertexBufferObject   m_vbo;
    kvs::glew::IndexBufferObject    m_ibo;

    // Offset for VBO drawing.
    size_t  m_loc_identifier; ///< loc indectifier
    size_t  m_off_index;
    size_t  m_off_coord;
    size_t  m_off_color;

    float   m_line_opacity;

public:

    StochasticLineRenderer( void );

    StochasticLineRenderer( const kvs::LineObject* line );

    virtual ~StochasticLineRenderer( void );

public:

    void exec( kvs::ObjectBase* object, kvs::Camera* camera, kvs::Light* light );

public:

    void initialize( void );

    void attachLineObject( const kvs::LineObject* line );

    void setOpacity( const unsigned char opacity );

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
