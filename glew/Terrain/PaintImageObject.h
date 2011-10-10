/*
 *  PaintImageObject.h
 *  
 *
 *  Created by njun on 11/10/10.
 *  Copyright 2011 Jun Nishimura. All rights reserved.
 *
 */

#ifndef KVS__PAINT_IMAGE_OBJECT_H_INCLUDE
#define KVS__PAINT_IMAGE_OBJECT_H_INCLUDE

#include <kvs/ObjectBase>
#include <kvs/ClassName>
#include <kvs/Module>
#include <kvs/ValueArray>
#include <kvs/Texture2D>
#include <kvs/Vector2>
#include <kvs/ImageImporter>

namespace kvs
{

class PaintImageObject : public kvs::ObjectBase
{

    // Class name.
    kvsClassName( kvs::PaintImageObject );

    // Module information.
    kvsModuleCategory( Object );
    kvsModuleBaseClass( kvs::ObjectBase );

protected:

    size_t                  m_width;
    size_t                  m_height;
    kvs::ValueArray<float>  m_data;

    bool                    m_is_initialize;
    kvs::Texture2D          m_texture;

public:

    PaintImageObject( void );

    PaintImageObject( const size_t width, const size_t height );

    PaintImageObject( const std::string& filename );

    virtual ~PaintImageObject( void );

public:

    const ObjectType objectType( void ) const;

    const size_t width( void ) const;

    const size_t height( void ) const;

    const kvs::ValueArray<float>& data( void ) const;

    kvs::ValueArray<float>& data( void );

    float& value( const size_t x, const size_t y );

    const bool isInitilize( void ) const;

    kvs::Texture2D& texture( void );

public:

    void setSize( const size_t width, const size_t height );

    const bool read( const std::string& filename );

    const bool write( const std::string& filename );

    void create( void );

    void download( void );

    void bind( void );

    void unbind( void );

};

}

#endif
