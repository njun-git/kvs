//
//  GestureGenerator.h
//  
//
//  Created by Jun Nishimura on 10/19/11.
//  Copyright (c) 2011 Jun Nishimura. All rights reserved.
//

#ifndef KVS__OPENNI__GESTURE_GENERATOR_H_INCLUDE
#define KVS__OPENNI__GESTURE_GENERATOR_H_INCLUDE

#include <kvs/ClassName>
#include <list>

#include "GeneratorBase.h"
#include "GestureEventListener.h"

namespace kvs
{

namespace ni
{

class GestureGenerator : public kvs::ni::GeneratorBase
{

    // Class name.
    kvsClassName( kvs::ni::GestureGenerator );

    typedef kvs::ni::GeneratorBase BaseClass;
    typedef std::list<kvs::ni::GestureRecognizedEventListener*> RecognizedList;
    typedef std::list<kvs::ni::GestureProgressEventListener*> ProgressList;

protected:

    xn::GestureGenerator    m_generator;
    XnCallbackHandle        m_handler;
    RecognizedList          m_recognized_list;
    ProgressList            m_progress_list;

public:

    GestureGenerator( void );

    virtual ~GestureGenerator( void );

public:

    const bool create( kvs::ni::Context& context );

public:

    xn::GestureGenerator& generator( void );

public:

    const bool addGesture( const std::string& gesture_name );

    void addGestureRecognizedEvent( kvs::ni::GestureRecognizedEventListener* event );

    void addGestureProgressEvent( kvs::ni::GestureProgressEventListener* event );

public:

    void gestureRecognized(
        xn::GestureGenerator& generator,
        const XnChar* gesture_name,
        const XnPoint3D* id_position,
        const XnPoint3D* end_position,
        void* cookie );

    void gestureProgress(
        xn::GestureGenerator& generator,
        const XnChar* gesture_name,
        const XnPoint3D* position,
        XnFloat progress,
        void* cookie );

};

}

}

#endif
