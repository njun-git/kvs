//
//  point.frag
//  
//
//  Created by Jun Nishimura on 11/4/10.
//  Copyright 2011 Jun Nishimura. All rights reserved.
//

varying vec3  position;
varying vec3  normal;
varying vec2  center;
varying float radius;

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
    // Discard a pixel outside circle.
    if ( radius > 0.0 )
    {
        if( distance( gl_FragCoord.xy, center ) > radius ) discard;
    }

    // Light position.
    vec3 light_position = gl_LightSource[0].position.xyz;

    // Light vector (L) and Normal vector (N)
    vec3 L = normalize( light_position - position );
    vec3 N = normalize( normal );

#if   defined( ENABLE_LAMBERT_SHADING )
    vec3 shaded_color = ShadingLambert( shading, gl_Color.xyz, L, N );

#elif defined( ENABLE_PHONG_SHADING )
    vec3 V = normalize( -position );
    vec3 shaded_color = ShadingPhong( shading, gl_Color.xyz, L, N, V );

#elif defined( ENABLE_BLINN_PHONG_SHADING )
    vec3 V = normalize( -position );
    vec3 shaded_color = ShadingBlinnPhong( shading, gl_Color.xyz, L, N, V );

#else // DISABLE SHADING
    vec3 shaded_color = ShadingNone( shading, gl_Color.xyz );
#endif

    gl_FragColor.xyz = shaded_color;
    gl_FragColor.w = 1.0;
}
