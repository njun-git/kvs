/*
 *  Application.cpp
 *  
 *
 *  Created by njun on 11/07/10.
 *  Copyright 2011 Jun Nishimura. All rights reserved.
 *
 */

#include "Application.h"
#include <kvs/glut/GLUT>

namespace kvs
{

namespace tdw
{

bool Application::m_is_master;

Application::Application( int argc, char** argv ) :
    kvs::ApplicationBase( argc, argv ),
    m_configuration( NULL )
{
    m_configuration = new kvs::tdw::Configuration();
    const char* name = getenv( "KVS_TDW_RENDERER" );
    m_is_master = ( !name );

    static bool flag = true;
    if ( flag )
    {
        glutInit( &argc, argv );
        flag = false;
    }
}

Application::~Application( void )
{
    if ( m_configuration ) delete m_configuration;
}

bool Application::IsMaster( void )
{
    return( m_is_master );
}

bool Application::IsRenderer( void )
{
    return( !m_is_master );
}

int Application::run( void )
{
    static bool flag = true;
    if ( flag )
    {
        flag = false;
        glutMainLoop();
    }

    return( true );
}

void Application::quit( void )
{
    exit(0);
}

} // end of namespace tdw

} // end of namespace kvs
