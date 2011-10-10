//
//  terrain.geom
//  
//
//  Created by Jun Nishimura on 11/22/9.
//  Copyright 2011 Jun Nishimura. All rights reserved.
//

#version 120
#extension GL_EXT_geometry_shader4 : enable
#extension GL_EXT_gpu_shader4 : enable

varying in vec4 position_in[3];

varying out vec3 position;
varying out vec3 normal;

uniform sampler2D texture;
uniform vec2 texture_size_inv;

void emit_new_vertex(
    const in vec4 new_position,
    const in vec3 new_normal,
    const in vec4 new_color )
{
    gl_Position = gl_ModelViewProjectionMatrix * new_position;
    position = ( gl_ModelViewMatrix * new_position ).xyz;
    normal = gl_NormalMatrix * new_normal;
    gl_FrontColor = new_color;
    EmitVertex();
}

void main( void )
{
    vec4 new_vertex[3];
    int level = 0;
    float kernel = 1.0 / float( ( 2 * level + 1 ) * ( 2 * level + 1 ) );

    for ( int i = 0; i < 3; i++ )
    {
        new_vertex[i] = position_in[i];
        vec2 texcoord = position_in[i].xy;
        texcoord.y = 1.0 - texcoord.y;
        for ( int x = - level; x <= level; x++ )
        {
            for ( int y = - level; y <= level; y++ )
            {
                new_vertex[i].z += kernel * texture2D( texture, texcoord + 2.5 * texture_size_inv * vec2( float(x), float(y) ) ).x;
            }
        }
    }

    vec3 v10 = ( new_vertex[1] - new_vertex[0] ).xyz;
    vec3 v20 = ( new_vertex[2] - new_vertex[0] ).xyz;
    vec3 n = normalize( cross( v10, v20 ) );

    for ( int i = 0; i < 3; i++ )
    {
        emit_new_vertex(
            new_vertex[i],
            n,
            gl_FrontColorIn[i] );
    }
    EndPrimitive();
}
