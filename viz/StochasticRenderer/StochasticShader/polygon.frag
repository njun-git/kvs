/*****************************************************************************/
/**
 *  @file   zooming.frag
 *  @author Naohisa Sakamoto
 */
/*----------------------------------------------------------------------------
 *
 *  Copyright 2007 Visualization Laboratory, Kyoto University.
 *  All rights reserved.
 *  See http://www.viz.media.kyoto-u.ac.jp/kvs/copyright/ for details.
 *
 *  $Id: zooming.frag 490 2010-03-22 12:00:41Z naohisa.sakamoto $
 */
/*****************************************************************************/

varying vec3 position;
varying vec3 normal;
varying vec2 id;

uniform sampler2D random_texture;

uniform vec2 screen_scale;
uniform vec2 screen_scale_inv;

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

/*===========================================================================*/
/**
 *  @brief  Calculates a shaded color of the particle with Lambert shading.
 */
/*===========================================================================*/
void main( void )
{
    if ( gl_Color.a == 0.0 ) { discard; return; }

    if ( gl_Color.a < 0.99999 )
    {
        vec2 random_position = ( vec2( float( int( id.x ) * 73 ), float( int( id.y ) * 31 ) ) 
                    + random_offset + gl_FragCoord.xy ) * random_texture_size_inv;

        float randf = texture2D( random_texture, random_position ).a;
        if ( randf > gl_Color.a ) { discard; return; }
    }

    vec3 frag_color = gl_Color.rgb;

    // Light position.
    vec3 light_position = gl_LightSource[0].position.xyz;

    // Light vector (L) and Normal vector (N)
    vec3 L = normalize( light_position - position );
    vec3 N = normalize( normal );

#if   defined( ENABLE_LAMBERT_SHADING )
    vec3 shaded_color = ShadingLambert( shading, frag_color, L, N );

#elif defined( ENABLE_PHONG_SHADING )
    vec3 V = normalize( -position );
    vec3 shaded_color = ShadingPhong( shading, frag_color, L, N, V );

#elif defined( ENABLE_BLINN_PHONG_SHADING )
    vec3 V = normalize( -position );
    vec3 shaded_color = ShadingBlinnPhong( shading, frag_color, L, N, V );

#else // DISABLE SHADING
    vec3 shaded_color = ShadingNone( shading, frag_color );
#endif

    gl_FragColor = vec4( shaded_color, 1.0 );
}
