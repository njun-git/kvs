//
//  Context.cpp
//  
//
//  Created by Jun Nishimura on 10/18/11.
//  Copyright (c) 2011 Jun Nishimura. All rights reserved.
//

#include "Context.h"

#include <kvs/Message>

namespace kvs
{

namespace ni
{

Context::Context( void ) :
    m_is_initialized( false )
{
}

Context::~Context( void )
{
    this->shutdown();
}

const bool Context::initialize( void )
{
    if ( m_is_initialized )
    {
        kvsMessageError( "Context is already initialized." );
        return( false );
    }

    XnStatus status = m_context.Init();
    if ( status != XN_STATUS_OK )
    {
        kvsMessageError( "%s error: <%s>.",
            xnGetStatusName( status ),
            xnGetStatusString( status ) );
        exit( EXIT_FAILURE );
    }

    m_is_initialized = true;
    return( true );
}

void Context::shutdown( void )
{
    if ( m_is_initialized )
    {
        m_context.Shutdown();
        m_is_initialized = false;
    }
}

const bool Context::isMirror( void )
{
    if ( !m_is_initialized ) return( false );

    return( static_cast<bool>( m_context.GetGlobalMirror() ) );
}

xn::Context& Context::context( void )
{
    return( m_context );
}

const bool Context::setMirror( const bool is_mirror )
{
    if ( !m_is_initialized )
    {
        kvsMessageError( "Context is not initialized yet." );
        return( false );
    }

    XnStatus status = m_context.SetGlobalMirror( static_cast<XnBool>( is_mirror ) );
    if ( status != XN_STATUS_OK )
    {
        kvsMessageError( "%s error: <%s>.",
            xnGetStatusName( status ),
            xnGetStatusString( status ) );
        return( false );
    }

    return( true );
}

const bool Context::startGeneration( void )
{
    if ( !m_is_initialized )
    {
        kvsMessageError( "Context is not initialized yet." );
        return( false );
    }

    XnStatus status = m_context.StartGeneratingAll();
    if ( status != XN_STATUS_OK )
    {
        kvsMessageError( "%s error: <%s>.",
            xnGetStatusName( status ),
            xnGetStatusString( status ) );
        return( false );
    }

    return( true );
}

const bool Context::stopGeneration( void )
{
    if ( !m_is_initialized )
    {
        kvsMessageError( "Context is not initialized yet." );
        return( false );
    }

    XnStatus status = m_context.StopGeneratingAll();
    if ( status != XN_STATUS_OK )
    {
        kvsMessageError( "%s error: <%s>.",
            xnGetStatusName( status ),
            xnGetStatusString( status ) );
        return( false );
    }

    return( true );
}

const bool Context::update( kvs::ni::GeneratorBase& generator )
{
    if ( !m_is_initialized )
    {
        kvsMessageError( "Context is not initialized yet." );
        return( false );
    }

    XnStatus status = m_context.WaitOneUpdateAll( generator.generator() );
    if ( status != XN_STATUS_OK )
    {
        kvsMessageError( "%s error: <%s>.",
            xnGetStatusName( status ),
            xnGetStatusString( status ) );
        return( false );
    }

    return( true );
}

}

}
