/*
 *  MainScreen.cpp
 *  
 *
 *  Created by njun on 11/10/11.
 *  Copyright 2011 Jun Nishimura. All rights reserved.
 *
 */

#include "MainScreen.h"

#include <kvs/PointRenderer>
#include <kvs/Bounds>

namespace kvs
{

MainScreen::MainScreen( kvs::glut::Application* app ) :
    kvs::glut::Screen( app )
{
    this->initialize( app );
}

MainScreen::~MainScreen( void )
{
    this->clear();
}

void MainScreen::initialize( kvs::glut::Application* app )
{
    BaseClass::setTitle( "kvs::MainScreen" );
    BaseClass::setGeometry( 0, 0, 512, 512 );

    int argc = app->argc();
    char** argv = app->argv();
    if ( argc > 1 ) m_volume = new kvs::StructuredVolumeImporter( std::string( argv[1] ) );
    else            m_volume = new kvs::TornadoVolumeData( kvs::Vector3ui( 32, 32, 32 ) );
    if ( !m_volume )
    {
        kvsMessageError( "Cannot create a structured volume object." );
        exit( EXIT_FAILURE );
    }

    const kvs::Vector3f center = m_volume->objectCenter();
    kvs::ValueArray<float> coords( 3 );
    coords[0] = center.x();
    coords[1] = center.y();
    coords[2] = center.z();

    m_seed_point = new kvs::PointObject( coords );
    if ( !m_seed_point )
    {
        kvsMessageError( "Cannot creat a point object." );
        delete m_volume;
        exit( EXIT_FAILURE );
    }
    m_seed_point->setColor( kvs::RGBColor( 255, 0, 0 ) );
    m_seed_point->setSize( 10.0f );
    m_seed_point->setMinMaxObjectCoords( m_volume->minObjectCoord(), m_volume->maxObjectCoord() );
    m_seed_point->setMinMaxExternalCoords( m_volume->minExternalCoord(), m_volume->maxExternalCoord() );

    kvs::PointRenderer* renderer = new kvs::PointRenderer();
    renderer->enableAntiAliasing();

    BaseClass::registerObject( m_seed_point, renderer );

    m_streamline = new kvs::Streamline( m_volume, m_seed_point, m_tfunc );
    if ( !m_streamline )
    {
        kvsMessageError( "Cannot creat a streamline object." );
        delete m_volume;
        delete m_seed_point;
        exit( EXIT_FAILURE );
    }
    m_streamline->setName( "Streamline" );

    BaseClass::registerObject( m_streamline );

    kvs::LineObject* bounds = new kvs::Bounds( m_volume );
    BaseClass::registerObject( bounds );
}

void MainScreen::clear( void )
{
    if ( m_volume ) delete m_volume;
}

kvs::PointObject* MainScreen::seedPoint( void )
{
    return( m_seed_point );
}

void MainScreen::updateStreamLine( void )
{
    m_streamline = new kvs::Streamline( m_volume, m_seed_point, m_tfunc );
    if ( !m_streamline )
    {
        kvsMessageError( "Cannot creat a streamline object." );
        delete m_volume;
        delete m_seed_point;
        exit( EXIT_FAILURE );
    }
    m_streamline->setName( "Streamline" );

    BaseClass::objectManager()->change( "Streamline", m_streamline );
}

}
