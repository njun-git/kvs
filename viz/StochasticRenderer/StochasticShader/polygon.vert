/*****************************************************************************/
/**
 *  @file   zooming.vert
 *  @author Naohisa Sakamoto
 */
/*----------------------------------------------------------------------------
 *
 *  Copyright 2007 Visualization Laboratory, Kyoto University.
 *  All rights reserved.
 *  See http://www.viz.media.kyoto-u.ac.jp/kvs/copyright/ for details.
 *
 *  $Id: zooming.vert 627 2010-10-03 13:56:57Z naohisa.sakamoto $
 */
/*****************************************************************************/

varying vec3 position;
varying vec3 normal;
varying vec2 id;

attribute vec2 identifier;

uniform float polygon_offset;

/*===========================================================================*/
/**
 *  @brief  Calculates a size of the particle in pixel.
 */
/*===========================================================================*/
void main( void )
{
    gl_Position = ftransform();
    gl_Position.z -= polygon_offset;
    gl_FrontColor = gl_Color;

    position = ( gl_ModelViewMatrix * gl_Vertex ).xyz;
    normal = ( gl_NormalMatrix * gl_Normal ).xyz;
    id = identifier;
}
