//
//  volume.geom
//  
//
//  Created by Jun Nishimura on 11/9/10.
//  Copyright 2011 Jun Nishimura. All rights reserved.
//

#version 120
#extension GL_EXT_geometry_shader4 : enable
#extension GL_EXT_gpu_shader4 : enable

varying in vec4 position_in[2];

void emit_new_vertex( const in vec4 new_position  )
{
    gl_Position = gl_ProjectionMatrix * new_position;
    EmitVertex();
}

void main( void )
{
    // Light position.
    vec3 light_position = gl_LightSource[0].position.xyz;

    vec3 v0l = normalize( position_in[0].xyz - light_position );
    vec3 v1l = normalize( position_in[1].xyz - light_position );

    emit_new_vertex( position_in[0] );
    emit_new_vertex( position_in[1] );
    emit_new_vertex( position_in[1] + vec4( 1000000.0 * v1l, position_in[1].w ) );
    emit_new_vertex( position_in[0] + vec4( 1000000.0 * v0l, position_in[0].w ) );
    EndPrimitive();
}

