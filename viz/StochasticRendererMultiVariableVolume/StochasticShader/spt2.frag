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

varying vec2 scalar_front;
varying vec2 scalar_back;
varying float distance;

uniform sampler3D preintegration_texture0;
uniform sampler3D preintegration_texture1;
uniform sampler2D random_texture;

uniform vec2 screen_scale;
uniform vec2 screen_scale_inv;
uniform vec2 preintegration_scale_offset;

uniform float random_texture_size_inv;
uniform vec2 random_offset;

uniform int start_volume;
uniform int end_volume;

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

void main( void )
{
    int accept = 0;
    vec3 accum_color = vec3( 0.0, 0.0, 0.0 );

    for ( int i = start_volume; i < end_volume; i++ )
    {
        vec4 lutdata;
        if ( i == 0 )
        {
            vec3 lutcoord = vec3( scalar_front.x, scalar_back.x, distance );
            lutdata = texture3D( preintegration_texture0, lutcoord );
        }
        else
        {
            vec3 lutcoord = vec3( scalar_front.y, scalar_back.y, distance );
            lutdata = texture3D( preintegration_texture1, lutcoord );
        }

        if ( lutdata.a == 0.0 ) continue;

        vec2 random_position = ( vec2( float( int( id.x ) * 73 ), float( int( id.y ) * 31 ) ) 
                    + ( 1.0 + float(i) * 2.6 ) * random_offset + gl_FragCoord.xy ) * random_texture_size_inv;

        float randf = texture2D( random_texture, random_position ).x;
        if ( randf > lutdata.a ) continue;

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

        accept += 1;
        accum_color += shaded_color;
    }

    if ( accept == 0 ) { discard; return; }
    else if ( accept == 1 )
    {
        gl_FragColor = vec4( accum_color, 1.0 );
    }
    else
    {
        gl_FragColor = vec4( 0.5 * accum_color, 1.0 );
    }
}
