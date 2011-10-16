//
//  ImageProcess.cpp
//  
//
//  Created by Jun Nishimura on 10/16/11.
//  Copyright (c) 2011 Jun Nishimura. All rights reserved.
//

#include "ImageProcess.h"

#include <kvs/Timer>

ImageProcess::ImageProcess( void ) :
    m_show_src_image( false ),
    m_process_type( GrayScale ),
    m_src_image( NULL )
{
}

ImageProcess::ImageProcess( kvs::ImageObject* src_image ) :
    m_show_src_image( false ),
    m_process_type( GrayScale ),
    m_src_image( NULL )
{
    this->setSourceImage( src_image );
}

ImageProcess::~ImageProcess( void )
{
    this->clear();
}

void ImageProcess::initialize( void )
{
    if ( !m_src_image )
    {
        kvsMessageError( "Source image is not set yet." );
        return;
    }

    this->create_texture();

    m_device.initialize( m_platform.platform() );
    m_context.create( m_device );
    m_queue.create( m_context, m_device.device() );

    kvs::cl::KernelSource source( "image_process.cl" );
    m_program.create( m_context, source );
    m_program.build();

    m_kernel[0].create( m_program, "gray_scale" );
    m_kernel[1].create( m_program, "sepia_tone" );
    m_kernel[2].create( m_program, "laplacian_filter" );
    m_kernel[3].create( m_program, "sobel_filter" );

    m_src_buffer.setAccessType( kvs::cl::BufferObject::ReadOnly );
    m_src_buffer.create( m_context, m_src_texture );

    m_dst_buffer.setAccessType( kvs::cl::BufferObject::WriteOnly );
    m_dst_buffer.create( m_context, m_dst_texture );

    for ( int i = 0; i < NumberOfTypes; i++ )
    {
        m_kernel[i].setArgument( 0, sizeof(kvs::cl::CLMemory), &m_src_buffer.memory() );
        m_kernel[i].setArgument( 1, sizeof(kvs::cl::CLMemory), &m_dst_buffer.memory() );
    }

    this->run();
}

void ImageProcess::clear( void )
{
    if ( m_src_image )
    {
        delete m_src_image;
        m_src_image = NULL;
    }
}

void ImageProcess::run( void )
{
    kvs::Timer timer;
    timer.start();

    m_src_buffer.acquireGL( m_queue );
    m_dst_buffer.acquireGL( m_queue );

    const size_t work_size[2] = { m_src_image->width(), m_src_image->height() };
    m_kernel[ m_process_type ].run( m_queue, 2, work_size );

    m_src_buffer.releaseGL( m_queue );
    m_dst_buffer.releaseGL( m_queue );

    m_queue.finish();

    timer.stop();
    std::cout << "Processing Time [" << m_kernel[ m_process_type ].name() << "] : " << timer.msec() << " [msec]" << std::endl;
}

void ImageProcess::render( void )
{
    if ( m_show_src_image )
    {
        if ( m_src_texture.isTexture() )
        {
            this->render_texture( m_src_texture );
        }
    }
    else
    {
        if ( m_dst_texture.isTexture() )
        {
            this->render_texture( m_dst_texture );
        }
    }
}

void ImageProcess::keyPressEvent( kvs::KeyEvent* event )
{
    switch ( event->key() )
    {
        case kvs::Key::s:
        {
            m_show_src_image = true;
            break;
        }
        case kvs::Key::d:
        {
            m_show_src_image = false;
            break;
        }
        case kvs::Key::Up:
        {
            this->setProcessType( ProcessType( m_process_type + 1 ) );
            this->run();
            break;
        }
        case kvs::Key::Down:
        {
            this->setProcessType( ProcessType( m_process_type - 1 ) );
            this->run();
            break;
        }
        default: break;
    }
}

void ImageProcess::setSourceImage( kvs::ImageObject* src_image )
{
    m_src_image = src_image;
}

void ImageProcess::setProcessType( const ProcessType& type )
{
    if ( type >= NumberOfTypes )
    {
        m_process_type = ProcessType( 0 );
    }
    else if ( type < 0 )
    {
        m_process_type = ProcessType( NumberOfTypes - 1 );
    }
    else
    {
        m_process_type = type;
    }
}

void ImageProcess::create_texture( void )
{
    m_src_texture.release();
    m_dst_texture.release();

    if ( m_src_image->type() != kvs::ImageObject::Color24 )
    {
        kvsMessageError( "Unknown pixel color type." );
        return;
    }

    const size_t nchannels         = 4;
    const size_t bytes_per_channel = 1;

    const size_t npixels = m_src_image->width() * m_src_image->height();
    kvs::ValueArray<unsigned char> data( 4 * npixels );
    for ( size_t i = 0; i < npixels; i++ )
    {
        data[ i * 4 + 0 ] = m_src_image->data()[ i * 3 + 0 ];
        data[ i * 4 + 1 ] = m_src_image->data()[ i * 3 + 1 ];
        data[ i * 4 + 2 ] = m_src_image->data()[ i * 3 + 2 ];
        data[ i * 4 + 3 ] = 255;
    }

    m_src_texture.setPixelFormat( nchannels, bytes_per_channel );
    m_src_texture.create( m_src_image->width(), m_src_image->height() );
    m_src_texture.download( m_src_image->width(), m_src_image->height(), data.pointer() );
    m_src_texture.unbind();

    m_dst_texture.setPixelFormat( nchannels, bytes_per_channel );
    m_dst_texture.create( m_src_image->width(), m_src_image->height() );
    m_dst_texture.unbind();

    glFlush();
}

void ImageProcess::render_texture( kvs::Texture2D& texture )
{
    glDisable( GL_DEPTH_TEST );
    glEnable( GL_TEXTURE_2D );
    texture.bind();

    float left   = 0.0f;
    float right  = 1.0f;
    float bottom = 0.0f;
    float top    = 1.0f;

    const float texture_aspect_ratio = static_cast<float>( texture.width() ) / texture.height();

    GLint viewport[4]; glGetIntegerv( GL_VIEWPORT, viewport );    
    const float current_aspect_ratio = static_cast<float>( viewport[2] ) / viewport[3];
    const float aspect_ratio = current_aspect_ratio / texture_aspect_ratio;
    if( aspect_ratio >= 1.0 )
    {
        left   = ( 1.0 - aspect_ratio ) * 0.5;
        right  = ( 1.0 + aspect_ratio ) * 0.5;
        bottom = 0.0;
        top    = 1.0;
    }
    else
    {
        left   = 0.0;
        right  = 1.0;
        bottom = ( 1.0 - 1.0 / aspect_ratio ) * 0.5;
        top    = ( 1.0 + 1.0 / aspect_ratio ) * 0.5;
    }

    glMatrixMode( GL_MODELVIEW );
    glPushMatrix();
    {
        glLoadIdentity();

        glMatrixMode( GL_PROJECTION );
        glPushMatrix();
        {
            glLoadIdentity();
            glOrtho( left, right, bottom, top, - 1.0f, 1.0f );

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

    texture.unbind();
    glDisable( GL_TEXTURE_2D );

    glFlush();
}
