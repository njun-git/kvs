//
//  OpenNI.h
//  
//
//  Created by Jun Nishimura on 10/18/11.
//  Copyright (c) 2011 Jun Nishimura. All rights reserved.
//

#ifndef KVS_SUPPORT_OPENNI_OPENNI_H_INCLUDE
#define KVS_SUPPORT_OPENNI_OPENNI_H_INCLUDE

#include <string>
#include <kvs/Platform>

#include <XnCppWrapper.h>

namespace kvs
{

namespace ni
{

inline const std::string Description( void )
{
    const std::string description( "OpenNI - Open Natural Interaction" );
    return( description );
}

inline const std::string Version( void )
{
    const std::string version( "Unknown" );
    return( version );
}

}

}


#endif
