//
//  TextureObject.h
//  
//
//  Created by Jun Nishimura on 10/27/11.
//  Copyright (c) 2011 Jun Nishimura. All rights reserved.
//

#ifndef KVS__OPENNI__TEXTURE_OBJECT_H_INCLUDE
#define KVS__OPENNI__TEXTURE_OBJECT_H_INCLUDE

#include <kvs/ObjectBase>
#include <kvs/ClassName>
#include <kvs/ValueArray>
#include <kvs/Module>

#include "OpenNI.h"
#include "ImageGenerator.h"
#include "DepthGenerator.h"
#include "UserGenerator.h"

namespace kvs
{

namespace ni
{

class TextureObject : public kvs::ObjectBase
{

    // Class name.
    kvsClassName( kvs::ni::TextureObject );

    // Module information.
    kvsModuleCategory( Object );
    kvsModuleBaseClass( kvs::ObjectBase );

protected:

    size_t      m_width;
    size_t      m_height;
    kvs::ValueArray<unsigned char>  m_data;

public:

    TextureObject( void );

    virtual ~TextureObject( void );

public:

    const ObjectType objectType( void ) const;

    const size_t width( void ) const;

    const size_t height( void ) const;

    const kvs::ValueArray<unsigned char>& data( void ) const;

public:

    void update( kvs::ni::ImageGenerator& image );

    void update( kvs::ni::DepthGenerator& depth );

    void update( kvs::ni::UserGenerator& user );

    void update(
        kvs::ni::DepthGenerator& depth,
        kvs::ni::UserGenerator& user );

};

}

}

#endif
