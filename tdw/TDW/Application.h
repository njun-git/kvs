/*
 *  Application.h
 *  
 *
 *  Created by njun on 11/07/10.
 *  Copyright 2011 Jun Nishimura. All rights reserved.
 *
 */

#ifndef KVS__TDW__APPLICATION_H_INCLUDE
#define KVS__TDW__APPLICATION_H_INCLUDE

#include <kvs/ApplicationBase>
#include "Configuration.h"

namespace kvs
{

namespace tdw
{

class Application : public kvs::ApplicationBase
{

protected:

    kvs::tdw::Configuration*    m_configuration;
    static bool                 m_is_master;

public:

    Application( int argc, char** argv );

    virtual ~Application( void );

public:

    static bool IsMaster( void );

    static bool IsRenderer( void );

public:

    virtual int run( void );

    virtual void quit( void );

};

} // end of namespace tdw

} // end of namespace kvs

#endif
