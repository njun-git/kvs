/*****************************************************************************/
/**
 *  @file   PreItegrationTable.h
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
#ifndef KVS__GLEW__PRE_INTEGRATION_TABLE_H_INCLUDE
#define KVS__GLEW__PRE_INTEGRATION_TABLE_H_INCLUDE

#include <kvs/ClassName>
#include <kvs/Vector4>
#include <kvs/TransferFunction>
#include <kvs/glew/Texture3D>


namespace kvs
{

namespace glew
{

/*===========================================================================*/
/**
 *  @brief  PreItegrationTable class.
 */
/*===========================================================================*/
class PreIntegrationTable
{
    kvsClassName_without_virtual( PreIntegrationTable );

protected:

    kvs::Vector4f*       m_transfer_function; ///< transfer function data
    kvs::Vector4f*       m_table;         ///< pre-integration table
    kvs::glew::Texture3D m_table_texture; ///< pre-integration table texture
    int                  m_size_scalar;   ///< table size for the scalar axis
    int                  m_size_depth;    ///< table size for the depth axis

public:

    PreIntegrationTable( void );

    PreIntegrationTable( const int size_scalar, const int size_depth );

    ~PreIntegrationTable( void );

public:

    const int sizeDepth( void ) const;

public:

    void setTableSize( const int size_scalar, const int size_depth );

    void setTransferFunction(
        const kvs::TransferFunction& transfer_function,
        const float min_scalar,
        const float max_scalar );

    void create( const float max_edge_length );

    void download( void );

    void bind( void );

    void unbind( void );

    void release( void );

    const bool isTexture( void );

    const bool isDownloaded( void );

private:

    void compute_exact_level( const int level, const float dl );

    void compute_incremental_level(
        const int current_level,
        const int previous_level,
        const int first_level,
        const float l,
        const float dl );
};

} // end of namespace glew

} // end of namespace kvs

#endif // KVS__GLEW__PRE_INTEGRATION_TABLE_H_INCLUDE
