//
//  terrain.vert
//  
//
//  Created by Jun Nishimura on 11/22/9.
//  Copyright 2011 Jun Nishimura. All rights reserved.
//

varying vec4 position_in;

void main( void )
{
    gl_Position = ftransform();
    gl_FrontColor = gl_Color;

    position_in = gl_Vertex;
}
