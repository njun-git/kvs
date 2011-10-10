/*
 *  PaintImageRenderer.cpp
 *  
 *
 *  Created by njun on 11/10/10.
 *  Copyright 2011 Jun Nishimura. All rights reserved.
 *
 */

#include "PaintImageRenderer.h"

namespace kvs
{

PaintImageRenderer::PaintImageRenderer( void )
{
}

PaintImageRenderer::~PaintImageRenderer( void )
{
}

void PaintImageRenderer::exec( kvs::ObjectBase* object, kvs::Camera* camera, kvs::Light* light )
{
    kvs::PaintImageObject* image = reinterpret_cast<kvs::PaintImageObject*>( object );

    glPushAttrib( GL_CURRENT_BIT | GL_ENABLE_BIT );
    glEnable( GL_TEXTURE_2D );

    if ( !image->isInitilize() ) image->create();
    image->bind();
    image->download();

    glMatrixMode( GL_MODELVIEW );
    glPushMatrix();
    {
        glLoadIdentity();

        glMatrixMode( GL_PROJECTION );
        glPushMatrix();
        {
            glLoadIdentity();
            glOrtho( 0, 1, 0, 1, -1, 1 );

            glBegin( GL_QUADS );
            glTexCoord2f( 0.0, 0.0 ); glVertex2f(  0.0,  1.0 );
            glTexCoord2f( 0.0, 1.0 ); glVertex2f(  0.0,  0.0 );
            glTexCoord2f( 1.0, 1.0 ); glVertex2f(  1.0,  0.0 );
            glTexCoord2f( 1.0, 0.0 ); glVertex2f(  1.0,  1.0 );
            glEnd();
        }
        glPopMatrix();
        glMatrixMode( GL_MODELVIEW );
    }
    glPopMatrix();

    image->unbind();

    glDisable( GL_TEXTURE_2D );
    glPopAttrib();
}

}
