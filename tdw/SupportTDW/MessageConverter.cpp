//
//  MessageConverter.cpp
//  
//
//  Created by Jun Nishimura on 11/13/9.
//  Copyright 2011 Jun Nishimura. All rights reserved.
//

#include "MessageConverter.h"

namespace kvs
{

namespace tdw
{

MessageConverter::MessageConverter( void )
{
}

MessageConverter::~MessageConverter( void )
{
}

const std::string MessageConverter::messageType( const kvs::MessageBlock& block )
{
    const size_t byte_size = block.size();
    unsigned char* pData = (unsigned char*)block.pointer();
    std::string message_type;
    for ( size_t i = 0; i < byte_size; i++ )
    {
        char data = *( (char*)pData );
        pData += sizeof(char);
        if ( data == ':' ) break;
        else message_type += data;
    }
    return( message_type );
}

const std::vector<kvs::MessageBlock> MessageConverter::eventStack( const kvs::MessageBlock& block )
{
    std::vector<kvs::MessageBlock> stack;
    if ( this->messageType( block ) == KVS_TDW_STACK_EVENT )
    {
        unsigned char* pData = (unsigned char*)block.pointer();
        pData += sizeof(unsigned char) * 6;
        const unsigned int nstacks = *( (unsigned int*)pData ); pData += sizeof(unsigned int);
        for ( size_t i = 0; i < nstacks; i++ )
        {
            const unsigned int data_size = *( (unsigned int*)pData ); pData += sizeof(unsigned int);
            kvs::MessageBlock message( pData, data_size );
            pData += data_size;
            stack.push_back( message );
        }
    }
    else
    {
        stack.push_back( block );
    }
    return( stack );
}

void MessageConverter::applyPaintEvent( kvs::ScreenBase* screen, const kvs::MessageBlock& block )
{
    unsigned char* pData = (unsigned char*)block.pointer();
    const size_t byte_size = block.size();
    std::string message_type;
    for ( size_t i = 0; i < byte_size; i++ )
    {
        char data = *( (char*)pData );
        pData += sizeof(char);
        if ( data == ':' ) break;
        else message_type += data;
    }

    if ( message_type != KVS_TDW_PAINT_EVENT ) return;

    std::string paint_type;
    for ( size_t i = message_type.length(); i < byte_size; i++ )
    {
        char data = *( (char*)pData );
        pData += sizeof(char);
        if ( data == ':' ) break;
        else paint_type += data;
    }

    if ( paint_type == KVS_TDW_PAINT_EVENT_ROTATION )
    {
        kvs::Quaternion<float> q;
        q[0] = *( (float*)pData ); pData += sizeof(float);
        q[1] = *( (float*)pData ); pData += sizeof(float);
        q[2] = *( (float*)pData ); pData += sizeof(float);
        q[3] = *( (float*)pData ); pData += sizeof(float);

        this->rotate( screen, q );
    }
    else if ( paint_type == KVS_TDW_PAINT_EVENT_SCALING )
    {
        kvs::Vector3f s;
        s[0] = *( (float*)pData ); pData += sizeof(float);
        s[1] = *( (float*)pData ); pData += sizeof(float);
        s[2] = *( (float*)pData ); pData += sizeof(float);

        this->scale( screen, s );
    }
    else if ( paint_type == KVS_TDW_PAINT_EVENT_TRANSLATION )
    {
        kvs::Vector3f t;
        t[0] = *( (float*)pData ); pData += sizeof(float);
        t[1] = *( (float*)pData ); pData += sizeof(float);
        t[2] = *( (float*)pData ); pData += sizeof(float);

        this->translate( screen, t );
    }
}

kvs::KeyEvent* MessageConverter::keyEvent( const kvs::MessageBlock& block )
{
    unsigned char* pData = (unsigned char*)block.pointer();
    const size_t byte_size = block.size();
    std::string message_type;
    for ( size_t i = 0; i < byte_size; i++ )
    {
        char data = *( (char*)pData );
        pData += sizeof(char);
        if ( data == ':' ) break;
        else message_type += data;
    }

    if ( message_type != KVS_TDW_KEY_EVENT ) return( NULL );

    const int key = *( (int*)pData ); pData += sizeof(int);
    const int x = *( (int*)pData ); pData += sizeof(int);
    const int y = *( (int*)pData ); pData += sizeof(int);

    kvs::KeyEvent* event = new kvs::KeyEvent( key, x, y );
    return( event );
}

kvs::TimeEvent* MessageConverter::timerEvent( const kvs::MessageBlock& block )
{
    unsigned char* pData = (unsigned char*)block.pointer();
    const std::string message_type = this->messageType( block );
    pData += sizeof(char) * message_type.length();

    if ( message_type != KVS_TDW_TIMER_EVENT ) return( NULL );

    kvs::TimeEvent* event = new kvs::TimeEvent();
    return( event );
}

const kvs::MessageBlock MessageConverter::paintEventMessage( kvs::ScreenBase* screen )
{
    std::string header = std::string( KVS_TDW_PAINT_EVENT ) + std::string( ":" );
    if ( screen )
    {
        if ( screen->mouse()->mode() == kvs::Mouse::Rotation )
        {
            kvs::Quaternion<float> q = screen->mouse()->rotation();
            header += std::string( KVS_TDW_PAINT_EVENT_ROTATION ) + std::string( ":" );

            return( kvs::tdw::MessageConverter::CreateMessage<float>( header, 4, q[0], q[1], q[2], q[3] ) );
        }
        else if ( screen->mouse()->mode() == kvs::Mouse::Scaling )
        {
            kvs::Vector3f s = screen->mouse()->scaling();
            header += std::string( KVS_TDW_PAINT_EVENT_SCALING ) + std::string( ":" );

            return( kvs::tdw::MessageConverter::CreateMessage<float>( header, 3, s[0], s[1], s[2] ) );
        }
        else if ( screen->mouse()->mode() == kvs::Mouse::Translation )
        {
            kvs::Vector3f t = screen->mouse()->translation();
            header += std::string( KVS_TDW_PAINT_EVENT_TRANSLATION ) + std::string( ":" );

            return( kvs::tdw::MessageConverter::CreateMessage<float>( header, 3, t[0], t[1], t[2] ) );
        }
    }

    header += std::string( KVS_TDW_PAINT_EVENT_NONE ) + std::string( ":" );

    return( kvs::tdw::MessageConverter::CreateMessage<int>( header, 0 ) );
}

const kvs::MessageBlock MessageConverter::keyEventMessage( kvs::KeyEvent* event )
{
    const std::string header = std::string( KVS_TDW_KEY_EVENT ) + std::string( ":" );
    const int key = event->key();
    const int x = event->x();
    const int y = event->y();

    return( kvs::tdw::MessageConverter::CreateMessage<int>( header, 3, key, x, y ) );
}

const kvs::MessageBlock MessageConverter::timerEventMessage( kvs::TimeEvent* event )
{
    const std::string header = std::string( KVS_TDW_TIMER_EVENT ) + std::string( ":" );

    return( kvs::tdw::MessageConverter::CreateMessage<int>( header, 0 ) );
}

const kvs::MessageBlock MessageConverter::eventStackMessage( const std::vector<kvs::MessageBlock>& stack )
{
    const std::string header = std::string( KVS_TDW_STACK_EVENT ) + std::string( ":" );
    unsigned int nstacks = stack.size();
    size_t message_size = header.length() + sizeof(unsigned int) * ( 1 + nstacks );
    for ( unsigned int i = 0; i < nstacks; i++ ) message_size += stack[i].size();

    kvs::ValueArray<unsigned char> data( message_size );
    unsigned char* p = data.pointer();
    size_t loc = 0;

    memcpy( p, header.c_str(), header.length() ); loc += header.length();
    memcpy( p + loc, &nstacks, sizeof(nstacks) ); loc += sizeof(nstacks);

    for ( unsigned int i = 0; i < nstacks; i++ )
    {
        unsigned int block_size = stack[i].size();
        memcpy( p + loc, &block_size, sizeof(block_size) ); loc += sizeof(block_size);
        memcpy( p + loc, stack[i].pointer(), block_size );  loc += block_size;
    }

    return( kvs::MessageBlock( data.pointer(), data.size() ) );
}

void MessageConverter::rotate( kvs::ScreenBase* screen, const kvs::Quaternion<float>& q )
{
    if ( screen )
    {
        switch( screen->controlTarget() )
        {
        case kvs::ScreenBase::TargetCamera:
            screen->camera()->rotate( q.toMatrix() );
            break;
        case kvs::ScreenBase::TargetLight:
            screen->light()->rotate( q.toMatrix() );
            break;
        case kvs::ScreenBase::TargetObject:
            screen->objectManager()->rotate( q.toMatrix() );
            break;
        default: break;
        }
    }
}

void MessageConverter::scale( kvs::ScreenBase* screen, const kvs::Vector3f& s )
{
    if ( screen )
    {
        switch( screen->controlTarget() )
        {
        case kvs::ScreenBase::TargetCamera:
            screen->camera()->scale( s );
            break;
        case kvs::ScreenBase::TargetLight:
            screen->light()->scale( s );
            break;
        case kvs::ScreenBase::TargetObject:
            screen->objectManager()->scale( s );
            break;
        default: break;
        }
    }
}

void MessageConverter::translate( kvs::ScreenBase* screen, const kvs::Vector3f& t )
{
    if ( screen )
    {
        switch( screen->controlTarget() )
        {
        case kvs::ScreenBase::TargetCamera:
            screen->camera()->translate( t );
            break;
        case kvs::ScreenBase::TargetLight:
            screen->light()->translate( t );
            break;
        case kvs::ScreenBase::TargetObject:
            screen->objectManager()->translate( t );
            break;
        default: break;
        }
    }
}

}

}
