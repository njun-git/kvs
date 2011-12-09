//
//  spt.frag
//  
//
//  Created by Jun Nishimura on 11/4/10.
//  Copyright 2011 Jun Nishimura. All rights reserved.
//

varying vec3 position;
varying vec3 normal;
varying vec2 id;

varying float depth_front;
varying float depth_back;

varying float scalar_front;
varying float scalar_back;
varying float distance;

uniform sampler3D preintegration_texture;
uniform sampler2D random_texture;
uniform sampler2D depth_texture;

uniform vec2 screen_scale;
uniform vec2 screen_scale_inv;
uniform vec2 preintegration_scale_offset;

uniform float random_texture_size_inv;
uniform vec2 random_offset;

struct Shading
{
    float Ka; // ambient
    float Kd; // diffuse
    float Ks; // specular
    float S;  // shininess
};

vec3 ShadingNone( in Shading shading, in vec3 color )
{
    return( color );
}

vec3 ShadingLambert( in Shading shading, in vec3 color, in vec3 L, in vec3 N )
{
//    float dd = max( dot( N, L ), 0.0 );
    float dd = abs( dot( N, L ) );

    float Ia = shading.Ka;
    float Id = shading.Kd * dd;

    return( color * ( Ia + Id ) );
}

vec3 ShadingPhong( in Shading shading, in vec3 color, in vec3 L, in vec3 N, in vec3 V )
{
    vec3 R = reflect( -L, N );
//    float dd = max( dot( N, L ), 0.0 );
    float dd = abs( dot( N, L ) );
//    float ds = pow( max( dot( R, V ), 0.0 ), shading.S );
    float ds = pow( abs( dot( R, V ) ), shading.S );
    if ( dd <= 0.0 ) ds = 0.0;

    float Ia = shading.Ka;
    float Id = shading.Kd * dd;
    float Is = shading.Ks * ds;

    return( color * ( Ia + Id ) + Is );
}

vec3 ShadingBlinnPhong( in Shading shading, in vec3 color, in vec3 L, in vec3 N, in vec3 V )
{
    vec3 H = normalize( L + V );
//    float dd = max( dot( N, L ), 0.0 );
    float dd = abs( dot( N, L ) );
//    float ds = pow( max( dot( H, N ), 0.0 ), shading.S );
    float ds = pow( abs( dot( H, N ) ), shading.S );
    if ( dd <= 0.0 ) ds = 0.0;

    float Ia = shading.Ka;
    float Id = shading.Kd * dd;
    float Is = shading.Ks * ds;

    return( color * ( Ia + Id ) + Is );
}

uniform Shading shading;

float distance_to_depth( const in float distance )
{
    return( ( distance - preintegration_scale_offset.y ) / preintegration_scale_offset.x );
}

float distance_to_texture_coord( const in float distance )
{
    return( distance * preintegration_scale_offset.x + preintegration_scale_offset.y );
}

void main( void )
{
    vec4 lutdata;

    vec2 screen_coord = gl_FragCoord.xy * screen_scale_inv;
    float geom_d = texture2D( depth_texture, screen_coord ).x;

    if ( geom_d < 1.0 &&
         depth_front <= geom_d &&
         geom_d <= depth_back )
    {
        float ratio = ( geom_d - depth_front ) / ( depth_back - depth_front );
        vec3 lutcoord = vec3( scalar_front, ( 1.0 - ratio ) * scalar_front + ratio * scalar_back, ratio * distance );
        lutdata = texture3D( preintegration_texture, lutcoord );
    }
    else
    {
        vec3 lutcoord = vec3( scalar_front, scalar_back, distance );
        lutdata = texture3D( preintegration_texture, lutcoord );
    }

    if ( lutdata.a == 0.0 ) { discard; return; }

    vec2 random_position = ( vec2( float( int( id.x ) * 73 ), float( int( id.y ) * 31 ) ) 
                + random_offset + gl_FragCoord.xy ) * random_texture_size_inv;

    float randf = texture2D( random_texture, random_position ).x;
    if ( randf > lutdata.a ) { discard; return; }

    vec3 frag_color = lutdata.xyz / lutdata.a;

    // Light position.
    vec3 light_position = gl_LightSource[0].position.xyz;

    // Light vector (L) and Normal vector (N)
    vec3 L = normalize( light_position - position );
    vec3 N = normalize( normal );

#if   defined( ENABLE_LAMBERT_SHADING )
    vec3 shaded_color = ShadingLambert( shading, frag_color, L, N );

#elif defined( ENABLE_PHONG_SHADING )
    vec3 V = normalize( - position );
    vec3 shaded_color = ShadingPhong( shading, frag_color, L, N, V );

#elif defined( ENABLE_BLINN_PHONG_SHADING )
    vec3 V = normalize( - position );
    vec3 shaded_color = ShadingBlinnPhong( shading, frag_color, L, N, V );

#else // DISABLE SHADING
    vec3 shaded_color = ShadingNone( shading, frag_color );
#endif

    gl_FragColor = vec4( shaded_color, 1.0 );
    gl_FragDepth = depth_front;
}
