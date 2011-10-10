/*
 *  TerrainRenderer.cpp
 *  
 *
 *  Created by njun on 11/10/11.
 *  Copyright 2011 Jun Nishimura. All rights reserved.
 *
 */

#include "TerrainRenderer.h"

namespace kvs
{

namespace glew
{

TerrainRenderer::TerrainRenderer( void )
{
    this->initialize();
    this->setShader( kvs::Shader::Lambert() );
}

TerrainRenderer::~TerrainRenderer( void )
{
    this->clear();
}

void TerrainRenderer::initialize( void )
{
    m_ref_terrain = NULL;
    m_image = NULL;

    m_is_initialize = false;
    m_is_downloaded = false;

    m_shader = NULL;
}

void TerrainRenderer::clear( void )
{
    if ( m_shader )
    {
        delete m_shader;
        m_shader = NULL;
    }
}

void TerrainRenderer::attachTerrainObject( const kvs::TerrainObject* terrain )
{
    m_ref_terrain = terrain;
    m_is_downloaded = false;
}

void TerrainRenderer::attachImage( kvs::PaintImageObject* image )
{
    m_image = image;
}

void TerrainRenderer::exec( kvs::ObjectBase* object, kvs::Camera* camera, kvs::Light* light )
{
    kvs::TerrainObject* terrain = reinterpret_cast<kvs::TerrainObject*>( object );
    if ( !m_ref_terrain ) this->attachTerrainObject( terrain );

    if ( !m_is_initialize ) this->initialize_shader();
    if ( !m_is_downloaded ) this->download_buffer_object();

    glPushAttrib( GL_CURRENT_BIT | GL_ENABLE_BIT );

    RendererBase::initialize();
    if( !BaseClass::isShading() )
    {
        glDisable( GL_NORMALIZE );
        glDisable( GL_LIGHTING );
    }
    else
    {
        glEnable( GL_NORMALIZE );
        glEnable( GL_LIGHTING );
    }

    glEnable( GL_TEXTURE_2D );

    m_texture.bind();
    if ( m_image )
    {
        m_texture.download( m_image->width(), m_image->height(), m_image->data().pointer() );
    }
    m_vbo.bind();
    m_ibo.bind();
    m_terrain_shader.bind();
    glEnable( GL_DEPTH_TEST );
    {
        BaseClass::timer().start();
        this->setup_shader_parameter();
        this->draw_buffer_object();
        BaseClass::timer().stop();
    }
    glDisable( GL_DEPTH_TEST );
    m_terrain_shader.unbind();
    m_vbo.unbind();
    m_ibo.unbind();
/*
    glMatrixMode( GL_MODELVIEW );
    glPushMatrix();
    {
        glLoadIdentity();

        glMatrixMode( GL_PROJECTION );
        glPushMatrix();
        {
            glLoadIdentity();
            glOrtho( 0, 1, 0, 1, -1, 1 );

            glBegin( GL_QUADS );
            glTexCoord2f( 0.0, 0.0 ); glVertex2f(  0.0,  1.0 );
            glTexCoord2f( 0.0, 1.0 ); glVertex2f(  0.0,  0.0 );
            glTexCoord2f( 1.0, 1.0 ); glVertex2f(  1.0,  0.0 );
            glTexCoord2f( 1.0, 0.0 ); glVertex2f(  1.0,  1.0 );
            glEnd();
        }
        glPopMatrix();
        glMatrixMode( GL_MODELVIEW );
    }
    glPopMatrix();
*/
    m_texture.unbind();

    glDisable( GL_TEXTURE_2D );

    glPopAttrib();
}

void TerrainRenderer::download_buffer_object( void )
{
    m_vbo.release();
    m_ibo.release();

    const size_t nvertices = m_ref_terrain->nvertices();
    const size_t size_of_vertex = sizeof(CoordType) * 3 * nvertices;
    const CoordType* ptr_vertex = m_ref_terrain->coords().pointer();

    m_vbo.create( size_of_vertex );
    m_vbo.bind();
    m_vbo.download( size_of_vertex, ptr_vertex, 0 );

    const size_t npolygons = m_ref_terrain->npolygons();
    const size_t size_of_polygon = sizeof(ConnectType) * 3 * npolygons;
    const ConnectType* ptr_connect = m_ref_terrain->connections().pointer();
    m_ibo.create( size_of_polygon );
    m_ibo.bind();
    m_ibo.download( size_of_polygon, ptr_connect, 0 );

    GLenum error = glGetError();
    if ( error != GL_NO_ERROR )
    {
        kvsMessageError( "Vertex Buffer Object download failed: %s(%d).", gluErrorString( error ), error );
    }

    m_texture.release();

    m_texture.setWrapS( GL_CLAMP_TO_EDGE );
    m_texture.setWrapT( GL_CLAMP_TO_EDGE );
    m_texture.setMagFilter( GL_LINEAR );
    m_texture.setMinFilter( GL_LINEAR );

    m_texture.setPixelFormat( GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT, GL_FLOAT );
    if ( !m_image )
    {
        kvsMessageError( "No image." );
        exit( EXIT_FAILURE );
    }
    m_texture.create( m_image->width(), m_image->height() );
    {
        GLenum error = glGetError();
        if ( error != GL_NO_ERROR )
        {
            kvsMessageError( "depth buffer allocation failed: %s.", gluErrorString(error));
            exit( EXIT_FAILURE );
        }
    }
    m_texture.unbind();

    m_is_downloaded = true;
}

void TerrainRenderer::draw_buffer_object( void )
{
    glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );

    glEnableClientState( GL_VERTEX_ARRAY );
    glVertexPointer( 3, GL_FLOAT, 0, 0 );

    const size_t npolygons = m_ref_terrain->npolygons();
    glDrawElements( GL_TRIANGLES, 3 * npolygons, GL_UNSIGNED_INT, 0 );

    glDisableClientState( GL_VERTEX_ARRAY );
}

void TerrainRenderer::initialize_shader( void )
{
    const std::string vert_code = "TerrainShader/terrain.vert";
    const std::string geom_code = "TerrainShader/terrain.geom";
    const std::string frag_code = "TerrainShader/terrain.frag";

    kvs::glew::ShaderSource vert( vert_code );
    kvs::glew::ShaderSource geom( geom_code );
    kvs::glew::ShaderSource frag( frag_code );

    if ( BaseClass::isShading() )
    {
        switch ( m_shader->type() )
        {
        case kvs::Shader::LambertShading: frag.define( "ENABLE_LAMBERT_SHADING" ); break;
        case kvs::Shader::PhongShading: frag.define( "ENABLE_PHONG_SHADING" ); break;
        case kvs::Shader::BlinnPhongShading: frag.define( "ENABLE_BLINN_PHONG_SHADING" ); break;
        default: /* NO SHADING */ break;
        }
    }

    this->create_shaders( m_terrain_shader, vert, geom, frag );

    m_terrain_shader.bind();
    switch ( m_shader->type() )
    {
    case kvs::Shader::LambertShading:
    {
        const GLfloat Ka = ((kvs::Shader::Lambert*)(m_shader))->Ka;
        const GLfloat Kd = ((kvs::Shader::Lambert*)(m_shader))->Kd;
        m_terrain_shader.setUniformValuef( "shading.Ka", Ka );
        m_terrain_shader.setUniformValuef( "shading.Kd", Kd );
        break;
    }
    case kvs::Shader::PhongShading:
    {
        const GLfloat Ka = ((kvs::Shader::Phong*)(m_shader))->Ka;
        const GLfloat Kd = ((kvs::Shader::Phong*)(m_shader))->Kd;
        const GLfloat Ks = ((kvs::Shader::Phong*)(m_shader))->Ks;
        const GLfloat S  = ((kvs::Shader::Phong*)(m_shader))->S;
        m_terrain_shader.setUniformValuef( "shading.Ka", Ka );
        m_terrain_shader.setUniformValuef( "shading.Kd", Kd );
        m_terrain_shader.setUniformValuef( "shading.Ks", Ks );
        m_terrain_shader.setUniformValuef( "shading.S",  S );
        break;
    }
    case kvs::Shader::BlinnPhongShading:
    {
        const GLfloat Ka = ((kvs::Shader::BlinnPhong*)(m_shader))->Ka;
        const GLfloat Kd = ((kvs::Shader::BlinnPhong*)(m_shader))->Kd;
        const GLfloat Ks = ((kvs::Shader::BlinnPhong*)(m_shader))->Ks;
        const GLfloat S  = ((kvs::Shader::BlinnPhong*)(m_shader))->S;
        m_terrain_shader.setUniformValuef( "shading.Ka", Ka );
        m_terrain_shader.setUniformValuef( "shading.Kd", Kd );
        m_terrain_shader.setUniformValuef( "shading.Ks", Ks );
        m_terrain_shader.setUniformValuef( "shading.S",  S );
        break;
    }
    default: /* NO SHADING */ break;
    }
    m_terrain_shader.unbind();

    m_is_initialize = true;
}

void TerrainRenderer::create_shaders(
    kvs::glew::ProgramObject& program_object,
    const kvs::glew::ShaderSource& vertex_source,
    const kvs::glew::ShaderSource& geometry_source,
    const kvs::glew::ShaderSource& fragment_source )
{
    // Vertex shader.
    kvs::glew::VertexShader vertex_shader;
    if ( !vertex_shader.create( vertex_source ) )
    {
        GLenum error = glGetError();
        kvsMessageError( "VertexShader compile failed: %s(%d)¥n", gluErrorString(error), error );
        std::cout << "error log:" << std::endl;
        std::cout << vertex_shader.log() << std::endl;
    }

    // Geometry shader.
    kvs::glew::GeometryShader geometry_shader;
    if ( !geometry_shader.create( geometry_source ) )
    {
        GLenum error = glGetError();
        kvsMessageError( "GeometryShader compile failed: %s(%d)¥n", gluErrorString(error), error );
        std::cout << "error log:" << std::endl;
        std::cout << geometry_shader.log() << std::endl;
    }

    // Fragment shader.
    kvs::glew::FragmentShader fragment_shader;
    if ( !fragment_shader.create( fragment_source ) )
    {
        GLenum error = glGetError();
        kvsMessageError( "FragmentShader compile failed: %s(%d)¥n", gluErrorString(error), error );
        std::cout << "error log:" << std::endl;
        std::cout << fragment_shader.log() << std::endl;
    }

    // Link the shaders.
    program_object.create();
    program_object.attach( fragment_shader );
    program_object.attach( geometry_shader );
    program_object.attach( vertex_shader );

    glProgramParameteriEXT( program_object.id(), GL_GEOMETRY_INPUT_TYPE_EXT, GL_TRIANGLES );
    glProgramParameteriEXT( program_object.id(), GL_GEOMETRY_OUTPUT_TYPE_EXT, GL_TRIANGLE_STRIP );
    GLint vertices_out = 3;
    glProgramParameteriEXT( program_object.id(), GL_GEOMETRY_VERTICES_OUT_EXT, vertices_out );

    if ( !program_object.link() )
    {
        GLenum error = glGetError();
        kvsMessageError( "ShaderProgram link failed: %s(%d)", gluErrorString(error), error );
        std::cout << "error log:" << std::endl;
        std::cout << program_object.log() << std::endl;
        exit(1);
    }
}

void TerrainRenderer::setup_shader_parameter( void )
{
    m_terrain_shader.setUniformValuei( "texture", 0 );
    m_terrain_shader.setUniformValuef( "texture_size_inv", 1.0f / m_texture.width(), 1.0f / m_texture.height() );
}

}

}
