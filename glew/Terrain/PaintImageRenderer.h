/*
 *  PaintImageRenderer.h
 *  
 *
 *  Created by njun on 11/10/10.
 *  Copyright 2011 Jun Nishimura. All rights reserved.
 *
 */

#ifndef KVS__PAINT_IMAGE_RENDERER_H_INCLUDE
#define KVS__PAINT_IMAGE_RENDERER_H_INCLUDE

#include <kvs/RendererBase>
#include <kvs/ClassName>
#include <kvs/Module>
#include "PaintImageObject.h"

namespace kvs
{

class PaintImageRenderer : public kvs::RendererBase
{

    // Class name.
    kvsClassName( kvs::PaintImageRenderer );

    // Module information.
    kvsModuleCategory( Renderer );
    kvsModuleBaseClass( kvs::RendererBase );

public:

    PaintImageRenderer( void );

    virtual ~PaintImageRenderer( void );

public:

    void exec( kvs::ObjectBase* object, kvs::Camera* camera, kvs::Light* light );

};

}

#endif
