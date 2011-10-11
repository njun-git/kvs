//
//  object_shader.vert
//  
//
//  Created by Jun Nishimura on 11/22/9.
//  Copyright 2011 Jun Nishimura. All rights reserved.
//

varying vec4 color;
varying vec4 position;
varying vec3 normal;

void main( void )
{
    position = gl_ModelViewMatrix * gl_Vertex;
    normal = normalize( gl_NormalMatrix * gl_Normal );
    color = gl_Color;

    gl_Position = ftransform();
}
