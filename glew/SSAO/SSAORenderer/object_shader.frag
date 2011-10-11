//
//  object_shader.frag
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
    gl_FragData[0] = color;
    gl_FragData[1] = vec4( position.xyz, 1.0 );
    gl_FragData[2] = vec4( normal, 1.0 );
}
