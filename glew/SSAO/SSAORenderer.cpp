//
//  SSAORenderer.cpp
//  
//
//  Created by Jun Nishimura on 11/26/9.
//  Copyright 2011 Jun Nishimura. All rights reserved.
//

#include "SSAORenderer.h"

#include <kvs/Camera>
#include <kvs/Light>
#include <kvs/ObjectBase>
#include <kvs/IgnoreUnusedVariable>

namespace kvs
{

namespace glew
{

SSAORenderer::SSAORenderer( void ) :
    m_ref_polygon( NULL ),
    m_shader( NULL )
{
    this->initialize();
    this->setShader( kvs::Shader::Phong() );
}

SSAORenderer::~SSAORenderer( void )
{
    if ( m_shader )
    {
        delete m_shader;
        m_shader = NULL;
    }
}

void SSAORenderer::initialize( void )
{
    m_is_debug_draw = false;

    m_width = 0;
    m_height = 0;

    m_off_coord = 0;
    m_off_color = 0;
    m_off_normal = 0;
}

void SSAORenderer::attachPolygonObject( const kvs::PolygonObject* polygon )
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

    if ( polygon->nnormals() != polygon->nvertices() && polygon->nnormals() > 0 ||
         polygon->normalType() != kvs::PolygonObject::VertexNormal ||
         polygon->nnormals() == 0 )
    {
        kvsMessageError( "Normal type of this polygon is not supported." );
        exit(1);
    }

    m_ref_polygon = polygon;

    if ( m_ref_polygon->normals().pointer() == NULL ||
         m_ref_polygon->nnormals() == 0 )
    {
        BaseClass::disableShading();
    }
}

void SSAORenderer::enableDebugDraw( void )
{
    m_is_debug_draw = true;
}

void SSAORenderer::disableDebugDraw( void )
{
    m_is_debug_draw = false;
}

void SSAORenderer::exec( kvs::ObjectBase* object, kvs::Camera* camera, kvs::Light* light )
{
    kvs::PolygonObject* polygon = kvs::PolygonObject::DownCast( object );
    if ( !m_ref_polygon ) this->attachPolygonObject( polygon );

    BaseClass::m_timer.start();
    this->create_image( polygon, camera, light );
    BaseClass::m_timer.stop();
}

void SSAORenderer::create_image( kvs::PolygonObject* polygon, kvs::Camera* camera, kvs::Light* light )
{
    kvs::IgnoreUnusedVariable( light );

    glPushAttrib( GL_CURRENT_BIT | GL_ENABLE_BIT | GL_LIGHTING_BIT );

    RendererBase::initialize();
    polygon->applyMaterial();

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

    // First render path to framebuffer object.
    m_framebuffer.bind();

    const GLenum buffers[] = {
        GL_COLOR_ATTACHMENT0_EXT,
        GL_COLOR_ATTACHMENT1_EXT,
        GL_COLOR_ATTACHMENT2_EXT,
    };

    glDrawBuffers( 3, buffers );

    glClearColor( 0.0f, 0.0f, 0.0f, 0.0f );
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    glEnable( GL_DEPTH_TEST );

    // Render polygon to framebuffer object.
    m_object_shader.bind();
    m_vbo.bind();
    m_ibo.bind();
    this->draw_buffer_object();
    m_vbo.unbind();
    m_ibo.unbind();
    m_object_shader.unbind();

    // Second render path to screen.
    m_framebuffer.disable();
    glDrawBuffer( GL_BACK );

    m_ssao_shader.bind();

    GLfloat projection[16];
    glGetFloatv( GL_PROJECTION_MATRIX, projection );

    glMatrixMode( GL_MODELVIEW );  glPushMatrix(); glLoadIdentity();
    glMatrixMode( GL_PROJECTION ); glPushMatrix(); glLoadIdentity();
    glOrtho( 0, 1, 0, 1, -1, 1 );

    glActiveTexture( GL_TEXTURE0 ); m_color_texture.bind();
    glActiveTexture( GL_TEXTURE1 ); m_position_texture.bind();
    glActiveTexture( GL_TEXTURE2 ); m_normal_texture.bind();
    glActiveTexture( GL_TEXTURE3 ); m_depth_texture.bind();

    this->setup_shader_parameter( projection );

    glDisable( GL_DEPTH_TEST );
    glDisable( GL_LIGHTING );
    glBegin( GL_QUADS );
    glColor4f( 1.0f, 1.0f, 1.0f, 1.0f );
    glTexCoord2f( 1, 1 );  glVertex2f( 1, 1 );
    glTexCoord2f( 0, 1 );  glVertex2f( 0, 1 );
    glTexCoord2f( 0, 0 );  glVertex2f( 0, 0 );
    glTexCoord2f( 1, 0 );  glVertex2f( 1, 0 );
    glEnd();

    glActiveTexture( GL_TEXTURE3 ); m_depth_texture.unbind();
    glActiveTexture( GL_TEXTURE2 ); m_normal_texture.unbind();
    glActiveTexture( GL_TEXTURE1 ); m_position_texture.unbind();
    glActiveTexture( GL_TEXTURE0 ); m_color_texture.unbind();

    m_ssao_shader.unbind();

    if ( m_is_debug_draw )
    {
        glEnable( GL_TEXTURE_2D );
        glColor4f( 1.0f, 1.0f, 1.0f, 1.0f );

        m_depth_texture.bind();
        glBegin( GL_QUADS );
        glTexCoord2f( 1, 1 );  glVertex2f( 0.25, 0.25 );
        glTexCoord2f( 0, 1 );  glVertex2f( 0.0f, 0.25 );
        glTexCoord2f( 0, 0 );  glVertex2f( 0.0f, 0.0f );
        glTexCoord2f( 1, 0 );  glVertex2f( 0.25, 0.0f );
        glEnd();
        m_depth_texture.unbind();

        m_normal_texture.bind();
        glBegin( GL_QUADS );
        glTexCoord2f( 1, 1 );  glVertex2f( 0.5f, 0.25 );
        glTexCoord2f( 0, 1 );  glVertex2f( 0.25, 0.25 );
        glTexCoord2f( 0, 0 );  glVertex2f( 0.25, 0.0f );
        glTexCoord2f( 1, 0 );  glVertex2f( 0.5f, 0.0f );
        glEnd();
        m_normal_texture.unbind();

        m_color_texture.bind();
        glBegin( GL_QUADS );
        glTexCoord2f( 1, 1 );  glVertex2f( 0.75, 0.25 );
        glTexCoord2f( 0, 1 );  glVertex2f( 0.5f, 0.25 );
        glTexCoord2f( 0, 0 );  glVertex2f( 0.5f, 0.0f );
        glTexCoord2f( 1, 0 );  glVertex2f( 0.75, 0.0f );
        glEnd();
        m_color_texture.unbind();

        m_position_texture.bind();
        glBegin( GL_QUADS );
        glTexCoord2f( 1, 1 );  glVertex2f( 1.0f, 0.25 );
        glTexCoord2f( 0, 1 );  glVertex2f( 0.75, 0.25 );
        glTexCoord2f( 0, 0 );  glVertex2f( 0.75, 0.0f );
        glTexCoord2f( 1, 0 );  glVertex2f( 1.0f, 0.0f );
        glEnd();
        m_position_texture.unbind();

        glDisable( GL_TEXTURE_2D );
    }

    glMatrixMode( GL_PROJECTION ); glPopMatrix();
    glMatrixMode( GL_MODELVIEW );  glPopMatrix();

    glPopAttrib();
}

void SSAORenderer::download_buffer_object( void )
{
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
}

void SSAORenderer::draw_buffer_object( void )
{
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

    const kvs::Vector3f center = m_ref_polygon->objectCenter();
    const kvs::Vector3f min_object_coord = m_ref_polygon->minObjectCoord();
    const kvs::Vector3f max_object_coord = m_ref_polygon->maxObjectCoord();
    const kvs::Vector3f length = max_object_coord - min_object_coord;
    const kvs::Vector3f min_coord = center - 3 * kvs::Vector3f( kvs::Math::Max( length.x(), length.y(), length.z() ) );
    const kvs::Vector3f max_coord = center + 3 * kvs::Vector3f( kvs::Math::Max( length.x(), length.y(), length.z() ) );

    glColor4ub( 192, 192, 192, 255 );
    glBegin( GL_TRIANGLE_STRIP );
        glNormal3f( 0.0f, 1.0f, 0.0f );
        glVertex3f( min_coord.x(), min_coord.y(), min_coord.z() );
        glVertex3f( min_coord.x(), min_coord.y(), max_coord.z() );
        glVertex3f( max_coord.x(), min_coord.y(), min_coord.z() );
        glVertex3f( max_coord.x(), min_coord.y(), max_coord.z() );
    glEnd();

    glBegin( GL_TRIANGLE_STRIP );
        glNormal3f( 0.0f, - 1.0f, 0.0f );
        glVertex3f( min_coord.x(), max_coord.y(), min_coord.z() );
        glVertex3f( min_coord.x(), max_coord.y(), max_coord.z() );
        glVertex3f( max_coord.x(), max_coord.y(), min_coord.z() );
        glVertex3f( max_coord.x(), max_coord.y(), max_coord.z() );
    glEnd();

    glBegin( GL_TRIANGLE_STRIP );
        glNormal3f( 1.0f, 0.0f, 0.0f );
        glVertex3f( min_coord.x(), min_coord.y(), min_coord.z() );
        glVertex3f( min_coord.x(), max_coord.y(), min_coord.z() );
        glVertex3f( min_coord.x(), min_coord.y(), max_coord.z() );
        glVertex3f( min_coord.x(), max_coord.y(), max_coord.z() );
    glEnd();

    glBegin( GL_TRIANGLE_STRIP );
        glNormal3f( - 1.0f, 0.0f, 0.0f );
        glVertex3f( max_coord.x(), min_coord.y(), min_coord.z() );
        glVertex3f( max_coord.x(), max_coord.y(), min_coord.z() );
        glVertex3f( max_coord.x(), min_coord.y(), max_coord.z() );
        glVertex3f( max_coord.x(), max_coord.y(), max_coord.z() );
    glEnd();

    glBegin( GL_TRIANGLE_STRIP );
        glNormal3f( 0.0f, 0.0f, 1.0f );
        glVertex3f( min_coord.x(), min_coord.y(), min_coord.z() );
        glVertex3f( min_coord.x(), max_coord.y(), min_coord.z() );
        glVertex3f( max_coord.x(), min_coord.y(), min_coord.z() );
        glVertex3f( max_coord.x(), max_coord.y(), min_coord.z() );
    glEnd();

    glBegin( GL_TRIANGLE_STRIP );
        glNormal3f( 0.0f, 0.0f, - 1.0f );
        glVertex3f( min_coord.x(), min_coord.y(), max_coord.z() );
        glVertex3f( min_coord.x(), max_coord.y(), max_coord.z() );
        glVertex3f( max_coord.x(), min_coord.y(), max_coord.z() );
        glVertex3f( max_coord.x(), max_coord.y(), max_coord.z() );
    glEnd();
}

void SSAORenderer::initialize_shaders( void )
{
    glHint( GL_PERSPECTIVE_CORRECTION_HINT, GL_FASTEST );

    // Initialize the shader for object.
    {
        const std::string vert_code = "SSAORenderer/object_shader.vert";
        const std::string frag_code = "SSAORenderer/object_shader.frag";

        kvs::glew::ShaderSource vert( vert_code );
        kvs::glew::ShaderSource frag( frag_code );

        this->create_shaders( m_object_shader, vert, frag );
    }

    // Initialize the shader for SSAO.
    {
        const std::string vert_code = "SSAORenderer/ssao_shader.vert";
        const std::string frag_code = "SSAORenderer/ssao_shader.frag";

        kvs::glew::ShaderSource vert( vert_code );
        kvs::glew::ShaderSource frag( frag_code );

        this->create_shaders( m_ssao_shader, vert, frag );
    }
}

void SSAORenderer::initialize_framebuffer( void )
{
    m_framebuffer.create();
    m_framebuffer.bind();

    this->create_texture( m_color_texture, m_framebuffer, GL_RGBA, GL_COLOR_ATTACHMENT0_EXT );
    this->create_texture( m_position_texture, m_framebuffer, GL_RGBA32F_ARB, GL_COLOR_ATTACHMENT1_EXT );
    this->create_texture( m_normal_texture, m_framebuffer, GL_RGBA32F_ARB, GL_COLOR_ATTACHMENT2_EXT );
    this->create_texture( m_depth_texture, m_framebuffer, GL_DEPTH_COMPONENT, GL_DEPTH_ATTACHMENT_EXT );

    m_framebuffer.disable();

    GLenum error = glGetError();
    if ( error != GL_NO_ERROR )
    {
        kvsMessageError( "framebuffer allocation failed: %s.", gluErrorString(error));
        exit( EXIT_FAILURE );
    }
}

void SSAORenderer::create_texture(
    kvs::Texture2D& texture,
    kvs::glew::FrameBufferObject& framebuffer,
    GLint format,
    GLenum attachment )
{
    texture.release();

    texture.setWrapS( GL_CLAMP_TO_EDGE );
    texture.setWrapT( GL_CLAMP_TO_EDGE );
    texture.setMagFilter( GL_LINEAR );
    texture.setMinFilter( GL_LINEAR );

    const GLenum external_format = ( format == GL_DEPTH_COMPONENT ) ? GL_DEPTH_COMPONENT : GL_RGBA;

    texture.setPixelFormat( format, external_format, GL_UNSIGNED_BYTE );
    texture.create( m_width, m_height );
    {
        GLenum error = glGetError();
        if ( error != GL_NO_ERROR )
        {
            kvsMessageError( "color buffer allocation failed: %s.", gluErrorString(error));
            exit( EXIT_FAILURE );
        }
    }

    glFramebufferTexture2DEXT( GL_FRAMEBUFFER_EXT, attachment, GL_TEXTURE_2D, texture.id(), 0 );

    texture.unbind();
}

void SSAORenderer::create_shaders(
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

void SSAORenderer::setup_shader_parameter( const GLfloat* projection_matrix )
{
    m_ssao_shader.setUniformValuei( "color_texture", 0 );
    m_ssao_shader.setUniformValuei( "position_texture", 1 );
    m_ssao_shader.setUniformValuei( "normal_texture", 2 );
    m_ssao_shader.setUniformValuei( "depth_texture", 3 );
    glUniformMatrix4fv( m_ssao_shader.uniformLocation( "projection" ), 1, GL_FALSE, projection_matrix );
}

void SSAORenderer::initialize_projection( void )
{
    glMatrixMode( GL_PROJECTION );
    glMatrixMode( GL_MODELVIEW );
}

void SSAORenderer::initialize_modelview( void )
{
    glDisable( GL_LINE_SMOOTH );

    glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );

    glShadeModel( GL_SMOOTH );

    glColorMaterial( GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE );
    glEnable( GL_COLOR_MATERIAL );

    if( !this->isShading() )
    {
        glDisable( GL_NORMALIZE );
        glDisable( GL_LIGHTING );
    }
    else
    {
        glEnable( GL_NORMALIZE );
        glEnable( GL_LIGHTING );
    }

    glEnable( GL_BLEND );
    glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
}

}

}
