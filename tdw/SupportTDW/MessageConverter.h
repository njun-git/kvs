//
//  MessageConverter.h
//  
//
//  Created by Jun Nishimura on 11/13/9.
//  Copyright 2011 Jun Nishimura. All rights reserved.
//

#ifndef KVS__TDW__MESSAGE_CONVERTER_H_INCLUDE
#define KVS__TDW__MESSAGE_CONVERTER_H_INCLUDE

#include <kvs/ClassName>
#include <kvs/MessageBlock>
#include <kvs/ScreenBase>
#include <kvs/KeyEvent>
#include <kvs/TimeEvent>
#include <cstdarg>

#define KVS_TDW_PAINT_EVENT "PAINT"
#define KVS_TDW_KEY_EVENT "KEY_PRESS"
#define KVS_TDW_TIMER_ENENT "TIMER"

#define KVS_TDW_PAINT_EVENT_ROTATION "ROTATION"
#define KVS_TDW_PAINT_EVENT_TRANSLATION "TRANSLATION"
#define KVS_TDW_PAINT_EVENT_SCALING "SCALING"
#define KVS_TDW_PAINT_EVENT_NONE "NONE"

namespace kvs
{

namespace tdw
{

class MessageConverter
{

    kvsClassName( kvs::tdw::MessageConverter );

public:

    MessageConverter( void );

    virtual ~MessageConverter( void );

public:

    const std::string messageType( const kvs::MessageBlock& block );

    void applyPaintEvent( kvs::ScreenBase* screen, const kvs::MessageBlock& block );

    kvs::KeyEvent* keyEvent( const kvs::MessageBlock& block );

    kvs::TimeEvent* timerEvent( const kvs::MessageBlock& block );

public:

    // Change Xform, and redraw.
    const kvs::MessageBlock paintEventMessage( kvs::ScreenBase* screen = 0 );

    // Apply key event, and redraw.
    const kvs::MessageBlock keyEventMessage( kvs::KeyEvent* event );

    // Apply Timer event, and redraw.
    const kvs::MessageBlock timerEventMessage( kvs::TimeEvent* event = 0 );

public:

    template <typename T>
    const kvs::MessageBlock CreateMessage( const std::string& header, const int argc, ... );

private:

    void rotate( kvs::ScreenBase* screen, const kvs::Quaternion<float>& q );

    void scale( kvs::ScreenBase* screen, const kvs::Vector3f& s );

    void translate( kvs::ScreenBase* screen, const kvs::Vector3f& t );

};

template <typename T>
inline const kvs::MessageBlock MessageConverter::CreateMessage( const std::string& header, const int argc, ... )
{
    const size_t header_size = header.length();
    const size_t byte_size = sizeof(T);
    const size_t message_size = header_size + argc * byte_size;

    kvs::ValueArray<unsigned char> data( message_size );
    unsigned char* p = data.pointer();
    size_t loc = 0;

    memcpy( p, header.c_str(), header_size ); loc += header_size;

    va_list list;
    va_start( list, argc );

    for ( int i = 0; i < argc; i++ )
    {
        if ( typeid(T) == typeid(float) || typeid(T) == typeid(double) )
        {
            const T value = static_cast<T>( va_arg( list, double ) );
            memcpy( p + loc, &value, byte_size );
        }
        else
        {
            const T value = static_cast<T>( va_arg( list, int ) );
            memcpy( p + loc, &value, byte_size );
        }
        loc += byte_size;
    }
    va_end( list );

    return( kvs::MessageBlock( data.pointer(), data.size() ) );
}

}

}

#endif
