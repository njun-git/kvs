//
//  image_process.cl
//  
//
//  Created by Jun Nishimura on 10/16/11.
//  Copyright (c) 2011 Jun Nishimura. All rights reserved.
//

__constant sampler_t sampler =
    CLK_NORMALIZED_COORDS_FALSE |
    CLK_ADDRESS_CLAMP_TO_EDGE |
    CLK_FILTER_NEAREST;

__kernel void gray_scale(
    __read_only  image2d_t src_image,
    __write_only image2d_t dst_image )
{
    int id_x = get_global_id(0);
    int id_y = get_global_id(1);
    int2 coord = (int2)( id_x, id_y );

    float4 color = read_imagef( src_image, sampler, coord );

    float Y = 0.298912 * color.x + 0.586611 * color.y + 0.114478 * color.z;
    color.x = Y;
    color.y = Y;
    color.z = Y;

    write_imagef( dst_image, coord, color );
}

__kernel void sepia_tone(
    __read_only  image2d_t src_image,
    __write_only image2d_t dst_image )
{
    int id_x = get_global_id(0);
    int id_y = get_global_id(1);
    int2 coord = (int2)( id_x, id_y );

    float4 color = read_imagef( src_image, sampler, coord );

    float Y = 0.298912 * color.x + 0.586611 * color.y + 0.114478 * color.z;
    color.x = Y * 0.941;
    color.y = Y * 0.784;
    color.z = Y * 0.569;

    write_imagef( dst_image, coord, color );
}

__kernel void laplacian_filter(
    __read_only  image2d_t src_image,
    __write_only image2d_t dst_image )
{
    int id_x = get_global_id(0);
    int id_y = get_global_id(1);
    int2 coord = (int2)( id_x, id_y );

    float4 laplacian;
    {
        float4 color = read_imagef( src_image, sampler, coord );
        float Y = 0.298912 * color.x + 0.586611 * color.y + 0.114478 * color.z;
        laplacian = (float4)( - 4.0 * Y );
    }

    for ( int i = - 1; i <= 1; i += 2 )
    {
        int2 coordx = (int2)( i, 0 );
        float4 color = read_imagef( src_image, sampler, coord + coordx );
        float Y = 0.298912 * color.x + 0.586611 * color.y + 0.114478 * color.z;
        laplacian += (float4)(Y);
    }

    for ( int i = - 1; i <= 1; i += 2 )
    {
        int2 coordy = (int2)( 0, i );
        float4 color = read_imagef( src_image, sampler, coord + coordy );
        float Y = 0.298912 * color.x + 0.586611 * color.y + 0.114478 * color.z;
        laplacian += (float4)(Y);
    }

    laplacian.w = 1.0;

    write_imagef( dst_image, coord, laplacian );
}

__kernel void sobel_filter(
    __read_only  image2d_t src_image,
    __write_only image2d_t dst_image )
{
    int id_x = get_global_id(0);
    int id_y = get_global_id(1);
    int2 coord = (int2)( id_x, id_y );

    float4 sobel = 0.0;

    float horizontal = 0.0;
    for ( int i = - 1; i <= 1; i++ )
    {
        for ( int j = - 1; j <= 1; j += 2 )
        {
            int2 coordx = (int2)( j, i );
            float4 color = read_imagef( src_image, sampler, coord + coordx );
            float Y = 0.298912 * color.x + 0.586611 * color.y + 0.114478 * color.z;
            if ( i == 0 ) horizontal += 2.0 * j * Y;
            else          horizontal += j * Y;
        }
    }

    float vertical = 0.0;
    for ( int i = - 1; i <= 1; i++ )
    {
        for ( int j = - 1; j <= 1; j += 2 )
        {
            int2 coordy = (int2)( i, j );
            float4 color = read_imagef( src_image, sampler, coord + coordy );
            float Y = 0.298912 * color.x + 0.586611 * color.y + 0.114478 * color.z;
            if ( i == 0 ) vertical += 2.0 * j * Y;
            else          vertical += j * Y;
        }
    }

    sobel = (float4)( sqrt( horizontal * horizontal + vertical * vertical ) );
    sobel.w = 1.0;

    write_imagef( dst_image, coord, sobel );
}
