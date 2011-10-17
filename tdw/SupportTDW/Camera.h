/*
 *  Camera.h
 *  
 *
 *  Created by njun on 11/07/10.
 *  Copyright 2011 Jun Nishimura. All rights reserved.
 *
 */

#ifndef KVS__TDW__CAMERA_H_INCLUDE
#define KVS__TDW__CAMERA_H_INCLUDE

#include <kvs/Camera>

namespace kvs
{

namespace tdw
{

class Camera : public kvs::Camera
{

public:

    Camera( const bool collision = true );

    virtual ~Camera( void );

public:

    void setTDWFrustum( const float half_length = 5.0f );

public:

    virtual void update( void );
};

} // end of namespace tdw

} // end of namespace kvs

#endif
