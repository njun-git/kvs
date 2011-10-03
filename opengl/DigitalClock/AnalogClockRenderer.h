//
//  AnalogClockRenderer.h
//  
//
//  Created by Jun Nishimura on 11/25/8.
//  Copyright 2011 Jun Nishimura. All rights reserved.
//

#ifndef KVS__ANALOG_CLOCK_RENDERER_H_INCLUDE
#define KVS__ANALOG_CLOCK_RENDERER_H_INCLUDE

#include <kvs/RendererBase>
#include <kvs/ClassName>
#include <kvs/Module>
#include "AnalogClockObject.h"

namespace kvs
{

class AnalogClockRenderer : public kvs::RendererBase
{
    // Class name.
    kvsClassName( kvs::AnalogClockRenderer );

    // Module information.
    kvsModuleCategory( Renderer );
    kvsModuleBaseClass( kvs::RendererBase );

protected:

    mutable bool m_enable_anti_aliasing; ///< flag for anti-aliasing (AA)
    mutable bool m_enable_multisample_anti_aliasing; ///< flag for multisample anti-aliasing (MSAA)
    mutable bool m_enable_two_side_lighting; ///< flag for two-side lighting

public:

    AnalogClockRenderer( void );

    virtual ~AnalogClockRenderer( void );

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
