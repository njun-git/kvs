//
//  DigitalClockRenderer.h
//  
//
//  Created by Jun Nishimura on 11/24/8.
//  Copyright 2011 Jun Nishimura. All rights reserved.
//

#ifndef KVS__DIGITAL_CLOCK_RENDERER_H_INCLUDE
#define KVS__DIGITAL_CLOCK_RENDERER_H_INCLUDE

#include <kvs/RendererBase>
#include <kvs/ClassName>
#include <kvs/Module>

#include "DigitalClockObject.h"

namespace kvs
{

class DigitalClockRenderer : public kvs::RendererBase
{
    // Class name.
    kvsClassName( kvs::DigitalClockRenderer );

    // Module information.
    kvsModuleCategory( Renderer );
    kvsModuleBaseClass( kvs::RendererBase );

protected:

    mutable bool m_enable_anti_aliasing; ///< flag for anti-aliasing (AA)
    mutable bool m_enable_multisample_anti_aliasing; ///< flag for multisample anti-aliasing (MSAA)
    mutable bool m_enable_two_side_lighting; ///< flag for two-side lighting

public:

    DigitalClockRenderer( void );

    virtual ~DigitalClockRenderer( void );

public:

    void exec( kvs::ObjectBase* object, kvs::Camera* camera, kvs::Light* light );

public:

    void enableAntiAliasing( const bool multisample = false ) const;

    void disableAntiAliasing( void ) const;

    void enableTwoSideLighting( void ) const;

    void disableTwoSideLighting( void ) const;

    const bool isTwoSideLighting( void ) const;

protected:

    virtual void initialize_projection( void );

    virtual void initialize_modelview( void );
};

}

#endif
