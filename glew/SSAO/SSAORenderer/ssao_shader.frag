//
//  ssao_shader.frag
//  
//
//  Created by Jun Nishimura on 11/22/9.
//  Copyright 2011 Jun Nishimura. All rights reserved.
//

uniform sampler2D color_texture;
uniform sampler2D position_texture;
uniform sampler2D normal_texture;
uniform sampler2D depth_texture;
uniform mat4 projection;

vec3 ShadingPhong( in vec3 color, in vec3 L, in vec3 N, in vec3 V )
{
    vec3 R = reflect( - L, N );
    float dd = max( dot( N, L ), 0.0 );
    //float dd = abs( dot( N, L ) );
    float ds = pow( max( dot( R, V ), 0.0 ), 100.0 );
    if ( dd <= 0.0 ) ds = 0.0;

    float Ia = 0.4;
    float Id = 0.5 * dd;
    float Is = 0.6 * ds;

    return( color * ( Ia + Id ) + Is );
}

vec4 RGBtoYCrCb( in vec4 rgba )
{
    mat4 matrix = mat4(
          0.299,   0.587,   0.114, 0.0,
          0.500, - 0.419, - 0.081, 0.0,
        - 0.169, - 0.332,   0.500, 0.0,
          0.000,   0.000,   0.000, 1.0 );
    return( clamp( rgba * matrix, 0.0, 1.0 ) );
}

vec4 YCrCbtoRGB( in vec4 ycrcb )
{
    mat4 matrix = mat4(
        1.0,   1.402,   0.000, 0.0,
        1.0, - 0.714, - 0.344, 0.0,
        1.0,   0.000, - 1.772, 0.0,
        0.0,   0.000,   0.000, 1.0 );
    return( clamp( ycrcb * matrix, 0.0, 1.0 ) );
}

void main( void )
{
    vec4 color = texture2D( color_texture, gl_TexCoord[0].st );
    if ( color.w == 0.0 )
    {
        gl_FragColor = vec4( color.xyz, 0.0 );
        return;
    }

    vec4 position = texture2D( position_texture, gl_TexCoord[0].st );
    vec3 normal = texture2D( normal_texture, gl_TexCoord[0].st ).xyz;
    vec3 light_position = gl_LightSource[0].position.xyz;

    // Light vector (L) and Normal vector (N)
    vec3 L = normalize( light_position - position.xyz );
    vec3 N = normalize( normal );
    //vec3 N = refract( vec3( 0.0, 0.0, 1.0 ), normal, 0.75 );
    vec3 V = normalize( - position.xyz );

    int count = 0;
    int resolution = 3;
    float length = 0.4 / float(resolution);
    for ( int x = - resolution; x <= resolution; x++ )
    {
        for ( int y = - resolution; y <= resolution; y++ )
        {
            for ( int z = - resolution; z <= resolution; z++ )
            {
                if ( x != 0 && y != 0 && z != 0 )
                {
                    vec4 q = projection * ( position + length * vec4( float(x), float(y), float(z), 0.0 ) );
                    q = q * 0.5 / q.w + 0.5;
                    if ( q.z < texture2D( depth_texture, q.xy ).z ) count++;
                }
            }
        }
    }
    int nsamples = ( 2 * resolution + 1 ) * ( 2 * resolution + 1 ) * ( 2 * resolution + 1 ) - 1;
    float alpha = clamp( float(count) * 3.0 / float(nsamples), 0.0, 1.0 );

    vec4 shaded_color = vec4( ShadingPhong( color.xyz * alpha, L, N, V ), 1.0 );
    gl_FragColor = shaded_color;
/*
    vec4 ycrcb = RGBtoYCrCb( shaded_color );
    ycrcb.x = sin( ycrcb.x * 1.57079633 );

    gl_FragColor = YCrCbtoRGB( ycrcb );
*/
}
