//
//  GestureGenerator.cpp
//  
//
//  Created by Jun Nishimura on 10/19/11.
//  Copyright (c) 2011 Jun Nishimura. All rights reserved.
//

#include "GestureGenerator.h"

#include "Context.h"

namespace { kvs::ni::GestureGenerator* context; }

namespace kvs
{

namespace ni
{

void XN_CALLBACK_TYPE GestureRecognizedEvent(
    xn::GestureGenerator& generator,
    const XnChar* gesture_name,
    const XnPoint3D* id_position,
    const XnPoint3D* end_position,
    void* cookie )
{
    ::context->gestureRecognized( generator, gesture_name, id_position, end_position, cookie );
}

void XN_CALLBACK_TYPE GestureProgressEvent(
    xn::GestureGenerator& generator,
    const XnChar* gesture_name,
    const XnPoint3D* position,
    XnFloat progress,
    void* cookie )
{
    ::context->gestureProgress( generator, gesture_name, position, progress, cookie );
}

GestureGenerator::GestureGenerator( void ) :
    kvs::ni::GeneratorBase()
{
    context = this;
}

GestureGenerator::~GestureGenerator( void )
{
}

const bool GestureGenerator::create( kvs::ni::Context& context )
{
    if ( m_is_created )
    {
        kvsMessageError( "GestureGenerator is already created." );
        return( false );
    }

    {
        XnStatus status = m_generator.Create( context.context() );
        if ( status != XN_STATUS_OK )
        {
            kvsMessageError( "%s error: <%s>.",
                xnGetStatusName( status ),
                xnGetStatusString( status ) );
            return( false );
        }
    }

    {
        XnStatus status = m_generator.RegisterGestureCallbacks(
            GestureRecognizedEvent, GestureProgressEvent, NULL, m_handler );
        if ( status != XN_STATUS_OK )
        {
            kvsMessageError( "%s error: <%s>.",
                xnGetStatusName( status ),
                xnGetStatusString( status ) );
            return( false );
        }
    }

    m_is_created = true;
    return( true );
}

xn::GestureGenerator& GestureGenerator::generator( void )
{
    return( m_generator );
}

const bool GestureGenerator::addGesture( const std::string& gesture_name )
{
    if ( !m_is_created )
    {
        kvsMessageError( "GestureGenerator is not created yet." );
        return( false );
    }

    XnStatus status = m_generator.AddGesture( gesture_name.c_str(), NULL );
    if ( status != XN_STATUS_OK )
    {
        kvsMessageError( "%s error: <%s>.",
            xnGetStatusName( status ),
            xnGetStatusString( status ) );
        return( false );
    }

    return( true );
}

void GestureGenerator::addGestureRecognizedEvent( kvs::ni::GestureRecognizedEventListener* event )
{
    m_recognized_list.push_back( event );
}

void GestureGenerator::addGestureProgressEvent( kvs::ni::GestureProgressEventListener* event )
{
    m_progress_list.push_back( event );
}

void GestureGenerator::gestureRecognized(
    xn::GestureGenerator& generator,
    const XnChar* gesture_name,
    const XnPoint3D* id_position,
    const XnPoint3D* end_position,
    void* cookie )
{
    const std::string str_gesture( gesture_name );
    const kvs::Vector3f id_pos( id_position->X, id_position->Y, id_position->Z );
    const kvs::Vector3f end_pos( end_position->X, end_position->Y, end_position->Z );

    RecognizedList::iterator itr = m_recognized_list.begin();
    RecognizedList::iterator end = m_recognized_list.end();
    while ( itr != end )
    {
        (*itr)->update( str_gesture, id_pos, end_pos );
        itr++;
    }
}

void GestureGenerator::gestureProgress(
    xn::GestureGenerator& generator,
    const XnChar* gesture_name,
    const XnPoint3D* position,
    XnFloat progress,
    void* cookie )
{
    const std::string str_gesture( gesture_name );
    const kvs::Vector3f pos( position->X, position->Y, position->Z );
    const float prog = static_cast<float>( progress );

    ProgressList::iterator itr = m_progress_list.begin();
    ProgressList::iterator end = m_progress_list.end();
    while ( itr != end )
    {
        (*itr)->update( str_gesture, pos, prog );
        itr++;
    }
}

}

}
