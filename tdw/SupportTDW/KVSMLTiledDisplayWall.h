/*
 *  KVSMLTiledDisplayWall.h
 *  
 *
 *  Created by njun on 11/07/09.
 *  Copyright 2011 Jun Nishimura. All rights reserved.
 *
 */

#ifndef KVS__KVSML_TILED_DISPLAY_WALL_H_INCLUDE
#define KVS__KVSML_TILED_DISPLAY_WALL_H_INCLUDE

#include <kvs/FileFormatBase>
#include <string>
#include <vector>
#include <Core/FileFormat/KVSML/KVSMLTag.h>
#include "Master.h"
#include "Renderer.h"

namespace kvs
{

class KVSMLTiledDisplayWall : public kvs::FileFormatBase
{
    kvsClassName( kvs::KVSMLTiledDisplayWall );

protected:

    kvs::kvsml::KVSMLTag            m_kvsml_tag;
    size_t                          m_nrenderers;
    kvs::tdw::Master                m_master;
    std::vector<kvs::tdw::Renderer> m_renderers;
    bool                            m_is_fullscreen;

public:

    KVSMLTiledDisplayWall( void );

    KVSMLTiledDisplayWall( const std::string& filename );

    virtual ~KVSMLTiledDisplayWall( void );

public:

    const size_t nrenderers( void ) const;

    const bool isFullscreen( void ) const;

    const kvs::tdw::Master& master( void ) const;

    const std::vector<kvs::tdw::Renderer>& rendererList( void ) const;

public:

    void setFilename( const std::string& filename );

public:

    const bool read( const std::string& filename );

    const bool write( const std::string& filename );

public:

    static const bool CheckFileExtension( const std::string& filename );

    static const bool CheckFileFormat( const std::string& filename );

    friend std::ostream& operator << ( std::ostream& os, const KVSMLTiledDisplayWall& rhs );

};

} // end of namespace kvs

#endif
