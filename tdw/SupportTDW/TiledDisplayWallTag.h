/*
 *  TiledDisplayWallTag.h
 *  
 *
 *  Created by njun on 11/07/09.
 *  Copyright 2011 Jun Nishimura. All rights reserved.
 *
 */

#ifndef KVS__KVSML__TILED_DISPLAY_WALL_TAG_H_INCLUDE
#define KVS__KVSML__TILED_DISPLAY_WALL_TAG_H_INCLUDE

#include <kvs/XMLNode>
#include <Core/FileFormat/KVSML/TagBase.h>
#include <kvs/Vector2>

namespace kvs
{

namespace kvsml
{

class TiledDisplayWallTag : public kvs::kvsml::TagBase
{
public:

    typedef kvs::kvsml::TagBase BaseClass;

protected:

    bool m_has_nrenderers;
    int m_nrenderers;

    bool m_has_full_resolution;
    kvs::Vector2i m_full_resolution;

    bool m_is_fullscreen;
    bool m_is_sync;

public:

    TiledDisplayWallTag( void );

    virtual ~TiledDisplayWallTag( void );

public:

    const bool hasNRenderers( void ) const;

    const int nrenderers( void ) const;

    const bool hasFullResolution( void ) const;

    const kvs::Vector2i fullResolution( void ) const;

    const bool isFullscreen( void ) const;

    const bool isSync( void ) const;

public:

    void setNRenderers( const int nrenderers );

    void setFullResolution( const kvs::Vector2i& full_resolution );

    void setFullscreen( const bool fullscreen );

    void setSync( const bool sync );

public:

    const bool read( const kvs::XMLNode::SuperClass* parent );

    const bool write( kvs::XMLNode::SuperClass* parent );
};

} // end of namespace kvsml

} // end of namespace kvs

#endif
