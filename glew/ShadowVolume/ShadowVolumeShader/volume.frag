//
//  volume.frag
//  
//
//  Created by Jun Nishimura on 11/9/10.
//  Copyright 2011 Jun Nishimura. All rights reserved.
//

void main( void )
{
    gl_FragColor = vec4( 0.0, 0.0, 0.0, 1.0 );
    gl_FragDepth = gl_FragCoord.z;
}

