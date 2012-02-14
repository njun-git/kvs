//
//  point.frag
//  
//
//  Created by Jun Nishimura on 11/4/10.
//  Copyright 2011 Jun Nishimura. All rights reserved.
//

uniform float densityFactor;
uniform int circle_threshold;
uniform vec2 screen_scale;
uniform sampler2D random_texture;
uniform float random_texture_size_inv;
uniform vec2 random_offset;
attribute vec2 identifier;

const float CIRCLE_SCALE = 0.564189583547756; // 1.0 / sqrt(PI)

varying vec3  position;
varying vec3  normal;
varying vec2  center;
varying float radius;


float zooming( in vec4 p )
{
    // Depth value.
    float D = p.z;
    if ( D < 1.0 ) D = 1.0; // to avoid front-clip

    // Calculate the footprint size of the particle.
    float s = densityFactor / D; // footprint size of the particle in pixel
    float sf = floor( s );       // round-down value of s
    float sc = ceil( s );        // round-up value of s

    // Calculate a probability 'pc' that the footprint size is 'sc'.
    float fraction = fract( s );
    float pc = fraction * ( 2.0 * sf + fraction ) / ( 2.0 * sf + 1.0 );

    // Random number from the random number texture.
    vec2 random_texture_index = identifier * random_texture_size_inv;
    float R = texture2D( random_texture, random_texture_index ).x;

    if ( circle_threshold <= 0 || s <= float( circle_threshold ) )
    {
        // Draw the particle as square.
        s = ( ( R < pc ) ? sc : sf );
        radius = 0.0;
    }
    else
    {
        // Draw the particle as circle.
        // Convert position to screen coordinates.
        center = screen_scale + ( ( p.xy / p.w ) * screen_scale );
        radius = ( ( R < pc ) ? sc : sf ) * CIRCLE_SCALE;
        s = ceil( s * CIRCLE_SCALE * 2.0 ) + 1.0;
    }

    return( s );
}

void main( void )
{
    gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
    gl_FrontColor = gl_Color;

    position = vec3( gl_ModelViewMatrix * gl_Vertex );
    normal = ( gl_NormalMatrix * gl_Normal ).xyz;

    gl_PointSize = zooming( gl_Position );
}
