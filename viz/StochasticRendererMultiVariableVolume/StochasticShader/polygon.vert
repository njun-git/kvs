//
//  polygon.vert
//  
//
//  Created by Jun Nishimura on 11/4/10.
//  Copyright 2011 Jun Nishimura. All rights reserved.
//


varying vec3 position;
varying vec3 normal;
varying vec2 id;

attribute vec2 identifier;

uniform float polygon_offset;

void main( void )
{
    gl_Position = ftransform();
    gl_Position.z -= polygon_offset;
    gl_FrontColor = gl_Color;

    position = ( gl_ModelViewMatrix * gl_Vertex ).xyz;
    normal = ( gl_NormalMatrix * gl_Normal ).xyz;
    id = identifier;
}
