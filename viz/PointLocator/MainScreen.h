/*
 *  MainScreen.h
 *  
 *
 *  Created by njun on 11/10/11.
 *  Copyright 2011 Jun Nishimura. All rights reserved.
 *
 */

#ifndef KVS__MAIN_SCREEN_H_INCLUDE
#define KVS__MAIN_SCREEN_H_INCLUDE

#include <kvs/glut/Application>
#include <kvs/glut/Screen>

#include <kvs/StructuredVolumeImporter>
#include <kvs/Streamline>
#include <kvs/TornadoVolumeData>

namespace kvs
{

class MainScreen : public kvs::glut::Screen
{

    // Class name.
    kvsClassName( kvs::MainScreen );

    // Module information.
    kvsModuleBaseClass( kvs::glut::Screen );

protected:

    kvs::StructuredVolumeObject*    m_volume;
    kvs::PointObject*               m_seed_point;
    kvs::LineObject*                m_streamline;
    kvs::TransferFunction           m_tfunc;

public:

    MainScreen( kvs::glut::Application* app );

    virtual ~MainScreen( void );

public:

    void initialize( kvs::glut::Application* app );

    void clear( void );

public:

    kvs::PointObject* seedPoint( void );

    void updateStreamLine( void );

};

}

#endif
