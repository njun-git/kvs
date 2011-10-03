//
//  SevenSegmentLED.h
//  
//
//  Created by Jun Nishimura on 11/24/8.
//  Copyright 2011 Jun Nishimura. All rights reserved.
//

#ifndef KVS__SEVEN_SEGMENT_LED_H_INCLUDE
#define KVS__SEVEN_SEGMENT_LED_H_INCLUDE

#include <kvs/ClassName>
#include <kvs/Vector3>
#include <kvs/ValueArray>

namespace kvs
{

class ColonLED
{
    // Class name.
    kvsClassName( kvs::ColonLED );

protected:

    bool                    m_is_on;
    kvs::ValueArray<float>  m_coords;

public:

    ColonLED( void );

    ColonLED( const kvs::Vector3f& center );

    virtual ~ColonLED( void );

public:

    void initialize( const kvs::Vector3f& center );

public:

    const bool isOn( void ) const;

    const kvs::ValueArray<float>& coords( void ) const;

public:

    void setOn( const bool is_on );

private:

    void create_coords( const kvs::Vector3f& center );

};

class OneSegment
{
    // Class name.
    kvsClassName( kvs::OneSegment );

public:

    enum SegmentType
    {
        SegmentA = 0,
        SegmentB,
        SegmentC,
        SegmentD,
        SegmentE,
        SegmentF,
        SegmentG
    };

protected:

    bool                    m_is_on;
    SegmentType             m_type;
    kvs::ValueArray<float>  m_coords;

public:

    OneSegment( void );

    OneSegment( const SegmentType type, const kvs::Vector3f& center );

    virtual ~OneSegment( void );

public:

    void initialize( const kvs::Vector3f& center );

public:

    const bool isOn( void ) const;

    const kvs::ValueArray<float>& coords( void ) const;

public:

    void setOn( const bool is_on );

    void setSegmentType( const SegmentType type );

private:

    void create_segment( const kvs::Vector3f& center );

    void build_segment_coord(
        const kvs::Vector3f& center,
        const float length,
        const float width,
        const float offset,
        const float left_ratio,
        const float right_ratio,
        const float rotate_theta );

};

class SevenSegmentLED
{
    // Class name.
    kvsClassName( kvs::SevenSegmentLED );

protected:

    int                 m_number;
    kvs::OneSegment     m_segments[7];

public:

    SevenSegmentLED( void );

    SevenSegmentLED( const kvs::Vector3f& center );

    virtual ~SevenSegmentLED( void );

public:

    void initialize( const kvs::Vector3f& center );

public:

    const int number( void ) const;

    const kvs::OneSegment& segment( const size_t type ) const;

public:

    void setNumber( const int number );

};

}

#endif
