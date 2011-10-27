//
//  TextureRenderer.h
//  
//
//  Created by Jun Nishimura on 10/27/11.
//  Copyright (c) 2011 Jun Nishimura. All rights reserved.
//

#ifndef KVS__OPENNI__TEXTURE_RENDERER_H_INCLUDE
#define KVS__OPENNI__TEXTURE_RENDERER_H_INCLUDE

#include <kvs/RendererBase>
#include <kvs/ClassName>
#include <kvs/Texture2D>
#include <kvs/Module>

#include "TextureObject.h"

namespace kvs
{

namespace ni
{

class TextureRenderer : public kvs::RendererBase
{

    // Class name.
    kvsClassName( kvs::ni::TextureRenderer );

    // Module information.
    kvsModuleCategory( Renderer );
    kvsModuleBaseClass( kvs::RendererBase );

public:

    enum Type
    {
        Stretch,
        Center,
        LeftTop,
        LeftBottom,
        RightTop,
        RightBottom
    };

protected:

    float   m_initial_aspect_ratio;
    float   m_left;
    float   m_right;
    float   m_bottom;
    float   m_top;
    float   m_scale;
    Type    m_type;

    kvs::ValueArray<float>  m_coords;
    kvs::ValueArray<float>  m_texcoords;
    kvs::Texture2D          m_texture;

public:

    TextureRenderer( const Type& type = RightBottom );

    virtual ~TextureRenderer( void );

public:

    const float scale( void ) const;

public:

    void setType( const Type& type );

    void setScale( const float scale );

public:

    void exec( kvs::ObjectBase* object, kvs::Camera* camera, kvs::Light* light );

protected:

    void create_texture( const kvs::ni::TextureObject* object );

    void centering( const float width, const float height );

    void calculate_vertices( void );

};

}

}

#endif
