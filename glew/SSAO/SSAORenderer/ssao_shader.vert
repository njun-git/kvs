//
//  ssao_shader.vert
//  
//
//  Created by Jun Nishimura on 11/22/9.
//  Copyright 2011 Jun Nishimura. All rights reserved.
//

void main( void )
{
    gl_Position = ftransform();
    gl_TexCoord[0] = gl_MultiTexCoord0;
}
