//
//  ShadowMapRenderer.cpp
//  
//
//  Created by Jun Nishimura on 11/9/10.
//  Copyright 2011 Jun Nishimura. All rights reserved.
//

#include "ShadowMapRenderer.h"

namespace kvs
{

namespace glew
{

ShadowMapRenderer::ShadowMapRenderer( kvs::ScreenBase* screen ) :
    m_ref_screen( screen )
{
    this->initialize();
    this->setShader( kvs::Shader::Phong() );
}

ShadowMapRenderer::~ShadowMapRenderer( void )
{
    this->clear();
}

void ShadowMapRenderer::initialize( void )
{
    m_ref_polygon = NULL;

    m_width = 0;
    m_height = 0;

    m_shadow_width = 1024;
    m_shadow_height = 1024;

    m_shader = NULL;

    m_off_coord = 0;
    m_off_color = 0;
    m_off_normal = 0;

    m_shadow_level = 0;
    m_shadow_strength = 0.5f;

    m_is_degug_draw = false;
}

void ShadowMapRenderer::clear( void )
{
    if ( m_shader )
    {
        delete m_shader;
        m_shader = NULL;
    }
}

void ShadowMapRenderer::attachPolygonObject( const kvs::PolygonObject* polygon )
{
    // Check polygon object for rendering.
    if ( polygon->polygonType() != kvs::PolygonObject::Triangle )
    {
        kvsMessageError( "Polygon type of this polygon is not triangle." );
        exit( EXIT_FAILURE );
    }

    if ( polygon->ncolors() != polygon->nvertices() && polygon->ncolors() > 1 )
    {
        kvsMessageError( "Color type of this polygon is not vertex color." );
        exit( EXIT_FAILURE );
    }

    m_ref_polygon = polygon;

    if ( m_ref_polygon->normals().pointer() == NULL ||
         m_ref_polygon->nnormals() == 0 )
    {
        BaseClass::disableShading();
    }
}

void ShadowMapRenderer::setShadowSize( const size_t width, const size_t height )
{
    m_shadow_width = width;
    m_shadow_height = height;

    if ( m_width != 0 && m_height != 0 )
    {
        this->initialize_framebuffer();
    }
}

void ShadowMapRenderer::setShadowLevel( const int level )
{
    m_shadow_level = level;
}

void ShadowMapRenderer::setShadowStrength( const float strength )
{
    m_shadow_strength = strength;
}

void ShadowMapRenderer::enableDebugDraw( void )
{
    m_is_degug_draw = true;
}

void ShadowMapRenderer::disableDebugDraw( void )
{
    m_is_degug_draw = false;
}

void ShadowMapRenderer::exec(
    kvs::ObjectBase* object, kvs::Camera* camera, kvs::Light* light )
{
    kvs::PolygonObject* polygon = kvs::PolygonObject::DownCast( object );
    if ( !m_ref_polygon ) this->attachPolygonObject( polygon );

    BaseClass::m_timer.start();
    this->create_image( polygon, camera, light );
    BaseClass::m_timer.stop();
}

void ShadowMapRenderer::create_image(
    kvs::PolygonObject* polygon, kvs::Camera* camera, kvs::Light* light )
{
    glPushAttrib( GL_CURRENT_BIT | GL_ENABLE_BIT | GL_LIGHTING_BIT );

    glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
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

    // Following processes are executed once.
    if ( m_width == 0 && m_height == 0 )
    {
        this->initialize_shaders();
        this->initialize_framebuffer();
        this->download_buffer_object();
    }

    // Following processes are executed when the window size is changed.
    if ( ( m_width  != camera->windowWidth() ) ||
         ( m_height != camera->windowHeight() ) )

    {
        m_width  = camera->windowWidth();
        m_height = camera->windowHeight();
    }

    m_framebuffer.bind();
    glViewport( 0, 0, m_shadow_width, m_shadow_height );

    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    glColorMask( GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE );
    glEnable( GL_DEPTH_TEST );
    GLfloat texture_matrix[16];
    this->draw_shadow( texture_matrix );

    m_framebuffer.disable();
    glViewport( 0, 0, m_width, m_height );

    glColorMask( GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE );

    glEnable( GL_TEXTURE_2D );
    glMatrixMode( GL_TEXTURE ); glLoadIdentity();
    glTranslatef( 0.5, 0.5, 0.5 );
    glScalef( 0.5, 0.5, 0.5 );
    glMultMatrixf( texture_matrix );
    glMatrixMode( GL_MODELVIEW );

    m_shadow_map_shader.bind();
    m_shadow_texture.bind();
    this->setup_shader_parameter();
    this->draw_buffer_object();
    m_shadow_texture.unbind();
    m_shadow_map_shader.unbind();

    glDisable( GL_TEXTURE_2D );
    glDisable( GL_DEPTH_TEST );

    if ( m_is_degug_draw )
    {
        glMatrixMode( GL_TEXTURE ); glLoadIdentity();
        glMatrixMode( GL_MODELVIEW );  glPushMatrix(); glLoadIdentity();
        glMatrixMode( GL_PROJECTION ); glPushMatrix(); glLoadIdentity();
        const float aspect = (float)m_width / (float)m_height;
        glOrtho( 0.0, aspect, 0.0, 1.0, -1.0, 1.0 );
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

void ShadowMapRenderer::download_buffer_object( void )
{
    m_vbo.release();
    m_ibo.release();

    const size_t nvertices = m_ref_polygon->nvertices();
    const size_t npolygons = m_ref_polygon->nconnections();
    const size_t nnormals = m_ref_polygon->nnormals();
    const size_t ncolors = m_ref_polygon->ncolors();
    const bool has_colors = ncolors > 1;
    const bool has_normals = BaseClass::isShading();
    const bool has_connect = npolygons > 0;

    const size_t size_v = sizeof(CoordType) * 3 * nvertices;
    const size_t size_c = has_colors ? sizeof(ColorType) * 3 * nvertices : 0;
    const size_t size_n = has_normals ? sizeof(NormalType) * 3 * nvertices : 0;
    const size_t size_of_vertex = size_v + size_c + size_n;

    const size_t off_v = 0;
    const size_t off_c = off_v + size_v;
    const size_t off_n = off_c + size_c;

    m_vbo.create( size_of_vertex );
    m_vbo.bind();

    const CoordType* ptr_v = m_ref_polygon->coords().pointer();
    m_vbo.download( size_v, ptr_v, off_v );

    if ( has_colors )
    {
        const ColorType* ptr_c = m_ref_polygon->colors().pointer();
        m_vbo.download( size_c, ptr_c, off_c );
    }

    if ( has_normals )
    {
        const float* p_normals = m_ref_polygon->normals().pointer();
        kvs::ValueArray<NormalType> normals( 3 * nvertices );
        if ( nnormals == nvertices )
        {
            for ( size_t i = 0; i < nvertices; i++ )
            {
                const kvs::Vector3f v( p_normals + i * 3 );
                const kvs::Vector3f n = v.normalize() * 127.0f;
                normals[ i * 3     ] = static_cast<NormalType>(n[0]);
                normals[ i * 3 + 1 ] = static_cast<NormalType>(n[1]);
                normals[ i * 3 + 2 ] = static_cast<NormalType>(n[2]);
            }
        }
        else
        {
            for ( size_t i = 0; i < nvertices / 3; i++ )
            {
                const kvs::Vector3f v( p_normals + i * 3 );
                const kvs::Vector3f n = v.normalize() * 127.0f;
                for ( size_t j = 0; j < 3; j++ )
                {
                    normals[ i * 9 + j * 3     ] = static_cast<NormalType>(n[0]);
                    normals[ i * 9 + j * 3 + 1 ] = static_cast<NormalType>(n[1]);
                    normals[ i * 9 + j * 3 + 2 ] = static_cast<NormalType>(n[2]);
                }
            }
        }
        const NormalType* ptr_n = normals.pointer();
        m_vbo.download( size_n, ptr_n, off_n );
    }

    if ( has_connect )
    {
        const size_t size_of_polygon = sizeof(ConnectType) * 3 * npolygons;
        const ConnectType* ptr_conn = m_ref_polygon->connections().pointer();

        m_ibo.create( size_of_polygon );
        m_ibo.bind();
        m_ibo.download( size_of_polygon, ptr_conn, 0 );
    }

    GLenum error = glGetError();
    if ( error != GL_NO_ERROR )
    {
        kvsMessageError( "Vertex Buffer Object download failed: %s(%d).", gluErrorString( error ), error );
    }

    m_off_coord  = off_v;
    m_off_color  = off_c;
    m_off_normal = off_n;
}

void ShadowMapRenderer::draw_buffer_object( void )
{
    const size_t nvertices = m_ref_polygon->nvertices();
    const size_t npolygons = m_ref_polygon->nconnections();
    const size_t ncolors = m_ref_polygon->ncolors();
    const bool has_colors = ncolors > 1;
    const bool has_normals = BaseClass::isShading();
    const bool has_connect = npolygons > 0;

    m_vbo.bind();

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
        glColor3ub( color.r(), color.g(), color.b() );
    }

    if ( has_normals )
    {
        glEnableClientState( GL_NORMAL_ARRAY );
        glNormalPointer( GL_BYTE, 0, (char*)(m_off_normal) );
    }

    if ( has_connect )
    {
        m_ibo.bind();
        glDrawElements( GL_TRIANGLES, 3 * npolygons, GL_UNSIGNED_INT, 0 );
        m_ibo.unbind();
    }
    else
    {
        glDrawArrays( GL_TRIANGLES, 0, nvertices );
    }

    if ( has_normals ) glDisableClientState( GL_NORMAL_ARRAY );
    if ( has_colors )  glDisableClientState( GL_COLOR_ARRAY );
    glDisableClientState( GL_VERTEX_ARRAY );

    m_vbo.unbind();
}

void ShadowMapRenderer::draw_shadow( GLfloat* modelview_matrix )
{
    const kvs::Camera* camera = m_ref_screen->camera();
    const kvs::Vector3f l = m_ref_screen->light()->position();
    const kvs::Vector3f n = m_ref_screen->objectManager()->normalize();
    const kvs::Vector3f c = m_ref_screen->objectManager()->objectCenter();
    const float aspect = (float)m_shadow_width / (float)m_shadow_height;

    glMatrixMode( GL_PROJECTION ); glPushMatrix(); glLoadIdentity();
    glMatrixMode( GL_MODELVIEW );  glPushMatrix(); glLoadIdentity();

    gluPerspective(
        camera->fieldOfView(), aspect, camera->front(), camera->back() );
    gluLookAt(
        l.x(), l.y(), l.z(),
          0.0,   0.0,   0.0,
          0.0,   1.0,   0.0 );

    glScalef( n.x(), n.y(), n.z() );
    glTranslatef( - c.x(), - c.y(), - c.z() );
    glGetFloatv( GL_MODELVIEW_MATRIX, modelview_matrix );

    this->draw_buffer_object_shadow();

    glMatrixMode( GL_PROJECTION ); glPopMatrix();
    glMatrixMode( GL_MODELVIEW );  glPopMatrix();
}

void ShadowMapRenderer::draw_buffer_object_shadow( void )
{
    const size_t nvertices = m_ref_polygon->nvertices();
    const size_t npolygons = m_ref_polygon->nconnections();
    const bool has_connect = npolygons > 0;

    m_vbo.bind();

    glEnableClientState( GL_VERTEX_ARRAY );
    glVertexPointer( 3, GL_FLOAT, 0, (char*)(m_off_coord) );

    if ( has_connect )
    {
        m_ibo.bind();
        glDrawElements( GL_TRIANGLES, 3 * npolygons, GL_UNSIGNED_INT, 0 );
        m_ibo.unbind();
    }
    else
    {
        glDrawArrays( GL_TRIANGLES, 0, nvertices );
    }

    glDisableClientState( GL_VERTEX_ARRAY );

    m_vbo.unbind();
}

void ShadowMapRenderer::initialize_shaders( void )
{
    glHint( GL_PERSPECTIVE_CORRECTION_HINT, GL_FASTEST );
    // Initialize the shader for shadow map.
    {
        const std::string vert_code = "ShadowMapShader/shadow_map.vert";
        const std::string frag_code = "ShadowMapShader/shadow_map.frag";

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

        this->create_shaders( m_shadow_map_shader, vert, frag );

        if ( BaseClass::isShading() )
        {
            m_shadow_map_shader.bind();
            switch ( m_shader->type() )
            {
            case kvs::Shader::LambertShading:
            {
                const GLfloat Ka = ((kvs::Shader::Lambert*)(m_shader))->Ka;
                const GLfloat Kd = ((kvs::Shader::Lambert*)(m_shader))->Kd;
                m_shadow_map_shader.setUniformValuef( "shading.Ka", Ka );
                m_shadow_map_shader.setUniformValuef( "shading.Kd", Kd );
                break;
            }
            case kvs::Shader::PhongShading:
            {
                const GLfloat Ka = ((kvs::Shader::Phong*)(m_shader))->Ka;
                const GLfloat Kd = ((kvs::Shader::Phong*)(m_shader))->Kd;
                const GLfloat Ks = ((kvs::Shader::Phong*)(m_shader))->Ks;
                const GLfloat S  = ((kvs::Shader::Phong*)(m_shader))->S;
                m_shadow_map_shader.setUniformValuef( "shading.Ka", Ka );
                m_shadow_map_shader.setUniformValuef( "shading.Kd", Kd );
                m_shadow_map_shader.setUniformValuef( "shading.Ks", Ks );
                m_shadow_map_shader.setUniformValuef( "shading.S",  S );
                break;
            }
            case kvs::Shader::BlinnPhongShading:
            {
                const GLfloat Ka = ((kvs::Shader::BlinnPhong*)(m_shader))->Ka;
                const GLfloat Kd = ((kvs::Shader::BlinnPhong*)(m_shader))->Kd;
                const GLfloat Ks = ((kvs::Shader::BlinnPhong*)(m_shader))->Ks;
                const GLfloat S  = ((kvs::Shader::BlinnPhong*)(m_shader))->S;
                m_shadow_map_shader.setUniformValuef( "shading.Ka", Ka );
                m_shadow_map_shader.setUniformValuef( "shading.Kd", Kd );
                m_shadow_map_shader.setUniformValuef( "shading.Ks", Ks );
                m_shadow_map_shader.setUniformValuef( "shading.S",  S );
                break;
            }
            default: /* NO SHADING */ break;
            }
            m_shadow_map_shader.unbind();
        }
    }
}

void ShadowMapRenderer::initialize_framebuffer( void )
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
    m_shadow_texture.create( m_shadow_width, m_shadow_height );
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

void ShadowMapRenderer::create_shaders(
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

void ShadowMapRenderer::setup_shader_parameter( void )
{
    m_shadow_map_shader.setUniformValuei( "shadow", 0 );
    m_shadow_map_shader.setUniformValuei( "level", m_shadow_level );
    m_shadow_map_shader.setUniformValuef( "strength", m_shadow_strength );
    m_shadow_map_shader.setUniformValuef( "texture_size_inv", 1.0f / m_shadow_width, 1.0f / m_shadow_height );
}

}

}
