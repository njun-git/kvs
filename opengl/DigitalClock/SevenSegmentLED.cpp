//
//  SevenSegmentLED.cpp
//  
//
//  Created by Jun Nishimura on 11/24/8.
//  Copyright 2011 Jun Nishimura. All rights reserved.
//

#include "SevenSegmentLED.h"

#include <kvs/Math>

namespace
{
    static const bool SegmentTable[10][7] = {
        {  true,  true,  true,  true,  true,  true, false },
        { false,  true,  true, false, false, false, false },
        {  true,  true, false,  true,  true, false,  true },
        {  true,  true,  true,  true, false, false,  true },
        { false,  true,  true, false, false,  true,  true },
        {  true, false,  true,  true, false,  true,  true },
        {  true, false,  true,  true,  true,  true,  true },
        {  true,  true,  true, false, false, false, false },
        {  true,  true,  true,  true,  true,  true,  true },
        {  true,  true,  true,  true, false,  true,  true }
    };
}

namespace kvs
{

ColonLED::ColonLED( void ) :
    m_is_on( false )
{
}

ColonLED::ColonLED( const kvs::Vector3f& center ) :
    m_is_on( false )
{
    this->initialize( center );
}

ColonLED::~ColonLED( void )
{
}

void ColonLED::initialize( const kvs::Vector3f& center )
{
    this->create_coords( center );
}

const bool ColonLED::isOn( void ) const
{
    return( m_is_on );
}

const kvs::ValueArray<float>& ColonLED::coords( void ) const
{
    return( m_coords );
}

void ColonLED::setOn( const bool is_on )
{
    m_is_on = is_on;
}

void ColonLED::create_coords( const kvs::Vector3f& center )
{
    m_coords.allocate( 3 * 8 );
    const float length = 2.0;
    const float width = 0.2;

    // Set const z coord.
    for ( size_t i = 2; i < 24; i += 3 ) m_coords[i] = center.z();

    for ( size_t i = 0; i < 2; i++ )
    {
        float is_minus = ( i == 0 ) ? 1.0f : - 1.0f;
        m_coords[ i * 12 +  0 ] = center.x() - width;
        m_coords[ i * 12 +  1 ] = center.y() + is_minus * length / 2 + width;

        m_coords[ i * 12 +  3 ] = center.x() + width;
        m_coords[ i * 12 +  4 ] = center.y() + is_minus * length / 2 + width;

        m_coords[ i * 12 +  6 ] = center.x() + width;
        m_coords[ i * 12 +  7 ] = center.y() + is_minus * length / 2 - width;

        m_coords[ i * 12 +  9 ] = center.x() - width;
        m_coords[ i * 12 + 10 ] = center.y() + is_minus * length / 2 - width;
    }
}

OneSegment::OneSegment( void ) :
    m_is_on( false ),
    m_type( SegmentA )
{
}

OneSegment::OneSegment( const SegmentType type, const kvs::Vector3f& center ) :
    m_is_on( false ),
    m_type( type )
{
    this->initialize( center );
}

OneSegment::~OneSegment( void )
{
}

void OneSegment::initialize( const kvs::Vector3f& center )
{
    this->create_segment( center );
}

const bool OneSegment::isOn( void ) const
{
    return( m_is_on );
}

const kvs::ValueArray<float>& OneSegment::coords( void ) const
{
    return( m_coords );
}

void OneSegment::setOn( const bool is_on )
{
    m_is_on = is_on;
}

void OneSegment::setSegmentType( const SegmentType type )
{
    m_type = type;
}

void OneSegment::create_segment( const kvs::Vector3f& center )
{
    m_coords.deallocate();
    const float length = 2.0;
    const float width = 0.2;
    const float offset = 0.2;
    switch ( m_type )
    {
        case SegmentA:
        this->build_segment_coord( center + kvs::Vector3f( 0.0f, length, 0.0f ), length, width, width * offset, 0.75f, 0.75f, 180.0f );
        break;

        case SegmentB:
        this->build_segment_coord( center + kvs::Vector3f( length / 2, length / 2, 0.0f ), length, width, width * offset, 0.0f, 0.75f, 270.0f );
        break;

        case SegmentC:
        this->build_segment_coord( center + kvs::Vector3f( length / 2, - length / 2, 0.0f ), length, width, width * offset, 0.75f, 0.0f, 270.0f );
        break;

        case SegmentD:
        this->build_segment_coord( center + kvs::Vector3f( 0.0f, - length, 0.0f ), length, width, width * offset, 0.75f, 0.75f, 0.0f );
        break;

        case SegmentE:
        this->build_segment_coord( center + kvs::Vector3f( - length / 2, - length / 2, 0.0f ), length, width, width * offset, 0.0f, 0.75f, 90.0f );
        break;

        case SegmentF:
        this->build_segment_coord( center + kvs::Vector3f( - length / 2, length / 2, 0.0f ), length, width, width * offset, 0.75f, 0.0f, 90.0f );
        break;

        case SegmentG:
        this->build_segment_coord( center, length, width, width * offset, 0.0f, 0.0f, 0.0f );
        break;

        default: break;
    }
}

void OneSegment::build_segment_coord(
    const kvs::Vector3f& center,
    const float length,
    const float width,
    const float offset,
    const float left_ratio,
    const float right_ratio,
    const float rotate_theta )
{
    m_coords.allocate( 3 * 6 );

    // Set const z coord.
    for ( size_t i = 2; i < 18; i += 3 ) m_coords[i] = center.z();

    const float half_length = length / 2;

    m_coords[ 0] = - ( half_length + left_ratio * width - offset );
    m_coords[ 1] = - left_ratio * width;

    m_coords[ 3] = - ( half_length - width - offset );
    m_coords[ 4] = width;

    m_coords[ 6] = - ( half_length - ( 1.0f - 2 * left_ratio ) * width - offset );
    m_coords[ 7] = - width;

    m_coords[ 9] = ( half_length - width - offset );
    m_coords[10] = width;

    m_coords[12] = ( half_length - ( 1.0f - 2 * right_ratio ) * width - offset );
    m_coords[13] = - width;

    m_coords[15] = ( half_length + right_ratio * width - offset );
    m_coords[16] = - right_ratio * width;

    for ( size_t i = 0; i < 6; i++ )
    {
        const float theta = kvs::Math::Deg2Rad( rotate_theta );
        const float x = m_coords[ i * 3 + 0 ];
        const float y = m_coords[ i * 3 + 1 ];
        m_coords[ i * 3 + 0 ] = center.x() + cosf( theta ) * x + sinf( theta ) * y;
        m_coords[ i * 3 + 1 ] = center.y() - sinf( theta ) * x + cosf( theta ) * y;
    }
}

SevenSegmentLED::SevenSegmentLED( void ) :
    m_number( 0 )
{
}

SevenSegmentLED::SevenSegmentLED( const kvs::Vector3f& center ) :
    m_number( 0 )
{
    this->initialize( center );
}

SevenSegmentLED::~SevenSegmentLED( void )
{
}

void SevenSegmentLED::initialize( const kvs::Vector3f& center )
{
    for ( int i = 0; i < 7; i++ )
    {
        m_segments[i].setSegmentType( kvs::OneSegment::SegmentType(i) );
        m_segments[i].initialize( center );
    }
}

const int SevenSegmentLED::number( void ) const
{
    return( m_number );
}

const kvs::OneSegment& SevenSegmentLED::segment( const size_t type ) const
{
    if ( type >= 7 ) return( m_segments[6] );
    return( m_segments[type] );
}

void SevenSegmentLED::setNumber( const int number )
{
    if ( number < 0 )      m_number = 0;
    else if ( number > 9 ) m_number = 9;
    else                   m_number = number;

    for ( size_t i = 0; i < 7; i++ )
    {
        m_segments[i].setOn( ::SegmentTable[ m_number ][i] );
    }
}



}
