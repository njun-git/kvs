//
//  shadow.vert
//  
//
//  Created by Jun Nishimura on 11/9/10.
//  Copyright 2011 Jun Nishimura. All rights reserved.
//

varying vec3 position;
varying vec3 normal;

void main( void )
{
    position = ( gl_ModelViewMatrix * gl_Vertex ).xyz;
    normal = normalize( gl_NormalMatrix * gl_Normal );
    gl_TexCoord[0] = gl_MultiTexCoord0;

    gl_Position = ftransform();
    gl_FrontColor = gl_Color;
}
