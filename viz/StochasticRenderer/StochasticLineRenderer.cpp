//
//  StochasticLineRenderer.cpp
//  
//
//  Created by Jun Nishimura on 11/4/10.
//  Copyright 2011 Jun Nishimura. All rights reserved.
//

#include "StochasticLineRenderer.h"

namespace kvs
{

namespace glew
{

StochasticLineRenderer::StochasticLineRenderer( void )
{
    this->initialize();
}

StochasticLineRenderer::StochasticLineRenderer( const kvs::LineObject* line )
{
    this->initialize();
    this->attachLineObject( line );
}

StochasticLineRenderer::~StochasticLineRenderer( void )
{
}

void StochasticLineRenderer::exec(
    kvs::ObjectBase* object,
    kvs::Camera*     camera,
    kvs::Light*      light )
{
    // This function is not called.
}

void StochasticLineRenderer::initialize( void )
{
    BaseClass::initialize();

    m_ref_line = NULL;

    m_loc_identifier = 0;
    m_off_index = 0;
    m_off_coord = 0;
    m_off_color = 0;

    m_line_opacity = 1.0f;
}

void StochasticLineRenderer::attachLineObject( const kvs::LineObject* line )
{
    // Check polygon object for rendering.
    if ( line->ncolors() != line->nvertices() && line->ncolors() > 1 )
    {
        kvsMessageError( "Color type of this line is not vertex color." );
        exit(1);
    }

    m_ref_line = line;

    BaseClass::disableShading();
}

void StochasticLineRenderer::setOpacity( const unsigned char opacity )
{
    m_line_opacity = static_cast<float>( opacity / 255.0f );
}

const StochasticRendererBase::RendererType StochasticLineRenderer::rendererType( void ) const
{
    return( BaseClass::Line );
}

void StochasticLineRenderer::initializeShader( void )
{
    const std::string vert_code = "StochasticShader/line.vert";
    const std::string frag_code = "StochasticShader/line.frag";

    kvs::glew::ShaderSource vert( vert_code );
    kvs::glew::ShaderSource frag( frag_code );

    this->create_shaders( m_shader_program, vert, frag );
    m_loc_identifier = m_shader_program.attributeLocation( "identifier" );
}

void StochasticLineRenderer::createVertexBuffer( void )
{
    m_vbo.release();
    m_ibo.release();

    const size_t nvertices = m_ref_line->nvertices();
    const size_t nlines = m_ref_line->nconnections();

    const bool has_color    = m_ref_line->ncolors() > 1;
    const bool has_connect  = m_ref_line->nconnections() > 0 &&
        ( m_ref_line->lineType() == kvs::LineObject::Segment ||
          m_ref_line->lineType() == kvs::LineObject::Uniline );

    const size_t size_i = sizeof(IndexType) * 2 * nvertices;
    const size_t size_v = sizeof(CoordType) * 3 * nvertices;
    const size_t size_c = has_color ? sizeof(ColorType) * 3 * nvertices : 0;
    const size_t size_of_vertex = size_i + size_v + size_c;

    const size_t off_i = 0;
    const size_t off_v = off_i + size_i;
    const size_t off_c = off_v + size_v;

    const size_t random_texture_size = m_random_texture.width();
    kvs::ValueArray<IndexType> indices( 2 * nvertices );
    for ( size_t i = 0; i < nvertices; i++ )
    {
        unsigned int index = i * 12347;
        indices[ i * 2     ] = static_cast<IndexType>( index % random_texture_size );
        indices[ i * 2 + 1 ] = static_cast<IndexType>( ( index / random_texture_size ) % random_texture_size );
    }

    const IndexType*  ptr_i = indices.pointer();
    const CoordType*  ptr_v = m_ref_line->coords().pointer();
    const ColorType*  ptr_c = m_ref_line->colors().pointer();

    m_vbo.create( size_of_vertex );
    m_vbo.bind();
    m_vbo.download( size_i, ptr_i, off_i );
    m_vbo.download( size_v, ptr_v, off_v );
    if ( has_color )  m_vbo.download( size_c, ptr_c, off_c );

    if ( has_connect )
    {
        const size_t size_of_line = sizeof(ConnectType) * 2 * nlines;
        const ConnectType* ptr_conn = m_ref_line->connections().pointer();

        m_ibo.create( size_of_line );
        m_ibo.bind();
        m_ibo.download( size_of_line, ptr_conn, 0 );
    }

    GLenum error = glGetError();
    if ( error != GL_NO_ERROR )
    {
        kvsMessageError( "Vertex Buffer Object download failed: %s(%d).", gluErrorString( error ), error );
    }

    m_off_index  = off_i;
    m_off_coord  = off_v;
    m_off_color  = off_c;
}

void StochasticLineRenderer::downloadVertexBuffer( void )
{
    this->createVertexBuffer();
}

void StochasticLineRenderer::drawVertexBuffer( const float modelview_matrix[16] )
{
    m_vbo.bind();
    m_ibo.bind();

    glActiveTexture(GL_TEXTURE0);     m_random_texture.bind();          glEnable(GL_TEXTURE_2D);

    m_shader_program.bind();

    this->setup_shader( modelview_matrix );

    const size_t nlines = m_ref_line->nconnections();

    const bool has_color    = m_ref_line->ncolors() > 1;
    const bool has_connect  = m_ref_line->nconnections() > 0 &&
        ( m_ref_line->lineType() == kvs::LineObject::Segment ||
          m_ref_line->lineType() == kvs::LineObject::Uniline );

    glPolygonOffset( 1.0, 0.0 );
    glEnable( GL_POLYGON_OFFSET_FILL );
    glEnable( GL_LINE_SMOOTH );

    const float width = m_ref_line->size();
    glLineWidth( width );

    glEnableClientState( GL_VERTEX_ARRAY );
    glVertexPointer( 3, GL_FLOAT, 0, (char*)(m_off_coord) );

    glEnableVertexAttribArray( m_loc_identifier );
    glVertexAttribPointer( m_loc_identifier, 2, GL_UNSIGNED_SHORT, GL_FALSE, 0, (char*)(m_off_index) );

    kvs::RGBColor color = m_ref_line->color();
    glColor3ub( color.r(), color.g(), color.b() );

    if ( has_color )
    {
        glEnableClientState( GL_COLOR_ARRAY );
        glColorPointer( 3, GL_UNSIGNED_BYTE, 0, (char*)(m_off_color) );
    }

    if ( has_connect )
    {
        switch ( m_ref_line->lineType() )
        {
            case kvs::LineObject::Uniline:
            {
                glDrawElements( GL_LINE_STRIP, nlines, GL_UNSIGNED_INT, 0 );
                break;
            }
            case kvs::LineObject::Segment:
            {
                glDrawElements( GL_LINES, 2 * nlines, GL_UNSIGNED_INT, 0 );
                break;
            }
            default: break;
        }
    }
    else
    {
        switch ( m_ref_line->lineType() )
        {
            case kvs::LineObject::Polyline:
            {
                size_t count = 0;
                const unsigned int* p_connect = m_ref_line->connections().pointer();
                for ( size_t i = 0; i < nlines; i++ )
                {
                    const size_t nvertices = p_connect[ i * 2 + 1 ] - p_connect[ i * 2 ] + 1;
                    glDrawArrays( GL_LINE_STRIP, count, nvertices );
                    count += nvertices;
                }
                break;
            }
            default:
            {
                const size_t nvertices = m_ref_line->nvertices();
                glDrawArrays( GL_LINE_STRIP, 0, nvertices );
                break;
            }
        }
    }

    if ( has_color )  glDisableClientState( GL_COLOR_ARRAY );
    glDisableClientState( GL_VERTEX_ARRAY );
    glDisableVertexAttribArray( m_loc_identifier );

    m_shader_program.unbind();

    glActiveTexture(GL_TEXTURE0);    m_random_texture.unbind();         glDisable(GL_TEXTURE_2D);
}

void StochasticLineRenderer::clearEnsembleBuffer( void )
{
    m_repeat_count = 0;
}

void StochasticLineRenderer::setup_shader( const float modelview_matrix[16] )
{
    const size_t random_texture_size = m_random_texture.width();
    int scramble_count = m_repeat_count++ * 12347;
    float rp_x = ( scramble_count                       ) % random_texture_size;
    float rp_y = ( scramble_count / random_texture_size ) % random_texture_size;

    const GLfloat random_texture_size_inv = 1.0f / random_texture_size;

    const GLfloat screen_scale_x = m_width * 0.5f;
    const GLfloat screen_scale_y = m_height * 0.5f;

    m_shader_program.setUniformValuef( "random_texture_size_inv", random_texture_size_inv );
    m_shader_program.setUniformValuef( "random_offset", rp_x, rp_y );

    m_shader_program.setUniformValuef( "screen_scale", screen_scale_x, screen_scale_y );
    m_shader_program.setUniformValuef( "screen_scale_inv", 1.0 / m_width, 1.0 / m_height );

    m_shader_program.setUniformValuei( "random_texture", 0 );

    m_shader_program.setUniformValuef( "opacity", m_line_opacity );
}

}

}
