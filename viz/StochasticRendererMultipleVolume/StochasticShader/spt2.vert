//
//  spt.vert
//  
//
//  Created by Jun Nishimura on 11/4/10.
//  Copyright 2011 Jun Nishimura. All rights reserved.
//

varying vec4 position_in;
varying vec2 id_in;
varying float value_in;
varying vec3 normal_in;

attribute vec2 identifier;
attribute float value;

void main( void )
{
    gl_Position = ftransform();

    position_in = gl_ModelViewMatrix * gl_Vertex;
    id_in = identifier;
    value_in = value;
    normal_in = gl_NormalMatrix * gl_Normal;
}
