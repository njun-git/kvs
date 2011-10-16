//
//  ImageProcess.h
//  
//
//  Created by Jun Nishimura on 10/16/11.
//  Copyright (c) 2011 Jun Nishimura. All rights reserved.
//

#ifndef IMAGE_PROCESS_H_INCLUDE
#define IMAGE_PROCESS_H_INCLUDE

#include <kvs/ImageObject>
#include <kvs/Texture2D>
#include <kvs/OpenGL>
#include <kvs/Key>
#include <kvs/KeyEvent>

#include <kvs/cl/OpenCL>
#include <kvs/cl/Platform>
#include <kvs/cl/Device>
#include <kvs/cl/Context>
#include <kvs/cl/CommandQueue>
#include <kvs/cl/KernelSource>
#include <kvs/cl/ProgramObject>
#include <kvs/cl/KernelObject>
#include <kvs/cl/BufferObject>

class ImageProcess
{

public:

    enum ProcessType
    {
        GrayScale = 0,
        SepiaTone,
        LaplacianFilter,
        SobelFilter,
        NumberOfTypes,
    };

protected:

    bool            m_show_src_image;
    ProcessType     m_process_type;

    // Source image.
    kvs::ImageObject*       m_src_image;

    // Texture2D.
    kvs::Texture2D          m_src_texture;
    kvs::Texture2D          m_dst_texture;

    // OpenCL.
    kvs::cl::Platform       m_platform;
    kvs::cl::Device         m_device;
    kvs::cl::Context        m_context;
    kvs::cl::CommandQueue   m_queue;
    kvs::cl::ProgramObject  m_program;
    kvs::cl::KernelObject   m_kernel[NumberOfTypes];

    // CL image buffer.
    kvs::cl::BufferObject   m_src_buffer;
    kvs::cl::BufferObject   m_dst_buffer;

public:

    ImageProcess( void );

    ImageProcess( kvs::ImageObject* src_image );

    virtual ~ImageProcess( void );

public:

    void initialize( void );

    void clear( void );

public:

    void run( void );

    void render( void );

    void keyPressEvent( kvs::KeyEvent* event );

public:

    void setSourceImage( kvs::ImageObject* src_image );

    void setProcessType( const ProcessType& type );

private:

    void create_texture( void );

    void render_texture( kvs::Texture2D& texture );

};

#endif
