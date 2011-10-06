/*
 *  Camera.cpp
 *  
 *
 *  Created by njun on 11/07/10.
 *  Copyright 2011 Jun Nishimura. All rights reserved.
 *
 */

#include "Camera.h"

#include "Application.h"
#include "Configuration.h"

#include <kvs/OpenGL>
#include <kvs/Matrix44>
#include <kvs/ViewingMatrix44>
#include <kvs/FrustumMatrix44>

namespace kvs
{

namespace tdw
{

Camera::Camera( const bool collision ) :
    kvs::Camera( collision )
{
    this->initialize();
    this->setTDWFrustum();
}

Camera::~Camera( void )
{
}

void Camera::setTDWFrustum( const float half_length )
{
    const float length = 0.1 * half_length;
    const kvs::Vector2i resolution = kvs::tdw::Configuration::FullResolution();
    m_window_width = resolution.x();
    m_window_height = resolution.y();

    const float aspect = static_cast<float>( m_window_width ) / m_window_height;
    if ( kvs::tdw::Application::IsMaster() )
    {
        if ( aspect >= 1.0f )
        {
            m_left = - length * aspect;
            m_right = length * aspect;
            m_bottom = - length;
            m_top = length;
        }
        else
        {
            m_left = - length;
            m_right = length;
            m_bottom = - length / aspect;
            m_top = length / aspect;
        }
    }
    else
    {
        const kvs::tdw::Renderer renderer = kvs::tdw::Configuration::Renderer();
        const kvs::Vector2i origin = renderer.origin();
        const kvs::Vector2i size = renderer.size();
        if ( aspect >= 1.0f )
        {
            const float ratio = 2.0f * length / m_window_height;
            m_left = - length * aspect + ratio * origin.x();
            m_right = m_left + ratio * size.x();
            m_top = length - ratio * origin.y();
            m_bottom = m_top - ratio * size.y();
        }
        else
        {
            const float ratio = 2.0f * length / m_window_width;
            m_left = - length + ratio * origin.x();
            m_right = m_left + ratio * size.x();
            m_top = length / aspect - ratio * origin.y();
            m_bottom = m_top - ratio * size.y();
        }
    }
}

void Camera::update( void )
{
    glMatrixMode( GL_PROJECTION );
    {
        glLoadIdentity();
        glFrustum( m_left, m_right, m_bottom, m_top, m_front, m_back );
    }

    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity();

    gluLookAt( m_position.x(),  m_position.y(),  m_position.z(),
               m_look_at.x(),   m_look_at.y(),   m_look_at.z(),
               m_up_vector.x(), m_up_vector.y(), m_up_vector.z() );
}

} // end of namespace tdw

} // end of namespace kvs
