//
//  StochasticVolumeRenderer.cpp
//  
//
//  Created by Jun Nishimura on 11/4/10.
//  Copyright 2011 Jun Nishimura. All rights reserved.
//

#include "StochasticVolumeRenderer.h"

#include "SPTDecompositionTable.h"
#include <kvs/TetrahedralCell>
#include <kvs/Math>

namespace kvs
{

namespace glew
{

StochasticVolumeRenderer::Volume::Volume( void ) :
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

StochasticVolumeRenderer::Volume::~Volume( void )
{
    this->release();
}

void StochasticVolumeRenderer::Volume::release( void )
{
    if ( m_indices ) { delete[] m_indices; m_indices = NULL; }
    if ( m_coords )  { delete[] m_coords;  m_coords  = NULL; }
    if ( m_values )  { delete[] m_values;  m_values  = NULL; }
    if ( m_normals ) { delete[] m_normals; m_normals  = NULL; }
    if ( m_connections ) { delete[] m_connections; m_connections = NULL; }
}

void StochasticVolumeRenderer::Volume::create(
    const size_t nsteps,
    const size_t nvertices,
    const size_t ncells )
{
    this->release();

    m_nsteps = nsteps;
    m_nvertices = nvertices;
    m_coords = new StochasticVolumeRenderer::CoordType [ nvertices * 3 ];
    m_values = new StochasticVolumeRenderer::ValueType [ nvertices * m_nsteps ];
    m_normals = new StochasticVolumeRenderer::NormalType [ nvertices * 3 ];
    m_indices = new StochasticVolumeRenderer::IndexType [ nvertices * 2 ];

    m_ncells = ncells;
    m_connections = new StochasticVolumeRenderer::ConnectType [ ncells * 4 ];
}

const size_t StochasticVolumeRenderer::Volume::nvertices( void ) const
{
    return( m_nvertices );
}

const size_t StochasticVolumeRenderer::Volume::ncells( void ) const
{
    return( m_ncells );
}

const size_t StochasticVolumeRenderer::Volume::byteSizePerVertex( void ) const
{
    const size_t index_size  = sizeof( StochasticVolumeRenderer::IndexType ) * 2;
    const size_t coord_size  = sizeof( StochasticVolumeRenderer::CoordType ) * 3;
    const size_t value_size  = sizeof( StochasticVolumeRenderer::ValueType ) * m_nsteps;
    const size_t normal_size = sizeof( StochasticVolumeRenderer::NormalType ) * 3;

    return( index_size + coord_size + value_size + normal_size );
}

const size_t StochasticVolumeRenderer::Volume::byteSizeOfVertex( void ) const
{
    return( this->byteSizePerVertex() * m_nvertices );
}

const size_t StochasticVolumeRenderer::Volume::byteSizePerCell( void ) const
{
    return( sizeof( StochasticVolumeRenderer::ConnectType ) * 4 );
}

const size_t StochasticVolumeRenderer::Volume::byteSizeOfCell( void ) const
{
    return( this->byteSizePerCell() * m_ncells );
}

const StochasticVolumeRenderer::IndexType* StochasticVolumeRenderer::Volume::indices( void ) const
{
    return( m_indices );
}

StochasticVolumeRenderer::IndexType* StochasticVolumeRenderer::Volume::indices( void )
{
    return( m_indices );
}

const StochasticVolumeRenderer::CoordType* StochasticVolumeRenderer::Volume::coords( void ) const
{
    return( m_coords );
}

StochasticVolumeRenderer::CoordType* StochasticVolumeRenderer::Volume::coords( void )
{
    return( m_coords );
}

const StochasticVolumeRenderer::ValueType* StochasticVolumeRenderer::Volume::values( void ) const
{
    return( m_values );
}

StochasticVolumeRenderer::ValueType* StochasticVolumeRenderer::Volume::values( void )
{
    return( m_values );
}

const StochasticVolumeRenderer::NormalType* StochasticVolumeRenderer::Volume::normals( void ) const
{
    return( m_normals );
}

StochasticVolumeRenderer::NormalType* StochasticVolumeRenderer::Volume::normals( void )
{
    return( m_normals );
}

const StochasticVolumeRenderer::ConnectType* StochasticVolumeRenderer::Volume::connections( void ) const
{
    return( m_connections );
}

StochasticVolumeRenderer::ConnectType* StochasticVolumeRenderer::Volume::connections( void )
{
    return( m_connections );
}


StochasticVolumeRenderer::Renderer::Renderer( void ):
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

void StochasticVolumeRenderer::Renderer::set(
    const StochasticVolumeRenderer::Volume* volume,
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

const bool StochasticVolumeRenderer::Renderer::download(
    kvs::glew::VertexBufferObject& vbo,
    kvs::glew::IndexBufferObject& ibo )
{
    if ( m_volume == NULL ) return( false );

    const size_t size = m_nvertices;  // number of vertices
    const size_t size_i = sizeof(StochasticVolumeRenderer::IndexType) * 2 * size;
    const size_t size_c = sizeof(StochasticVolumeRenderer::CoordType) * 3 * size;
    const size_t size_v = sizeof(StochasticVolumeRenderer::ValueType) * m_nsteps * size;
    const size_t size_n = sizeof(StochasticVolumeRenderer::NormalType) * 3 * size;
    const size_t off_i = 0;
    const size_t off_c = off_i + size_i;
    const size_t off_v = off_c + size_c;
    const size_t off_n = off_v + size_v;

    const StochasticVolumeRenderer::IndexType* ptr_i = m_volume->indices();
    const StochasticVolumeRenderer::CoordType* ptr_c = m_volume->coords();
    const StochasticVolumeRenderer::ValueType* ptr_v = m_volume->values();
    const StochasticVolumeRenderer::NormalType* ptr_n = m_volume->normals();

    vbo.download( size_i, ptr_i, off_i );
    vbo.download( size_c, ptr_c, off_c );
    vbo.download( size_v, ptr_v, off_v );
    vbo.download( size_n, ptr_n, off_n );

    const StochasticVolumeRenderer::ConnectType* ptr_conn = m_volume->connections();
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

void StochasticVolumeRenderer::Renderer::draw( const size_t step ) const
{
    glEnableClientState( GL_VERTEX_ARRAY );
    glEnableClientState( GL_NORMAL_ARRAY );
    glVertexPointer( 3, GL_FLOAT, 0, (char*)(m_off_coord) );
    glNormalPointer( GL_BYTE, 0, (char*)(m_off_normal) );

    glEnableVertexAttribArray( m_loc_values );
    glVertexAttribPointer( m_loc_values, 1, GL_FLOAT, GL_FALSE, 0, (char*)( m_off_value + step * sizeof(StochasticVolumeRenderer::ValueType) * m_nvertices ) );

    glEnableVertexAttribArray( m_loc_identifier );
    glVertexAttribPointer( m_loc_identifier, 2, GL_UNSIGNED_SHORT, GL_FALSE, 0, (char*)(m_off_index) );

    glColor4f( 1.0f, 1.0f, 1.0f, 1.0f );
    glDrawElements( GL_LINES_ADJACENCY_EXT, 4 * m_ncells, GL_UNSIGNED_INT, 0 );

    glDisableClientState( GL_NORMAL_ARRAY );
    glDisableClientState( GL_VERTEX_ARRAY );
    glDisableVertexAttribArray( m_loc_values );
    glDisableVertexAttribArray( m_loc_identifier );
}


StochasticVolumeRenderer::StochasticVolumeRenderer( void )
{
    this->initialize();
    BaseClass::setShader( kvs::Shader::Lambert() );
}

StochasticVolumeRenderer::StochasticVolumeRenderer(
    const kvs::UnstructuredVolumeObject* volume,
    const size_t nsteps )
{
    this->initialize();
    this->attachVolumeObject( volume );
    this->setNSteps( nsteps );
    BaseClass::setShader( kvs::Shader::Lambert() );
}

StochasticVolumeRenderer::~StochasticVolumeRenderer( void )
{
}

void StochasticVolumeRenderer::exec(
    kvs::ObjectBase* object,
    kvs::Camera*     camera,
    kvs::Light*      light )
{
    // This function is not called.
}


void StochasticVolumeRenderer::initialize( void )
{
    BaseClass::initialize();

    m_ref_volume = NULL;

    m_volume = NULL;
    m_renderer = NULL;

    m_table.setTableSize( 128, 128 );
    m_edge_size = 1.0f;

    m_nsteps = 1;
    m_step = 0;
}

void StochasticVolumeRenderer::attachVolumeObject( const kvs::UnstructuredVolumeObject* volume )
{
    m_ref_volume = volume;

    if ( volume->cellType() != kvs::UnstructuredVolumeObject::Tetrahedra )
    {
        kvsMessageError( "Cell type of this volume is not tetrahedral cell." );
        exit(1);
    }
}

void StochasticVolumeRenderer::setNSteps( const size_t nsteps )
{
    m_nsteps = nsteps;
}

void StochasticVolumeRenderer::setStep( const size_t step )
{
    if ( step < m_nsteps ) m_step = step;
}

void StochasticVolumeRenderer::setEdgeSize( const float edge_size )
{
    m_edge_size = edge_size;
}

void StochasticVolumeRenderer::setTransferFunction( const kvs::TransferFunction& tfunc )
{
    m_tfunc = tfunc;

    if ( m_table.isDownloaded() )
    {
        m_table.release();
    }
}

const kvs::TransferFunction& StochasticVolumeRenderer::transferFunction( void ) const
{
    return( m_tfunc );
}

kvs::TransferFunction& StochasticVolumeRenderer::transferFunction( void )
{
    return( m_tfunc );
}

const StochasticRendererBase::RendererType StochasticVolumeRenderer::rendererType( void ) const
{
    return( BaseClass::Volume );
}

void StochasticVolumeRenderer::initializeShader( void )
{
    const std::string vert_code = "StochasticShader/spt.vert";
    const std::string geom_code = "StochasticShader/spt.geom";
    const std::string frag_code = "StochasticShader/spt.frag";

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

void StochasticVolumeRenderer::createVertexBuffer( void )
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

void StochasticVolumeRenderer::downloadVertexBuffer( void )
{
    m_vbo.bind();
    m_ibo.bind();
    m_renderer->download( m_vbo, m_ibo );
}

void StochasticVolumeRenderer::drawVertexBuffer( const float modelview_matrix[16] )
{
    if ( !m_table.isTexture() )
    {
        this->create_preintegration_table();
    }

    if ( BaseClass::isEnabledShading() ) glEnable( GL_LIGHTING );
    else                                 glDisable( GL_LIGHTING );

    m_vbo.bind();
    m_ibo.bind();

    glActiveTexture(GL_TEXTURE0);     m_table.bind();                   glEnable(GL_TEXTURE_3D);
    glActiveTexture(GL_TEXTURE1);     m_random_texture.bind();          glEnable(GL_TEXTURE_2D);
    glActiveTexture(GL_TEXTURE2);     m_decomposition_texture.bind();   glEnable(GL_TEXTURE_2D);
    glActiveTexture(GL_TEXTURE0);

    m_shader_program.bind();

    this->setup_shader( modelview_matrix );
    m_renderer->draw( m_step );

    m_shader_program.unbind();

    glActiveTexture(GL_TEXTURE2);    m_decomposition_texture.unbind();  glDisable(GL_TEXTURE_2D);
    glActiveTexture(GL_TEXTURE1);    m_random_texture.unbind();         glDisable(GL_TEXTURE_2D);
    glActiveTexture(GL_TEXTURE0);    m_table.unbind();                  glDisable(GL_TEXTURE_3D);

    m_repeat_count++;
}

void StochasticVolumeRenderer::clearEnsembleBuffer( void )
{
    m_repeat_count = 0;
}

void StochasticVolumeRenderer::create_shaders(
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

void StochasticVolumeRenderer::initialize_decomposition_texture( void )
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

void StochasticVolumeRenderer::create_preintegration_table( void )
{
    m_table.setTransferFunction( m_tfunc, 0.0f, 1.0f );
    m_table.create( m_edge_size );
    m_table.download();
}

template <typename T>
void StochasticVolumeRenderer::create_vertexbuffer_from_volume( void )
{
    if ( m_volume ) delete m_volume;
    m_volume = new Volume();

    const size_t nvertices = m_ref_volume->nnodes();
    const size_t ncells = m_ref_volume->ncells();

    m_volume->create( m_nsteps, nvertices, ncells );

    const kvs::Real32* src_coord   = m_ref_volume->coords().pointer();
    const kvs::UInt32* src_connect = m_ref_volume->connections().pointer();
    const T* src_value = static_cast<const T*>( m_ref_volume->values().pointer() );

    if ( !m_ref_volume->hasMinMaxValues() ) m_ref_volume->updateMinMaxValues();
    const kvs::Real64 min_value = m_ref_volume->minValue();
    const kvs::Real64 max_value = m_ref_volume->maxValue();
    const kvs::Real64 normalize_factor = 1.0 / ( max_value - min_value );

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
            ValueType value = static_cast<ValueType>( *src_value++ );
            *(dst_values)++ = static_cast<ValueType>( ( value - min_value ) * normalize_factor );
        }
    }

    for ( size_t i = 0; i < ncells; i++ )
    {
        *(dst_connections)++ = src_connect[ i * 4     ];
        *(dst_connections)++ = src_connect[ i * 4 + 1 ];
        *(dst_connections)++ = src_connect[ i * 4 + 2 ];
        *(dst_connections)++ = src_connect[ i * 4 + 3 ];
    }


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

    for ( size_t i = 0; i < nvertices; i++ )
    {
        const kvs::Vector3f v( normals.pointer() + i * 3 );
        const kvs::Vector3f n( ( v / (float)counter[i] ).normalize() * 127.0f );
        *(dst_normals)++ = static_cast<NormalType>(n[0]);
        *(dst_normals)++ = static_cast<NormalType>(n[1]);
        *(dst_normals)++ = static_cast<NormalType>(n[2]);
    }
}

void StochasticVolumeRenderer::setup_shader( const float modelview_matrix[16] )
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
        1.0 - 1.0 / m_table.sizeDepth() / m_edge_size,
        1.0 / ( 2.0 * m_table.sizeDepth() ) );

    m_shader_program.setUniformValuei( "preintegration_texture", 0 );
    m_shader_program.setUniformValuei( "random_texture", 1 );
    m_shader_program.setUniformValuei( "decomposion_texture", 2 );
}

}

}
