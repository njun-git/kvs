/*****************************************************************************/
/**
 *  @file   PreItegrationTable.cpp
 *  @author Naohisa Sakamoto
 */
/*----------------------------------------------------------------------------
 *
 *  Copyright 2007 Visualization Laboratory, Kyoto University.
 *  All rights reserved.
 *  See http://www.viz.media.kyoto-u.ac.jp/kvs/copyright/ for details.
 *
 *  $Id$
 */
/*----------------------------------------------------------------------------
 *
 * Acknowledgements:
 * Original source code was developped by University of Utah and can be
 * downloaded at http://havs.sourceforge.net/
 */
/*****************************************************************************/
#include "PreIntegrationTable.h"
#include <vector>


namespace kvs
{

namespace glew
{

/*===========================================================================*/
/**
 *  @brief  Constructs a new PreIntegrationTable class.
 */
/*===========================================================================*/
PreIntegrationTable::PreIntegrationTable( void ):
    m_transfer_function( NULL ),
    m_table( NULL )
{
    // Set default table size.
    this->setTableSize( 128, 128 );
}

/*===========================================================================*/
/**
 *  @brief  Constructs a new PreIntegrationTable class.
 *  @param  size_scalar [in] table size for the scalar axis
 *  @param  size_depth [in] table size for the depth axis
 */
/*===========================================================================*/
PreIntegrationTable::PreIntegrationTable( const int size_scalar, const int size_depth ):
    m_transfer_function( NULL ),
    m_table( NULL ),
    m_size_scalar( size_scalar ),
    m_size_depth( size_depth )
{
}

/*===========================================================================*/
/**
 *  @brief  Destructs the PreIntegrationTable class.
 */
/*===========================================================================*/
PreIntegrationTable::~PreIntegrationTable( void )
{
    if ( m_transfer_function ) delete [] m_transfer_function;
    if ( m_table ) delete [] m_table;
}

const int PreIntegrationTable::sizeDepth( void ) const
{
    return( m_size_depth );
}

/*===========================================================================*/
/**
 *  @brief  Sets a table size.
 *  @param  size_scalar [in] table size for the scalar axis
 *  @param  size_depth [in] table size for the depth axis
 */
/*===========================================================================*/
void PreIntegrationTable::setTableSize( const int size_scalar, const int size_depth )
{
    m_size_scalar = size_scalar;
    m_size_depth = size_depth;
}

/*===========================================================================*/
/**
 *  @brief  Sets a transfer function data.
 *  @param  transfer_function [in] transfer function
 *  @param  min_scalar [in] minimum scalar value
 *  @param  max_scalar [in] maximum scalar value
 */
/*===========================================================================*/
void PreIntegrationTable::setTransferFunction(
    const kvs::TransferFunction& transfer_function,
    const float min_scalar,
    const float max_scalar )
{
    if ( m_transfer_function )
    {
        delete [] m_transfer_function;
        m_transfer_function = NULL;
    }
    m_transfer_function = new kvs::Vector4f [ m_size_scalar ];

    const float range = max_scalar - min_scalar;
    std::vector<kvs::Vector4f> colors;
    std::vector<float> color_values;
    std::vector<float> opacities;
    std::vector<float> opacity_values;

    // Convert the KVS transfer function table to floating-point vectors of the color and opacity.
    const size_t resolution = transfer_function.resolution();
    const kvs::ColorMap& cmap = transfer_function.colorMap();
    const kvs::OpacityMap& omap = transfer_function.opacityMap();
    for ( size_t i = 0; i < resolution; i++ )
    {
        const kvs::RGBColor& c = cmap[i];
        const float r = static_cast<float>( c.r() ) / 255.0f;
        const float g = static_cast<float>( c.g() ) / 255.0f;
        const float b = static_cast<float>( c.b() ) / 255.0f;
        const kvs::Vector4f color( r, g, b, 1.0f );
        const float opacity = omap[i];
        const float scalar = float( i ) * range / float( resolution - 1 ) + min_scalar;

        colors.push_back( color );
        color_values.push_back( scalar );
        opacities.push_back( opacity );
        opacity_values.push_back( scalar );
    }

    const size_t begin = 0;
    const size_t end = resolution - 1;
    color_values[begin]   = min_scalar - range / 100.0f;
    color_values[end]     = max_scalar + range / 100.0f;
    opacity_values[begin] = min_scalar - range / 100.0f;
    opacity_values[end]   = max_scalar + range / 100.0f;

    // Create the transfer function by sampling color and opacity ranges.
    float r = min_scalar;
    const float dr = range / float( m_size_scalar - 1 );
    for ( size_t i = 0; i < static_cast<size_t>( m_size_scalar ); i++ )
    {
        // Sample color.
        {
            float prev = color_values[0];
            float next = prev;
            size_t index = 0;
            const size_t csize = color_values.size();
            for ( index = 1; index < csize; index++ )
            {
                next = color_values[index];
                if ( prev < r && next >= r ) break;
                prev = next;
            }

            const float interp_next = ( r - prev ) / ( next - prev );
            const float interp_prev = 1.0f - interp_next;
            const kvs::Vector4f next_color = colors[index];
            const kvs::Vector4f prev_color = colors[index-1];
            m_transfer_function[i][0] = next_color[0] * interp_next + prev_color[0] * interp_prev;
            m_transfer_function[i][1] = next_color[1] * interp_next + prev_color[1] * interp_prev;
            m_transfer_function[i][2] = next_color[2] * interp_next + prev_color[2] * interp_prev;
        }

        // Sample opacity.
        {
            float prev = opacity_values[0];
            float next = prev;
            size_t index = 0;
            const size_t osize = opacity_values.size();
            for ( index = 1; index < osize; index++ )
            {
                next = opacity_values[index];
                if ( prev < r && next >= r ) break;
                prev = next;
            }

            const float interp_next = ( r - prev ) / ( next - prev );
            const float interp_prev = 1.0f - interp_next;
            const float next_opacity = opacities[index];
            const float prev_opacity = opacities[index-1];
            m_transfer_function[i][3] = next_opacity * interp_next + prev_opacity * interp_prev;
        }

        r += dr;
    }
}

/*===========================================================================*/
/**
 *  @brief  Compute pre-integration table by numerical integration.
 *  @param  max_edge_length [in]
 */
/*===========================================================================*/
void PreIntegrationTable::create(
    const float max_edge_length )
{
    m_table_texture.setWrapS( GL_CLAMP_TO_EDGE );
    m_table_texture.setWrapT( GL_CLAMP_TO_EDGE );
    m_table_texture.setWrapR( GL_CLAMP_TO_EDGE );
    m_table_texture.setMagFilter( GL_LINEAR );
    m_table_texture.setMinFilter( GL_LINEAR );
    m_table_texture.setPixelFormat( GL_RGBA8, GL_RGBA, GL_FLOAT );

    if ( m_table )
    {
        delete [] m_table;
        m_table = NULL;
    }
    m_table = new kvs::Vector4f [ m_size_scalar * m_size_scalar * m_size_depth ];

    // Build base level of the table.
    for ( size_t i = 0, index = 0; i < static_cast<size_t>( m_size_scalar ); i++ )
    {
        for ( size_t j = 0; j < static_cast<size_t>( m_size_scalar ); j++, index++ )
        {
            m_table[index][0] = 0.0f;
            m_table[index][1] = 0.0f;
            m_table[index][2] = 0.0f;
            m_table[index][3] = 0.0f;
        }
    }

    // Build first level of the table.
    {
        const float dl = max_edge_length / float( m_size_depth - 1 );
        float l = dl;
        this->compute_exact_level( 1, l );
        for ( size_t i = 2; i < static_cast<size_t>( m_size_depth ); i++ )
        {
            l += dl;
            this->compute_incremental_level( i, i - 1, 1, l, dl );
        }

        // Create pre-integration texture region on GPU.
        m_table_texture.create( m_size_scalar, m_size_scalar, m_size_depth );
    }
}

/*===========================================================================*/
/**
 *  @brief  Downloads pre-integration table onto GPU.
 */
/*===========================================================================*/
void PreIntegrationTable::download( void )
{
    m_table_texture.download( m_size_scalar, m_size_scalar, m_size_depth, m_table );
    m_table_texture.unbind();
}

/*===========================================================================*/
/**
 *  @brief  Bind the pre-integration table.
 */
/*===========================================================================*/
void PreIntegrationTable::bind( void )
{
    m_table_texture.bind();
}

void PreIntegrationTable::unbind( void )
{
    m_table_texture.unbind();
}

void PreIntegrationTable::release( void )
{
    m_table_texture.release();
}

const bool PreIntegrationTable::isTexture( void )
{
    return( m_table_texture.isTexture() );
}

const bool PreIntegrationTable::isDownloaded( void )
{
    return( m_table_texture.isDownloaded() );
}

/*===========================================================================*/
/**
 *  @brief  Computes 2D pre-integration table by numerical integration.
 *  @param  level [in]
 *  @param  dl [in]
 */
/*===========================================================================*/
void PreIntegrationTable::compute_exact_level( const int level, const float dl )
{
    const int n = static_cast<int>( m_size_scalar );
    const int M = 32; // supersampling factor
    const int offset = level * m_size_scalar * m_size_scalar;
    for ( int i = 0; i < n; i++ )
    {
        for ( int j = 0; j < n; j++ )
        {
            if ( i == j ) // sb == sf
            {
                kvs::Vector4f c = m_transfer_function[i];
                c[3] = 1.0f - std::pow( 1.0f - c[3], dl );
                c[0] *= c[3];
                c[1] *= c[3];
                c[2] *= c[3];
                m_table[ offset + i * m_size_scalar + j ] = c;
            }

            else if ( i > j ) // sb > sf
            {
                kvs::Vector4f c( 0.0f, 0.0f, 0.0f, 0.0f );
                for ( int k = j; k < i; k++ )
                {
                    float beta = 0.0f;
                    const float dbeta = 1.0f / static_cast<float>( M );
                    const float dgamma = 1.0f / static_cast<float>( i - j );
                    kvs::Vector4f c0 = m_transfer_function[k];
                    c0[3] = 1.0f - std::pow( 1.0f - c0[3], dl * dbeta * dgamma );
                    c0[0] *= c0[3];
                    c0[1] *= c0[3];
                    c0[2] *= c0[3];
                    kvs::Vector4f c1 = m_transfer_function[k+1];
                    c1[3] = 1.0f - std::pow( 1.0f - c1[3], dl * dbeta * dgamma );
                    c1[0] *= c1[3];
                    c1[1] *= c1[3];
                    c1[2] *= c1[3];
                    for ( int m = 0; m < M; m++, beta += dbeta )
                    {
                        const kvs::Vector4f ck = c0 * ( 1.0f - beta ) + c1 * beta;
                        c += ck * ( 1.0f - c[3] );
                    }
                }
                m_table[ offset + i * m_size_scalar + j ] = c;
            }

            else // sb < sf
            {
                kvs::Vector4f c( 0.0f, 0.0f, 0.0f, 0.0f );
                for ( int k = j; k > i; k-- )
                {
                    float beta = 0.0f;
                    const float dbeta = 1.0f / static_cast<float>(M);
                    const float dgamma = 1.0f / static_cast<float>( j - i );
                    kvs::Vector4f c0 = m_transfer_function[k];
                    c0[3] = 1.0f - std::pow( 1.0f - c0[3], dl * dbeta * dgamma );
                    c0[0] *= c0[3];
                    c0[1] *= c0[3];
                    c0[2] *= c0[3];
                    kvs::Vector4f c1 = m_transfer_function[k-1];
                    c1[3] = 1.0f - std::pow( 1.0f - c1[3], dl * dbeta * dgamma );
                    c1[0] *= c1[3];
                    c1[1] *= c1[3];
                    c1[2] *= c1[3];
                    for ( int m = 0; m < M; m++, beta += dbeta )
                    {
                        const kvs::Vector4f ck = c0 * ( 1.0f - beta ) + c1 * beta;
                        c += ck * ( 1.0f - c[3] );
                    }
                }
                m_table[ offset + i * m_size_scalar + j ] = c;
            }
        }
    }
}

/*===========================================================================*/
/**
 *  @brief  Computes approximate level using previous levels.
 *  @param  current_level [in]
 *  @param  previous_level [in]
 *  @param  first_level [in]
 *  @param  l [in]
 *  @param  dl [in]
 */
/*===========================================================================*/
void PreIntegrationTable::compute_incremental_level(
    const int current_level,
    const int previous_level,
    const int first_level,
    const float l,
    const float dl )
{
    const int n = m_size_scalar;
    const int first_offset = first_level * m_size_scalar * m_size_scalar;
    const int prev_offset = previous_level * m_size_scalar * m_size_scalar;
    const int cur_offset = current_level * m_size_scalar * m_size_scalar;
    for ( int i = 0; i < n; i++ )
    {
        for ( int j = 0; j < n; j++ )
        {
            const float sf = ( 2.0f * j + 1.0f ) / ( 2.0f * n );
            const float sb = ( 2.0f * i + 1.0f ) / ( 2.0f * n );
            const float sp = ( ( l - dl ) * sf + ( dl * sb ) ) / l;

            const int k = (int)( sp * (float)n - 0.5f );
            const float beta = sp * (float)n - ( (float)k + 0.5f );
            kvs::Vector4f color_first;
            kvs::Vector4f color_prev;
            if ( k == n - 1 )
            {
                color_first = m_table[ first_offset + k * m_size_scalar + j ];
                color_prev = m_table[ prev_offset + i * m_size_scalar + k ];
            }
            else
            {
                color_first =
                    m_table[ first_offset + k * m_size_scalar + j ] * ( 1.0f - beta ) +
                    m_table[ first_offset + ( k + 1 ) * m_size_scalar + j ] * beta;
                color_prev =
                    m_table[ prev_offset + i * m_size_scalar + k ] * ( 1.0f - beta ) +
                    m_table[ prev_offset + i * m_size_scalar + ( k + 1 ) ] * beta;
            }

            m_table[ cur_offset + i * m_size_scalar + j ] =
                color_first + color_prev * ( 1.0f - color_first[3] );
        }
    }
}

} // end of namespace glew

} // end of namespace kvs
