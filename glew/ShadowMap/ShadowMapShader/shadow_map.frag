//
//  shadow_map.frag
//  
//
//  Created by Jun Nishimura on 11/9/10.
//  Copyright 2011 Jun Nishimura. All rights reserved.
//

varying vec3 position;
varying vec3 normal;
uniform sampler2D shadow;

uniform int level;
uniform float strength;
uniform vec2 texture_size_inv;

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
    vec3 frag_color = gl_Color.xyz;
    //vec3 frag_color = vec3( 1.0 );

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

#else
    vec3 shaded_color = ShadingNone( shading, frag_color );
#endif

    float shadow_color = 1.0;
    vec4 shadow_texcoord = gl_TexCoord[0] / gl_TexCoord[0].w;

    int nsamples = ( 2 * level + 1 ) * ( 2 * level + 1 );
    float kernel = strength / float(nsamples);
    float dx = texture_size_inv.x;
    float dy = texture_size_inv.y;
    for ( int x = - level; x <= level; x++ )
    {
        for ( int y = - level; y <= level; y++ )
        {
            vec2 offset = vec2( float(x) * dx, float(y) * dy );
            float depth = texture2D( shadow, shadow_texcoord.xy + offset ).x;
            if ( shadow_texcoord.w > 0.0 && depth < shadow_texcoord.z - 0.001 )
            {
                shadow_color -= kernel;
            }
        }
    }

    vec3 color = shaded_color * shadow_color;

    gl_FragColor = vec4( color, 1.0 );
}

