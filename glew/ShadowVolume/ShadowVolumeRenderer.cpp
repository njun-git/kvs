//
//  ShadowVolumeRenderer.cpp
//  
//
//  Created by Jun Nishimura on 11/9/10.
//  Copyright 2011 Jun Nishimura. All rights reserved.
//

#include "ShadowVolumeRenderer.h"

#include <map>

namespace kvs
{

namespace glew
{

ShadowVolumeRenderer::ShadowVolumeRenderer( void )
{
    this->initialize();
    this->setShader( kvs::Shader::Phong() );
}

ShadowVolumeRenderer::~ShadowVolumeRenderer( void )
{
    this->clear();
}

void ShadowVolumeRenderer::initialize( void )
{
    m_ref_polygon = NULL;

    m_width = 0;
    m_height = 0;

    m_shader = NULL;

    m_off_coord = 0;
    m_off_color = 0;
    m_off_normal = 0;

    m_nedges = 0;

    m_shadow_strength = 0.5f;

    m_is_degug_draw = false;
}

void ShadowVolumeRenderer::clear( void )
{
    if ( m_shader )
    {
        delete m_shader;
        m_shader = NULL;
    }
}

void ShadowVolumeRenderer::attachPolygonObject( const kvs::PolygonObject* polygon )
{
    // Check polygon object for rendering.
    if ( polygon->polygonType() != kvs::PolygonObject::Triangle )
    {
        kvsMessageError( "Polygon type of this polygon is not triangle." );
        exit(1);
    }

    if ( polygon->ncolors() != polygon->nvertices() && polygon->ncolors() > 1 )
    {
        kvsMessageError( "Color type of this polygon is not vertex color." );
        exit(1);
    }

    if ( ( polygon->nnormals() != polygon->nvertices() && polygon->nnormals() ) > 0 ||
         polygon->normalType() != kvs::PolygonObject::VertexNormal ||
         polygon->nnormals() == 0 )
    {
        kvsMessageError( "Normal type of this polygon is not supported." );
        exit(1);
    }

    if ( polygon->nconnections() == 0 )
    {
        kvsMessageError( "Connectons of polygon are required." );
        exit(1);
    }

    m_ref_polygon = polygon;

    if ( m_ref_polygon->normals().pointer() == NULL ||
         m_ref_polygon->nnormals() == 0 )
    {
        BaseClass::disableShading();
    }
}

void ShadowVolumeRenderer::setShadowStrength( const float strength )
{
    m_shadow_strength = strength;
}

void ShadowVolumeRenderer::enableDebugDraw( void )
{
    m_is_degug_draw = true;
}

void ShadowVolumeRenderer::disableDebugDraw( void )
{
    m_is_degug_draw = false;
}

void ShadowVolumeRenderer::exec(
    kvs::ObjectBase* object, kvs::Camera* camera, kvs::Light* light )
{
    kvs::PolygonObject* polygon = kvs::PolygonObject::DownCast( object );
    if ( !m_ref_polygon ) this->attachPolygonObject( polygon );

    BaseClass::m_timer.start();
    this->create_image( polygon, camera, light );
    BaseClass::m_timer.stop();
}

void ShadowVolumeRenderer::create_image(
    kvs::PolygonObject* polygon, kvs::Camera* camera, kvs::Light* light )
{
    glPushAttrib( GL_CURRENT_BIT | GL_ENABLE_BIT | GL_LIGHTING_BIT );

    glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
    glEnable( GL_NORMALIZE );
    glEnable( GL_LIGHTING );

    // Following processes are executed once.
    if ( m_width == 0 && m_height == 0 )
    {
        this->initialize_shaders();
        this->download_buffer_object();
    }

    // Following processes are executed when the window size is changed.
    if ( ( m_width  != camera->windowWidth() ) ||
         ( m_height != camera->windowHeight() ) )

    {
        m_width  = camera->windowWidth();
        m_height = camera->windowHeight();

        this->initialize_framebuffer();
    }

    m_framebuffer.bind();

    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    glColorMask( GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE );
    glEnable( GL_DEPTH_TEST );
    m_volume_shader.bind();
    this->setup_volume_shader_parameter();
    this->draw_buffer_object_shadow();
    m_volume_shader.unbind();
    m_framebuffer.disable();

    glColorMask( GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE );
    glEnable( GL_TEXTURE_2D );

    m_shadow_shader.bind();
    m_shadow_texture.bind();
    this->setup_shadow_shader_parameter();
    this->draw_buffer_object();
    m_shadow_texture.unbind();
    m_shadow_shader.unbind();

    glDisable( GL_TEXTURE_2D );
    glDisable( GL_DEPTH_TEST );

    if ( m_is_degug_draw )
    {
        glMatrixMode( GL_TEXTURE ); glLoadIdentity();
        glMatrixMode( GL_MODELVIEW );  glPushMatrix(); glLoadIdentity();
        glMatrixMode( GL_PROJECTION ); glPushMatrix(); glLoadIdentity();
        glOrtho( 0.0, 1.0, 0.0, 1.0, -1.0, 1.0 );
        m_shadow_texture.bind();

        glEnable( GL_TEXTURE_2D );
        glDisable( GL_DEPTH_TEST );
        glDisable( GL_LIGHTING );
        glBegin( GL_QUADS );
        glColor4f( 1.0f, 1.0f, 1.0f, 1.0f );
        glTexCoord2f( 1.0, 1.0 );  glVertex2f( 0.5, 0.5 );
        glTexCoord2f( 0.0, 1.0 );  glVertex2f( 0.0, 0.5 );
        glTexCoord2f( 0.0, 0.0 );  glVertex2f( 0.0, 0.0 );
        glTexCoord2f( 1.0, 0.0 );  glVertex2f( 0.5, 0.0 );
        glEnd();
        glDisable( GL_TEXTURE_2D );

        m_shadow_texture.unbind();

        glMatrixMode( GL_PROJECTION ); glPopMatrix();
        glMatrixMode( GL_MODELVIEW );  glPopMatrix();
    }

    glPopAttrib();
}

void ShadowVolumeRenderer::download_buffer_object( void )
{
    // Download VBO and IBO of polygon.
    m_vbo.release();
    m_ibo.release();

    const size_t nvertices = m_ref_polygon->nvertices();
    const size_t npolygons = m_ref_polygon->nconnections();
    const size_t ncolors = m_ref_polygon->ncolors();
    const bool has_colors = ncolors > 1;

    const size_t size_v = sizeof(CoordType) * 3 * nvertices;
    const size_t size_c = has_colors ? sizeof(ColorType) * 3 * nvertices : 0;
    const size_t size_n = sizeof(NormalType) * 3 * nvertices;
    const size_t size_of_vertex = size_v + size_c + size_n;

    const size_t off_v = 0;
    const size_t off_c = off_v + size_v;
    const size_t off_n = off_c + size_c;

    const float* p_normals = m_ref_polygon->normals().pointer();
    kvs::ValueArray<NormalType> normals( 3 * nvertices );
    for ( size_t i = 0; i < nvertices; i++ )
    {
        const kvs::Vector3f v( p_normals + i * 3 );
        const kvs::Vector3f n = v.normalize() * 127.0f;
        normals[ i * 3     ] = static_cast<NormalType>(n[0]);
        normals[ i * 3 + 1 ] = static_cast<NormalType>(n[1]);
        normals[ i * 3 + 2 ] = static_cast<NormalType>(n[2]);
    }

    const CoordType*  ptr_v = m_ref_polygon->coords().pointer();
    const ColorType*  ptr_c = m_ref_polygon->colors().pointer();
    const NormalType* ptr_n = normals.pointer();

    m_vbo.create( size_of_vertex );
    m_vbo.bind();
    m_vbo.download( size_v, ptr_v, off_v );
    if ( has_colors ) m_vbo.download( size_c, ptr_c, off_c );
    m_vbo.download( size_n, ptr_n, off_n );

    const size_t size_of_polygon = sizeof(ConnectType) * 3 * npolygons;
    const ConnectType* ptr_conn = m_ref_polygon->connections().pointer();

    m_ibo.create( size_of_polygon );
    m_ibo.bind();
    m_ibo.download( size_of_polygon, ptr_conn, 0 );

    GLenum error = glGetError();
    if ( error != GL_NO_ERROR )
    {
        kvsMessageError( "Vertex Buffer Object download failed: %s(%d).", gluErrorString( error ), error );
    }

    m_off_coord  = off_v;
    m_off_color  = off_c;
    m_off_normal = off_n;

    // Download IBO of polygon edge.
    m_edge_ibo.release();

    std::multimap<size_t, kvs::Vector2ui> indices;
    std::vector<unsigned int> connections;
    const unsigned int* p_connections = m_ref_polygon->connections().pointer();

    for ( size_t i = 0; i < npolygons; i++ )
    {
        kvs::Vector2ui edges[3];
        bool has_edge[3];
        const unsigned int id0 = p_connections[ i * 3     ];
        const unsigned int id1 = p_connections[ i * 3 + 1 ];
        const unsigned int id2 = p_connections[ i * 3 + 2 ];
        edges[0].set( id0, id1 );
        edges[1].set( id1, id2 );
        edges[2].set( id2, id0 );
        has_edge[0] = has_edge[1] = has_edge[2] = false;

        for ( size_t j = 0; j < 3; j++ )
        {
            const size_t key = edges[j].x() + edges[j].y();
            const size_t nresults = indices.count( key );
            if ( nresults > 0 )
            {
                std::multimap<size_t, kvs::Vector2ui>::iterator itr = indices.find( key );
                for ( size_t k = 0; k < nresults; k++ )
                {
                    const kvs::Vector2ui r = itr->second;
                    if ( ( edges[j].x() == r.x() && edges[j].y() == r.y() ) ||
                         ( edges[j].x() == r.y() && edges[j].y() == r.x() ) )
                    {
                        has_edge[j] = true;
                    }
                    itr++;
                }
            }
        }

        for ( size_t j = 0; j < 3; j++ )
        {
            if ( !has_edge[j] )
            {
                connections.push_back( edges[j].x() );
                connections.push_back( edges[j].y() );
                indices.insert( std::multimap<size_t, kvs::Vector2ui>::value_type(
                    ( edges[j].x() + edges[j].y() ), edges[j] ) );
            }
        }
    }

    m_nedges = connections.size() / 2;

    kvs::ValueArray<unsigned int> line_connect( connections );
    const size_t size_of_line = sizeof(ConnectType) * 2 * m_nedges;
    const ConnectType* ptr_line_connect = line_connect.pointer();

    m_edge_ibo.create( size_of_line );
    m_edge_ibo.bind();
    m_edge_ibo.download( size_of_line, ptr_line_connect, 0 );

    error = glGetError();
    if ( error != GL_NO_ERROR )
    {
        kvsMessageError( "Vertex Buffer Object download failed: %s(%d).", gluErrorString( error ), error );
    }
}

void ShadowVolumeRenderer::draw_buffer_object( void )
{
    m_vbo.bind();
    m_ibo.bind();

    const size_t npolygons = m_ref_polygon->nconnections();
    const size_t ncolors = m_ref_polygon->ncolors();
    const bool has_colors = ncolors > 1;

    glEnableClientState( GL_VERTEX_ARRAY );
    glVertexPointer( 3, GL_FLOAT, 0, (char*)(m_off_coord) );

    if ( has_colors )
    {
        glEnableClientState( GL_COLOR_ARRAY );
        glColorPointer( 3, GL_UNSIGNED_BYTE, 0, (char*)(m_off_color) );
    }
    else
    {
        kvs::RGBColor color = m_ref_polygon->color();
        glColor4ub( color.r(), color.g(), color.b(), 255 );
    }

    glEnableClientState( GL_NORMAL_ARRAY );
    glNormalPointer( GL_BYTE, 0, (char*)(m_off_normal) );

    glDrawElements( GL_TRIANGLES, 3 * npolygons, GL_UNSIGNED_INT, 0 );

    glDisableClientState( GL_NORMAL_ARRAY );
    if ( has_colors ) glDisableClientState( GL_COLOR_ARRAY );
    glDisableClientState( GL_VERTEX_ARRAY );

    m_vbo.unbind();
    m_ibo.unbind();
}

void ShadowVolumeRenderer::draw_buffer_object_shadow( void )
{
    m_vbo.bind();

    glEnableClientState( GL_VERTEX_ARRAY );
    glVertexPointer( 3, GL_FLOAT, 0, (char*)(m_off_coord) );

    m_edge_ibo.bind();
    glDrawElements( GL_LINES, 2 * m_nedges, GL_UNSIGNED_INT, 0 );
    m_edge_ibo.unbind();

    glDisableClientState( GL_VERTEX_ARRAY );

    m_vbo.unbind();
}

void ShadowVolumeRenderer::initialize_shaders( void )
{
    glHint( GL_PERSPECTIVE_CORRECTION_HINT, GL_FASTEST );
    // Initialize the shader for shadow volume.
    {
        const std::string vert_code = "ShadowVolumeShader/volume.vert";
        const std::string geom_code = "ShadowVolumeShader/volume.geom";
        const std::string frag_code = "ShadowVolumeShader/volume.frag";

        kvs::glew::ShaderSource vert( vert_code );
        kvs::glew::ShaderSource geom( geom_code );
        kvs::glew::ShaderSource frag( frag_code );

        this->create_shaders( m_volume_shader, vert, geom, frag );
    }

    // Initialize the shader for shadow volume.
    {
        const std::string vert_code = "ShadowVolumeShader/shadow.vert";
        const std::string frag_code = "ShadowVolumeShader/shadow.frag";

        kvs::glew::ShaderSource vert( vert_code );
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

        this->create_shaders( m_shadow_shader, vert, frag );

        if ( BaseClass::isShading() )
        {
            m_shadow_shader.bind();
            switch ( m_shader->type() )
            {
            case kvs::Shader::LambertShading:
            {
                const GLfloat Ka = ((kvs::Shader::Lambert*)(m_shader))->Ka;
                const GLfloat Kd = ((kvs::Shader::Lambert*)(m_shader))->Kd;
                m_shadow_shader.setUniformValuef( "shading.Ka", Ka );
                m_shadow_shader.setUniformValuef( "shading.Kd", Kd );
                break;
            }
            case kvs::Shader::PhongShading:
            {
                const GLfloat Ka = ((kvs::Shader::Phong*)(m_shader))->Ka;
                const GLfloat Kd = ((kvs::Shader::Phong*)(m_shader))->Kd;
                const GLfloat Ks = ((kvs::Shader::Phong*)(m_shader))->Ks;
                const GLfloat S  = ((kvs::Shader::Phong*)(m_shader))->S;
                m_shadow_shader.setUniformValuef( "shading.Ka", Ka );
                m_shadow_shader.setUniformValuef( "shading.Kd", Kd );
                m_shadow_shader.setUniformValuef( "shading.Ks", Ks );
                m_shadow_shader.setUniformValuef( "shading.S",  S );
                break;
            }
            case kvs::Shader::BlinnPhongShading:
            {
                const GLfloat Ka = ((kvs::Shader::BlinnPhong*)(m_shader))->Ka;
                const GLfloat Kd = ((kvs::Shader::BlinnPhong*)(m_shader))->Kd;
                const GLfloat Ks = ((kvs::Shader::BlinnPhong*)(m_shader))->Ks;
                const GLfloat S  = ((kvs::Shader::BlinnPhong*)(m_shader))->S;
                m_shadow_shader.setUniformValuef( "shading.Ka", Ka );
                m_shadow_shader.setUniformValuef( "shading.Kd", Kd );
                m_shadow_shader.setUniformValuef( "shading.Ks", Ks );
                m_shadow_shader.setUniformValuef( "shading.S",  S );
                break;
            }
            default: /* NO SHADING */ break;
            }
            m_shadow_shader.unbind();
        }
    }
}

void ShadowVolumeRenderer::initialize_framebuffer( void )
{
    m_framebuffer.create();
    m_framebuffer.bind();

    glDrawBuffer( GL_NONE );
    glReadBuffer( GL_NONE );

    m_shadow_texture.release();

    m_shadow_texture.setWrapS( GL_CLAMP_TO_EDGE );
    m_shadow_texture.setWrapT( GL_CLAMP_TO_EDGE );
    m_shadow_texture.setMagFilter( GL_LINEAR );
    m_shadow_texture.setMinFilter( GL_LINEAR );

    m_shadow_texture.setPixelFormat( GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT, GL_FLOAT );
    m_shadow_texture.create( m_width, m_height );
    {
        GLenum error = glGetError();
        if ( error != GL_NO_ERROR )
        {
            kvsMessageError( "depth buffer allocation failed: %s.", gluErrorString(error));
            exit( EXIT_FAILURE );
        }
    }

    m_framebuffer.attachDepthTexture( m_shadow_texture );

    m_shadow_texture.unbind();
    m_framebuffer.disable();

    GLenum error = glGetError();
    if ( error != GL_NO_ERROR )
    {
        kvsMessageError( "framebuffer allocation failed: %s.", gluErrorString(error));
        exit( EXIT_FAILURE );
    }
}

void ShadowVolumeRenderer::create_shaders(
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

    glProgramParameteriEXT( program_object.id(), GL_GEOMETRY_INPUT_TYPE_EXT, GL_LINES );
    glProgramParameteriEXT( program_object.id(), GL_GEOMETRY_OUTPUT_TYPE_EXT, GL_TRIANGLE_STRIP );
    GLint vertices_out = 4;
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

void ShadowVolumeRenderer::create_shaders(
    kvs::glew::ProgramObject& program_object,
    const kvs::glew::ShaderSource& vertex_source,
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
    program_object.attach( vertex_shader );

    if ( !program_object.link() )
    {
        GLenum error = glGetError();
        kvsMessageError( "ShaderProgram link failed: %s(%d)", gluErrorString(error), error );
        std::cout << "error log:" << std::endl;
        std::cout << program_object.log() << std::endl;
        exit(1);
    }
}

void ShadowVolumeRenderer::setup_volume_shader_parameter( void )
{
}

void ShadowVolumeRenderer::setup_shadow_shader_parameter( void )
{
    m_shadow_shader.setUniformValuei( "shadow", 0 );
    m_shadow_shader.setUniformValuef( "strength", m_shadow_strength );
    m_shadow_shader.setUniformValuef( "texture_size_inv", 1.0f / m_width, 1.0f / m_height );
}

}

}
