//
//  StochasticVolume2Renderer.cpp
//  
//
//  Created by Jun Nishimura on 11/4/10.
//  Copyright 2011 Jun Nishimura. All rights reserved.
//

#include "StochasticVolume2Renderer.h"

#include "SPTDecompositionTable.h"
#include <kvs/TetrahedralCell>
#include <kvs/Math>

namespace kvs
{

namespace glew
{

StochasticVolume2Renderer::Volume::Volume( void ) :
    m_nsteps( 1 ),
    m_nvertices( 0 ),
    m_ncells( 0 ),
    m_indices( NULL ),
    m_coords( NULL ),
    m_values( NULL ),
    m_normals( NULL ),
    m_connections( NULL )
{
}

StochasticVolume2Renderer::Volume::~Volume( void )
{
    this->release();
}

void StochasticVolume2Renderer::Volume::release( void )
{
    if ( m_indices ) { delete[] m_indices; m_indices = NULL; }
    if ( m_coords )  { delete[] m_coords;  m_coords  = NULL; }
    if ( m_values )  { delete[] m_values;  m_values  = NULL; }
    if ( m_normals ) { delete[] m_normals; m_normals  = NULL; }
    if ( m_connections ) { delete[] m_connections; m_connections = NULL; }
}

void StochasticVolume2Renderer::Volume::create(
    const size_t nsteps,
    const size_t nvertices,
    const size_t ncells )
{
    this->release();

    m_nsteps = nsteps;
    m_nvertices = nvertices;
    m_coords = new StochasticVolume2Renderer::CoordType [ nvertices * 3 ];
    m_values = new StochasticVolume2Renderer::ValueType [ nvertices * 2 * m_nsteps ];
    m_normals = new StochasticVolume2Renderer::NormalType [ nvertices * 3 ];
    m_indices = new StochasticVolume2Renderer::IndexType [ nvertices * 2 ];

    m_ncells = ncells;
    m_connections = new StochasticVolume2Renderer::ConnectType [ ncells * 4 ];
}

const size_t StochasticVolume2Renderer::Volume::nvertices( void ) const
{
    return( m_nvertices );
}

const size_t StochasticVolume2Renderer::Volume::ncells( void ) const
{
    return( m_ncells );
}

const size_t StochasticVolume2Renderer::Volume::byteSizePerVertex( void ) const
{
    const size_t index_size  = sizeof( StochasticVolume2Renderer::IndexType ) * 2;
    const size_t coord_size  = sizeof( StochasticVolume2Renderer::CoordType ) * 3;
    const size_t value_size  = sizeof( StochasticVolume2Renderer::ValueType ) * 2 * m_nsteps;
    const size_t normal_size = sizeof( StochasticVolume2Renderer::NormalType ) * 3;

    return( index_size + coord_size + value_size + normal_size );
}

const size_t StochasticVolume2Renderer::Volume::byteSizeOfVertex( void ) const
{
    return( this->byteSizePerVertex() * m_nvertices );
}

const size_t StochasticVolume2Renderer::Volume::byteSizePerCell( void ) const
{
    return( sizeof( StochasticVolume2Renderer::ConnectType ) * 4 );
}

const size_t StochasticVolume2Renderer::Volume::byteSizeOfCell( void ) const
{
    return( this->byteSizePerCell() * m_ncells );
}

const StochasticVolume2Renderer::IndexType* StochasticVolume2Renderer::Volume::indices( void ) const
{
    return( m_indices );
}

StochasticVolume2Renderer::IndexType* StochasticVolume2Renderer::Volume::indices( void )
{
    return( m_indices );
}

const StochasticVolume2Renderer::CoordType* StochasticVolume2Renderer::Volume::coords( void ) const
{
    return( m_coords );
}

StochasticVolume2Renderer::CoordType* StochasticVolume2Renderer::Volume::coords( void )
{
    return( m_coords );
}

const StochasticVolume2Renderer::ValueType* StochasticVolume2Renderer::Volume::values( void ) const
{
    return( m_values );
}

StochasticVolume2Renderer::ValueType* StochasticVolume2Renderer::Volume::values( void )
{
    return( m_values );
}

const StochasticVolume2Renderer::NormalType* StochasticVolume2Renderer::Volume::normals( void ) const
{
    return( m_normals );
}

StochasticVolume2Renderer::NormalType* StochasticVolume2Renderer::Volume::normals( void )
{
    return( m_normals );
}

const StochasticVolume2Renderer::ConnectType* StochasticVolume2Renderer::Volume::connections( void ) const
{
    return( m_connections );
}

StochasticVolume2Renderer::ConnectType* StochasticVolume2Renderer::Volume::connections( void )
{
    return( m_connections );
}


StochasticVolume2Renderer::Renderer::Renderer( void ):
    m_volume( NULL ),
    m_nsteps(1),
    m_nvertices(0),
    m_ncells(0),
    m_off_index(0),
    m_off_coord(0),
    m_off_value(0),
    m_off_normal(0),
    m_loc_identifier(0),
    m_loc_values(0)
{
}

void StochasticVolume2Renderer::Renderer::set(
    const StochasticVolume2Renderer::Volume* volume,
    const size_t nsteps,
    const size_t nvertices,
    const size_t ncells,
    const size_t loc_identifier,
    const size_t loc_values )
{
    m_volume = volume;
    m_nsteps = nsteps;
    m_nvertices = nvertices;
    m_ncells = ncells;
    m_loc_identifier = loc_identifier;
    m_loc_values = loc_values;
}

const bool StochasticVolume2Renderer::Renderer::download(
    kvs::glew::VertexBufferObject& vbo,
    kvs::glew::IndexBufferObject& ibo )
{
    if ( m_volume == NULL ) return( false );

    const size_t size = m_nvertices;  // number of vertices
    const size_t size_i = sizeof(StochasticVolume2Renderer::IndexType) * 2 * size;
    const size_t size_c = sizeof(StochasticVolume2Renderer::CoordType) * 3 * size;
    const size_t size_v = sizeof(StochasticVolume2Renderer::ValueType) * 2 * m_nsteps * size;
    const size_t size_n = sizeof(StochasticVolume2Renderer::NormalType) * 3 * size;
    const size_t off_i = 0;
    const size_t off_c = off_i + size_i;
    const size_t off_v = off_c + size_c;
    const size_t off_n = off_v + size_v;

    const StochasticVolume2Renderer::IndexType* ptr_i = m_volume->indices();
    const StochasticVolume2Renderer::CoordType* ptr_c = m_volume->coords();
    const StochasticVolume2Renderer::ValueType* ptr_v = m_volume->values();
    const StochasticVolume2Renderer::NormalType* ptr_n = m_volume->normals();

    vbo.download( size_i, ptr_i, off_i );
    vbo.download( size_c, ptr_c, off_c );
    vbo.download( size_v, ptr_v, off_v );
    vbo.download( size_n, ptr_n, off_n );

    const StochasticVolume2Renderer::ConnectType* ptr_conn = m_volume->connections();
    ibo.download( m_volume->byteSizeOfCell(), ptr_conn, 0 );

    GLenum error = glGetError();
    if ( error != GL_NO_ERROR )
    {
        kvsMessageError( "Vertex Buffer Object download failed: %s(%d).", gluErrorString( error ), error );
    }

    m_off_index  = off_i;
    m_off_coord  = off_c;
    m_off_value  = off_v;
    m_off_normal = off_n;

    return( true );
}

void StochasticVolume2Renderer::Renderer::draw( const size_t step ) const
{
    glEnableClientState( GL_VERTEX_ARRAY );
    glEnableClientState( GL_NORMAL_ARRAY );
    glVertexPointer( 3, GL_FLOAT, 0, (char*)(m_off_coord) );
    glNormalPointer( GL_BYTE, 0, (char*)(m_off_normal) );

    glEnableVertexAttribArray( m_loc_values );
    glVertexAttribPointer( m_loc_values, 2, GL_FLOAT, GL_FALSE, 0, (char*)( m_off_value + 2 * step * sizeof(StochasticVolume2Renderer::ValueType) * m_nvertices ) );

    glEnableVertexAttribArray( m_loc_identifier );
    glVertexAttribPointer( m_loc_identifier, 2, GL_UNSIGNED_SHORT, GL_FALSE, 0, (char*)(m_off_index) );

    glColor4f( 1.0f, 1.0f, 1.0f, 1.0f );
    glDrawElements( GL_LINES_ADJACENCY_EXT, 4 * m_ncells, GL_UNSIGNED_INT, 0 );

    glDisableClientState( GL_NORMAL_ARRAY );
    glDisableClientState( GL_VERTEX_ARRAY );
    glDisableVertexAttribArray( m_loc_values );
    glDisableVertexAttribArray( m_loc_identifier );
}


StochasticVolume2Renderer::StochasticVolume2Renderer( void )
{
    this->initialize();
    BaseClass::setShader( kvs::Shader::Lambert() );
}

StochasticVolume2Renderer::StochasticVolume2Renderer(
    kvs::UnstructuredVolumeObject* volume,
    const size_t nsteps )
{
    this->initialize();
    this->attachVolumeObject( volume );
    this->setNSteps( nsteps );
    BaseClass::setShader( kvs::Shader::Lambert() );
}

StochasticVolume2Renderer::~StochasticVolume2Renderer( void )
{
}

void StochasticVolume2Renderer::exec(
    kvs::ObjectBase* object,
    kvs::Camera*     camera,
    kvs::Light*      light )
{
    // This function is not called.
}


void StochasticVolume2Renderer::initialize( void )
{
    BaseClass::initialize();

    m_ref_volume = NULL;

    m_volume = NULL;
    m_renderer = NULL;

    m_table[0].setTableSize( 128, 128 );
    m_table[1].setTableSize( 128, 128 );
    m_edge_size = 1.0f;

    m_nsteps = 1;
    m_step = 0;

    m_show_flag0 = true;
    m_show_flag1 = true;
}

void StochasticVolume2Renderer::attachVolumeObject( kvs::UnstructuredVolumeObject* volume )
{
    m_ref_volume = volume;

    if ( volume->cellType() != kvs::UnstructuredVolumeObject::Tetrahedra )
    {
        kvsMessageError( "Cell type of this volume is not tetrahedral cell." );
        exit(1);
    }

    if ( volume->veclen() != 2 )
    {
        kvsMessageError( "Veclen of this volume is not 2." );
        exit(1);
    }
}

void StochasticVolume2Renderer::setNSteps( const size_t nsteps )
{
    m_nsteps = nsteps;
}

void StochasticVolume2Renderer::setStep( const size_t step )
{
    if ( step < m_nsteps ) m_step = step;
}

void StochasticVolume2Renderer::setEdgeSize( const float edge_size )
{
    m_edge_size = edge_size;
}

void StochasticVolume2Renderer::setTransferFunction( const kvs::TransferFunction& tfunc, const size_t index )
{
    m_tfunc[index] = tfunc;

    if ( m_table[index].isDownloaded() )
    {
        m_table[index].release();
    }
}

void StochasticVolume2Renderer::show( const size_t index )
{
    if ( index == 0 ) m_show_flag0 = true;
    else              m_show_flag1 = true;
}

void StochasticVolume2Renderer::hide( const size_t index )
{
    if ( index == 0 ) m_show_flag0 = false;
    else              m_show_flag1 = false;
}

const kvs::TransferFunction& StochasticVolume2Renderer::transferFunction( const size_t index ) const
{
    return( m_tfunc[index] );
}

kvs::TransferFunction& StochasticVolume2Renderer::transferFunction( const size_t index )
{
    return( m_tfunc[index] );
}

const bool StochasticVolume2Renderer::isShown( const size_t index )
{
    if ( index == 0 ) return( m_show_flag0 );
    else              return( m_show_flag1 );
}

kvs::ObjectBase* StochasticVolume2Renderer::object( void ) const
{
    return( m_ref_volume );
}

void StochasticVolume2Renderer::attachObject( kvs::ObjectBase* object )
{
    this->attachVolumeObject( static_cast<kvs::UnstructuredVolumeObject*>( object ) );
}

const StochasticRendererBase::RendererType StochasticVolume2Renderer::rendererType( void ) const
{
    return( BaseClass::Volume );
}

void StochasticVolume2Renderer::initializeShader( void )
{
    const std::string vert_code = "StochasticShader/spt2.vert";
    const std::string geom_code = "StochasticShader/spt2.geom";
    const std::string frag_code = "StochasticShader/spt2.frag";

    kvs::glew::ShaderSource vert( vert_code );
    kvs::glew::ShaderSource geom( geom_code );
    kvs::glew::ShaderSource frag( frag_code );

    if ( BaseClass::isEnabledShading() )
    {
        switch ( BaseClass::m_shader->type() )
        {
        case kvs::Shader::LambertShading: frag.define("ENABLE_LAMBERT_SHADING"); break;
        case kvs::Shader::PhongShading: frag.define("ENABLE_PHONG_SHADING"); break;
        case kvs::Shader::BlinnPhongShading: frag.define("ENABLE_BLINN_PHONG_SHADING"); break;
        default: /* NO SHADING */ break;
        }
    }

    this->create_shaders( m_shader_program, vert, geom, frag );
    m_loc_identifier = m_shader_program.attributeLocation( "identifier" );
    m_loc_values = m_shader_program.attributeLocation( "value" );

    if ( BaseClass::isEnabledShading() )
    {
        m_shader_program.bind();
        switch ( BaseClass::m_shader->type() )
        {
        case kvs::Shader::LambertShading:
        {
            const GLfloat Ka = ((kvs::Shader::Lambert*)(BaseClass::m_shader))->Ka;
            const GLfloat Kd = ((kvs::Shader::Lambert*)(BaseClass::m_shader))->Kd;
            m_shader_program.setUniformValuef( "shading.Ka", Ka );
            m_shader_program.setUniformValuef( "shading.Kd", Kd );
            break;
        }
        case kvs::Shader::PhongShading:
        {
            const GLfloat Ka = ((kvs::Shader::Phong*)(BaseClass::m_shader))->Ka;
            const GLfloat Kd = ((kvs::Shader::Phong*)(BaseClass::m_shader))->Kd;
            const GLfloat Ks = ((kvs::Shader::Phong*)(BaseClass::m_shader))->Ks;
            const GLfloat S  = ((kvs::Shader::Phong*)(BaseClass::m_shader))->S;
            m_shader_program.setUniformValuef( "shading.Ka", Ka );
            m_shader_program.setUniformValuef( "shading.Kd", Kd );
            m_shader_program.setUniformValuef( "shading.Ks", Ks );
            m_shader_program.setUniformValuef( "shading.S",  S );
            break;
        }
        case kvs::Shader::BlinnPhongShading:
        {
            const GLfloat Ka = ((kvs::Shader::BlinnPhong*)(BaseClass::m_shader))->Ka;
            const GLfloat Kd = ((kvs::Shader::BlinnPhong*)(BaseClass::m_shader))->Kd;
            const GLfloat Ks = ((kvs::Shader::BlinnPhong*)(BaseClass::m_shader))->Ks;
            const GLfloat S  = ((kvs::Shader::BlinnPhong*)(BaseClass::m_shader))->S;
            m_shader_program.setUniformValuef( "shading.Ka", Ka );
            m_shader_program.setUniformValuef( "shading.Kd", Kd );
            m_shader_program.setUniformValuef( "shading.Ks", Ks );
            m_shader_program.setUniformValuef( "shading.S",  S );
            break;
        }
        default: /* NO SHADING */ break;
        }
        m_shader_program.unbind();
    }

    // Inititlize the textures.
    this->initialize_decomposition_texture();
    this->create_preintegration_table();
}

void StochasticVolume2Renderer::createVertexBuffer( void )
{
    // Extract surfaces.
    const std::type_info& type = m_ref_volume->values().typeInfo()->type();
    if (      type == typeid( kvs::Int8   ) ) this->create_vertexbuffer_from_volume<kvs::Int8>();
    else if ( type == typeid( kvs::Int16  ) ) this->create_vertexbuffer_from_volume<kvs::Int16>();
    else if ( type == typeid( kvs::Int32  ) ) this->create_vertexbuffer_from_volume<kvs::Int32>();
    else if ( type == typeid( kvs::Int64  ) ) this->create_vertexbuffer_from_volume<kvs::Int64>();
    else if ( type == typeid( kvs::UInt8  ) ) this->create_vertexbuffer_from_volume<kvs::UInt8>();
    else if ( type == typeid( kvs::UInt16 ) ) this->create_vertexbuffer_from_volume<kvs::UInt16>();
    else if ( type == typeid( kvs::UInt32 ) ) this->create_vertexbuffer_from_volume<kvs::UInt32>();
    else if ( type == typeid( kvs::UInt64 ) ) this->create_vertexbuffer_from_volume<kvs::UInt64>();
    else if ( type == typeid( kvs::Real32 ) ) this->create_vertexbuffer_from_volume<kvs::Real32>();
    else if ( type == typeid( kvs::Real64 ) ) this->create_vertexbuffer_from_volume<kvs::Real64>();
    else
    {
        kvsMessageError( "Unsupported data type '%s'.", m_ref_volume->values().typeInfo()->typeName() );
    }

    if ( m_renderer ) delete m_renderer;
    m_renderer = new Renderer();

    m_vbo.release();
    m_ibo.release();

    m_vbo.create( m_volume->byteSizeOfVertex() );
    m_ibo.create( m_volume->byteSizeOfCell() );
    m_renderer->set( m_volume, m_nsteps, m_volume->nvertices(), m_volume->ncells(), m_loc_identifier, m_loc_values );
    m_renderer->download( m_vbo, m_ibo );
}

void StochasticVolume2Renderer::downloadVertexBuffer( void )
{
    m_vbo.bind();
    m_ibo.bind();
    m_renderer->download( m_vbo, m_ibo );
}

void StochasticVolume2Renderer::drawVertexBuffer( const float modelview_matrix[16] )
{
    if ( !m_table[0].isTexture() || !m_table[1].isTexture() )
    {
        this->create_preintegration_table();
    }

    if ( BaseClass::isEnabledShading() ) glEnable( GL_LIGHTING );
    else                                 glDisable( GL_LIGHTING );

    m_vbo.bind();
    m_ibo.bind();

    glActiveTexture(GL_TEXTURE0);     m_table[0].bind();                glEnable(GL_TEXTURE_3D);
    glActiveTexture(GL_TEXTURE1);     m_table[1].bind();                glEnable(GL_TEXTURE_3D);
    glActiveTexture(GL_TEXTURE2);     m_random_texture.bind();          glEnable(GL_TEXTURE_2D);
    glActiveTexture(GL_TEXTURE3);     m_decomposition_texture.bind();   glEnable(GL_TEXTURE_2D);
    glActiveTexture(GL_TEXTURE0);

    m_shader_program.bind();

    this->setup_shader( modelview_matrix );
    m_renderer->draw( m_step );

    m_shader_program.unbind();

    glActiveTexture(GL_TEXTURE3);    m_decomposition_texture.unbind();  glDisable(GL_TEXTURE_2D);
    glActiveTexture(GL_TEXTURE2);    m_random_texture.unbind();         glDisable(GL_TEXTURE_2D);
    glActiveTexture(GL_TEXTURE1);    m_table[0].unbind();               glDisable(GL_TEXTURE_3D);
    glActiveTexture(GL_TEXTURE0);    m_table[1].unbind();               glDisable(GL_TEXTURE_3D);

    m_repeat_count++;
}

void StochasticVolume2Renderer::clearEnsembleBuffer( void )
{
    m_repeat_count = 0;
}

void StochasticVolume2Renderer::create_shaders(
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

    glProgramParameteriEXT( program_object.id(), GL_GEOMETRY_INPUT_TYPE_EXT, GL_LINES_ADJACENCY_EXT );
    glProgramParameteriEXT( program_object.id(), GL_GEOMETRY_OUTPUT_TYPE_EXT, GL_TRIANGLE_STRIP );
    GLint vertices_out = 4 * 3;
    GLint vertices_out_max = 0;
    glGetIntegerv( GL_MAX_GEOMETRY_OUTPUT_VERTICES_EXT, &vertices_out_max );
    if ( vertices_out > vertices_out_max )
    {
        kvsMessageError( "Geometry shader GL_GEOMETRY_VERTICES_OUT require = %d > max = %d", vertices_out, vertices_out_max );
    }
    glProgramParameteriEXT( program_object.id(), GL_GEOMETRY_VERTICES_OUT_EXT, vertices_out );

    if ( !program_object.link() )
    {
        GLenum error = glGetError();
        kvsMessageError( "ShaderProgram link failed: %s(%d)¥n", gluErrorString(error), error );
        std::cout << "error log:" << std::endl;
        std::cout << program_object.log() << std::endl;
        exit(1);
    }
}

void StochasticVolume2Renderer::initialize_decomposition_texture( void )
{
    m_decomposition_texture.release();
    m_decomposition_texture.setWrapS( GL_CLAMP_TO_EDGE );
    m_decomposition_texture.setWrapT( GL_CLAMP_TO_EDGE );
    m_decomposition_texture.setMagFilter( GL_NEAREST );
    m_decomposition_texture.setMinFilter( GL_NEAREST );
    m_decomposition_texture.setPixelFormat( GL_RGBA8, GL_RGBA, GL_UNSIGNED_BYTE );

    m_decomposition_texture.create( 81, 1 );

    GLubyte table[ 81 * 4 ];
    for ( size_t i = 0; i < 81; i++ )
    {
        table[ i * 4     ] = kvs::SPTDecompositionTable::DecompositionInfo[i][1] * 32;
        table[ i * 4 + 1 ] = kvs::SPTDecompositionTable::DecompositionInfo[i][2] * 32;
        table[ i * 4 + 2 ] = kvs::SPTDecompositionTable::DecompositionInfo[i][3] * 32;
        table[ i * 4 + 3 ] = kvs::SPTDecompositionTable::DecompositionInfo[i][0] * 32;
    }
    m_decomposition_texture.bind();
    m_decomposition_texture.download( 81, 1, table );
    m_decomposition_texture.unbind();
}

void StochasticVolume2Renderer::create_preintegration_table( void )
{
    for ( size_t i = 0; i < 2; i++ )
    {
        if ( !m_table[i].isDownloaded() )
        {
            m_table[i].setTransferFunction( m_tfunc[i], 0.0f, 1.0f );
            m_table[i].create( m_edge_size );
            m_table[i].download();
        }
    }
}

template <typename T>
void StochasticVolume2Renderer::create_vertexbuffer_from_volume( void )
{
    if ( m_volume ) delete m_volume;
    m_volume = new Volume();

    const size_t nvertices = m_ref_volume->nnodes();
    const size_t ncells = m_ref_volume->ncells();

    m_volume->create( m_nsteps, nvertices, ncells );

    const kvs::Real32* src_coord   = m_ref_volume->coords().pointer();
    const kvs::UInt32* src_connect = m_ref_volume->connections().pointer();
    const T* src_value = static_cast<const T*>( m_ref_volume->values().pointer() );

    double min_value[2];
    double max_value[2];
    double normalize_factor[2];
    for ( size_t i = 0; i < nvertices; i++ )
    {
        for ( size_t j = 0; j < 2; j++ )
        {
            if ( i == 0 )
            {
                min_value[j] = static_cast<double>( src_value[j] );
                max_value[j] = static_cast<double>( src_value[j] );
            }
            else
            {
                if ( min_value[j] > src_value[ i * 2 + j ] ) min_value[j] = src_value[ i * 2 + j ];
                if ( max_value[j] < src_value[ i * 2 + j ] ) max_value[j] = src_value[ i * 2 + j ];
            }
        }
    }

    for ( size_t j = 0; j < 2; j++ )
    {
        if ( max_value[j] == min_value[j] ) normalize_factor[j] = 1.0;
        else normalize_factor[j] = 1.0 / ( max_value[j] - min_value[j] );
    }

    IndexType*   dst_indices     = m_volume->indices();
    CoordType*   dst_coords      = m_volume->coords();
    ValueType*   dst_values      = m_volume->values();
    NormalType*  dst_normals     = m_volume->normals();
    ConnectType* dst_connections = m_volume->connections();

    const size_t random_texture_size = m_random_texture.width();

    for ( size_t i = 0; i < nvertices; i++ )
    {
        *(dst_coords)++ = src_coord[ i * 3     ];
        *(dst_coords)++ = src_coord[ i * 3 + 1 ];
        *(dst_coords)++ = src_coord[ i * 3 + 2 ];

        unsigned int index = i * 12347;
        *(dst_indices)++ = static_cast<IndexType>( index % random_texture_size );
        *(dst_indices)++ = static_cast<IndexType>( ( index / random_texture_size ) % random_texture_size );
    }

    for ( size_t step = 0; step < m_nsteps; step++ )
    {
        for ( size_t i = 0; i < nvertices; i++ )
        {
            for ( size_t j = 0; j < 2; j++ )
            {
                ValueType value = static_cast<ValueType>( *src_value++ );
                *(dst_values)++ = static_cast<ValueType>( ( value - min_value[j] ) * normalize_factor[j] );
            }
        }
    }

    for ( size_t i = 0; i < ncells; i++ )
    {
        *(dst_connections)++ = src_connect[ i * 4     ];
        *(dst_connections)++ = src_connect[ i * 4 + 1 ];
        *(dst_connections)++ = src_connect[ i * 4 + 2 ];
        *(dst_connections)++ = src_connect[ i * 4 + 3 ];
    }

/*
    kvs::TetrahedralCell<T> cell( m_ref_volume );
    kvs::ValueArray<int> counter( nvertices );
    kvs::ValueArray<float> normals( 3 * nvertices );
    counter.fill( 0x00 );
    normals.fill( 0x00 );
    for ( size_t i = 0; i < ncells; i++ )
    {
        cell.bindCell( i );

        const kvs::Vector3f g = - cell.gradient();
        const kvs::UInt32 id0 = m_ref_volume->connections().at( 4 * i );
        const kvs::UInt32 id1 = m_ref_volume->connections().at( 4 * i + 1 );
        const kvs::UInt32 id2 = m_ref_volume->connections().at( 4 * i + 2 );
        const kvs::UInt32 id3 = m_ref_volume->connections().at( 4 * i + 3 );

        counter[id0]++;
        counter[id1]++;
        counter[id2]++;
        counter[id3]++;

        normals[ 3 * id0 + 0 ] += g.x();
        normals[ 3 * id0 + 1 ] += g.y();
        normals[ 3 * id0 + 2 ] += g.z();

        normals[ 3 * id1 + 0 ] += g.x();
        normals[ 3 * id1 + 1 ] += g.y();
        normals[ 3 * id1 + 2 ] += g.z();

        normals[ 3 * id2 + 0 ] += g.x();
        normals[ 3 * id2 + 1 ] += g.y();
        normals[ 3 * id2 + 2 ] += g.z();

        normals[ 3 * id3 + 0 ] += g.x();
        normals[ 3 * id3 + 1 ] += g.y();
        normals[ 3 * id3 + 2 ] += g.z();
    }
*/

    for ( size_t i = 0; i < nvertices; i++ )
    {
/*
        const kvs::Vector3f v( normals.pointer() + i * 3 );
        const kvs::Vector3f n( ( v / (float)counter[i] ).normalize() * 127.0f );
        *(dst_normals)++ = static_cast<NormalType>(n[0]);
        *(dst_normals)++ = static_cast<NormalType>(n[1]);
        *(dst_normals)++ = static_cast<NormalType>(n[2]);
*/
        *(dst_normals)++ = static_cast<NormalType>(0);
        *(dst_normals)++ = static_cast<NormalType>(0);
        *(dst_normals)++ = static_cast<NormalType>(0);
    }
}

void StochasticVolume2Renderer::setup_shader( const float modelview_matrix[16] )
{
    int scramble_count = m_repeat_count * 12347;
    size_t random_texture_size = m_random_texture.width();
    float rp_x = ( scramble_count                       ) % random_texture_size;
    float rp_y = ( scramble_count / random_texture_size ) % random_texture_size;

    const GLfloat random_texture_size_inv = 1.0f / random_texture_size;

    const GLfloat screen_scale_x = m_width * 0.5f;
    const GLfloat screen_scale_y = m_height * 0.5f;

    m_shader_program.setUniformValuef( "random_texture_size_inv", random_texture_size_inv );
    m_shader_program.setUniformValuef( "random_offset", rp_x, rp_y );

    m_shader_program.setUniformValuef( "screen_scale", screen_scale_x, screen_scale_y );
    m_shader_program.setUniformValuef( "screen_scale_inv", 1.0f / m_width, 1.0f / m_height );

    m_shader_program.setUniformValuef( "preintegration_scale_offset",
        1.0 - 1.0 / m_table[0].sizeDepth() / m_edge_size,
        1.0 / ( 2.0 * m_table[0].sizeDepth() ) );

    int start_volume = 0;
    int end_volume = 2;
    if ( !m_show_flag0 ) start_volume++;
    if ( !m_show_flag1 ) end_volume--;

    m_shader_program.setUniformValuei( "start_volume", start_volume );
    m_shader_program.setUniformValuei( "end_volume", end_volume );

    m_shader_program.setUniformValuei( "preintegration_texture0", 0 );
    m_shader_program.setUniformValuei( "preintegration_texture1", 1 );
    m_shader_program.setUniformValuei( "random_texture", 2 );
    m_shader_program.setUniformValuei( "decomposion_texture", 3 );
}

}

}
