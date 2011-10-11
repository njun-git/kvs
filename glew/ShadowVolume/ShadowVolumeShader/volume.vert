//
//  volume.vert
//  
//
//  Created by Jun Nishimura on 11/9/10.
//  Copyright 2011 Jun Nishimura. All rights reserved.
//

varying vec4 position_in;

void main( void )
{
    gl_Position = ftransform();
    position_in = gl_ModelViewMatrix * gl_Vertex;
}
