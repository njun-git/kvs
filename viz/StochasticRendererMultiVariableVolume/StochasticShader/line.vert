//
//  line.vert
//  
//
//  Created by Jun Nishimura on 11/4/10.
//  Copyright 2011 Jun Nishimura. All rights reserved.
//

varying vec3 position;
varying vec2 id;

attribute vec2 identifier;

void main( void )
{
    gl_Position = ftransform();
    gl_FrontColor = gl_Color;

    position = ( gl_ModelViewMatrix * gl_Vertex ).xyz;
    id = identifier;
}
