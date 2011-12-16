//
//  PolygonToParticle.cpp
//  
//
//  Created by njun on 11/27/7.
//  Copyright 2011 Jun Nishimura. All rights reserved.
//

#include "PolygonToParticle.h"

namespace
{

float linear_mix( float a, float b, float factor )
{
    return( factor * a + ( 1.0 - factor ) * b );
}

kvs::Vector3f linear_mix3f( kvs::Vector3f a, kvs::Vector3f b, float factor )
{
    float x = linear_mix( a.x(), b.x(), factor );
    float y = linear_mix( a.y(), b.y(), factor );
    float z = linear_mix( a.z(), b.z(), factor );

    return kvs::Vector3f( x, y, z );
}

// generate one random point in a triangle
kvs::Vector3f sample_in_triangle( kvs::Vector3f a, kvs::Vector3f b, kvs::Vector3f c )
{
    kvs::Vector3f r;

    bool inside = false;

    int count = 0;
    do
    {
        // generate one random point 'ab' on one side
        float factor = kvs::CellByCellParticleGenerator::GetRandomNumber();
        kvs::Vector3f d = linear_mix3f( a, b, factor );

        // calc the vector 'ad'
        kvs::Vector3f vAD = d - a;

        // calc c_
        kvs::Vector3f c_ = c + vAD;

        //  random point between c_ and d
        float factor2 = kvs::CellByCellParticleGenerator::GetRandomNumber();
        r = linear_mix3f( c_, d, factor2 );

        // judge if point in triangle
        kvs::Vector3f vAC = a - c;
        float ac = vAC.length();
        // calc the distance of d, d_
        float dd_ = factor * ac;
        // calc the distance of d, r
        kvs::Vector3f vDR = r - d;
        float dr = vDR.length();

        if( dr < dd_ ) inside = true;

        if( count >= 10000 ) inside = true;
        count ++;
    }
    while ( !inside );

    return r;
}

// average sampling 
kvs::Vector3f average_sample( kvs::Vector3f a, kvs::Vector3f b, kvs::Vector3f c, int current_p, int nParticles )
{
    kvs::Vector3f vBA = a - b;
    float ba = vBA.length();
//printf(" aa %d¥n", nParticles);
    kvs::Vector3f vBC = c - b;
    float bc = vBC.length();

    // calc step length
    float t = sqrt( ba * bc / (2.0 * nParticles) );

    // # of steps on each side
    int num_ba = ba / t + 1;
    int num_bc = bc / t + 1;

    bool inside = false;

    kvs::Vector3f sample;

    static int current;
    if( current_p == 0 ) current = 0;
//printf(" 11 t %f bc %f¥n",t, bc);
    do
    {
//printf(" 22 num_ba %d¥n",num_ba);
        // current id on bc,ba side
        int i_bc = current % num_ba;
//printf(" 33 num_bc %f¥n",num_bc);
        int i_ba = current / num_bc;
//printf(" 44¥n");
        // unit vector of each side
        kvs::Vector3f v_ba = vBA / num_ba;
        kvs::Vector3f v_bc = vBC / num_bc;

        // calc current sampled particle
        kvs::Vector3f d = b + v_bc * i_bc;    
        sample = b + v_bc * i_bc + v_ba * i_ba;

        // judge if point in triangle
        kvs::Vector3f vAC = a - c;
        //float ac = vAC.length();
        
        // calc the distance of d, sample
        float dsample = i_ba * t;
        float db = i_bc * t;
        float factor = abs( bc - db )/bc;
        float dd_ = factor * ba;

        if( dsample < dd_ ) inside = true;

        //if( current >= 20000 ) inside = true;
        current ++;
    }
    while ( !inside );

    return sample;
}

float area_of_triangle( kvs::Vector3f a, kvs::Vector3f b, kvs::Vector3f c )
{
    // Heron's Formula
    float cl = ( a - b ).length();
    float bl = ( a - c ).length();
    float al = ( b - c ).length();

    float p = 0.5 * ( al + bl + cl );


    return sqrt( p * ( p - al ) * ( p - bl ) * ( p - cl ) );
}

kvs::Vector3f interp_in_triangle( kvs::Vector3f p, 
                                 kvs::Vector3f a, kvs::Vector3f Na,
                                 kvs::Vector3f b, kvs::Vector3f Nb,
                                 kvs::Vector3f c, kvs::Vector3f Nc )
{
    float pa = 1.0/( p - a ).length();
    float pb = 1.0/( p - b ).length();
    float pc = 1.0/( p - c ).length();

    // calt normalized factor
    float sum = pa + pb + pc;
    float a_ = pa/sum;
    float b_ = pb/sum;
    float c_ = pc/sum;

    return ( a_ * Na + b_ * Nb + c_ * Nc );
}

kvs::Vector3f normal_in_triangle( kvs::Vector3f a, kvs::Vector3f b, kvs::Vector3f c )
{
    //Normal.x = (v1.y * v2.z) - (v1.z * v2.y);
    //Normal.y = -((v2.z * v1.x) - (v2.x * v1.z));
    //Normal.z = (v1.x * v2.y) - (v1.y * v2.x);

    kvs::Vector3f v1 = b - a;
    kvs::Vector3f v2 = c - a;
    float x, y, z;
    x = ( v1.y() * v2.z() ) - ( v1.z() * v2.y() );
    y = -( ( v2.z() * v1.x() ) - ( v2.x() * v1.z() ) );
    z = ( v1.x() * v2.y() ) - ( v1.y() * v2.x() );
    
    kvs::Vector3f normal;
    normal.set( x, y, z );

    return normal;
}

}

namespace kvs
{

PolygonToParticle::PolygonToParticle( void ) :
    kvs::PointObject()
{
}

PolygonToParticle::PolygonToParticle(
    const kvs::PolygonObject* polygon,
    const size_t nparticles ) :
    kvs::PointObject()
{
    this->mapping( polygon, nparticles );
}

PolygonToParticle::~PolygonToParticle( void )
{
}

void PolygonToParticle::mapping( const kvs::PolygonObject* polygon, const size_t nparticles )
{
    if ( polygon->polygonType() != kvs::PolygonObject::Triangle )
    {
        kvsMessageError( "Only support triangle polyon." );
        return;
    }

    size_t nvertices = polygon->nvertices();
    size_t npolygons = polygon->nconnections();

    kvs::ValueArray<kvs::Real32> polygon_coords( polygon->coords() );
    kvs::ValueArray<kvs::UInt32> polygon_conncections;
    kvs::ValueArray<kvs::Real32> polygon_normals;

    if ( npolygons == 0 )
    {
        npolygons = nvertices / 3;
        polygon_conncections.allocate( 3 * npolygons );
        for ( size_t i = 0; i < 3 * npolygons; i++ ) polygon_conncections[i] = i;
    }
    else
    {
        polygon_conncections = polygon->connections();
    }

    if ( polygon->nnormals() == nvertices )
    {
        polygon_normals = polygon->normals();
    }
    else
    {
        polygon_normals.allocate( 3 * nvertices );
        polygon_normals.fill( 0x00 );
        for ( size_t i = 0; i < npolygons; i++ )
        {
            kvs::Vector3f v0( polygon_coords.pointer() + 3 * polygon_conncections[ i * 3     ] );
            kvs::Vector3f v1( polygon_coords.pointer() + 3 * polygon_conncections[ i * 3 + 1 ] );
            kvs::Vector3f v2( polygon_coords.pointer() + 3 * polygon_conncections[ i * 3 + 2 ] );

            kvs::Vector3f normal = normal_in_triangle( v0, v1, v2 );
            for ( size_t j = 0; j < 3; j++ )
            {
                polygon_normals[ 3 * polygon_conncections[ i * 3 + j ]     ] += normal.x();
                polygon_normals[ 3 * polygon_conncections[ i * 3 + j ] + 1 ] += normal.y();
                polygon_normals[ 3 * polygon_conncections[ i * 3 + j ] + 2 ] += normal.z();
            }
        }
        for ( size_t i = 0; i < nvertices; i++ )
        {
            kvs::Vector3f normal( polygon_normals.pointer() + 3 * i );
            normal.normalize();
            polygon_normals[ i * 3     ] = normal.x();
            polygon_normals[ i * 3 + 1 ] = normal.y();
            polygon_normals[ i * 3 + 2 ] = normal.z();
        }
    }

    std::cout << "nvertices : " << nvertices << std::endl;
    std::cout << "npolygons : " << npolygons << std::endl;

    float total_area = 0.0f;
    for ( size_t i = 0; i < npolygons; i++ )
    {
        kvs::Vector3f v0( polygon_coords.pointer() + 3 * polygon_conncections[ i * 3     ] );
        kvs::Vector3f v1( polygon_coords.pointer() + 3 * polygon_conncections[ i * 3 + 1 ] );
        kvs::Vector3f v2( polygon_coords.pointer() + 3 * polygon_conncections[ i * 3 + 2 ] );
        total_area += area_of_triangle( v0, v1, v2 );
    }
    std::cout << "total_area : " << total_area << std::endl;

    const float lattice = sqrtf( total_area / ( nparticles * 0.88 ) );
    const float np_unit = ( nparticles * 0.8 ) / total_area;
    std::cout << "lattice : " << lattice << std::endl;
    std::cout << "np_unit : " << np_unit << std::endl;

    std::vector<kvs::Real32> coords;
    std::vector<kvs::UInt8>  colors;
    std::vector<kvs::Real32> normals;
    for ( size_t i = 0; i < npolygons; i++ )
    {
        kvs::Vector3f v0( polygon_coords.pointer() + 3 * polygon_conncections[ i * 3     ] );
        kvs::Vector3f v1( polygon_coords.pointer() + 3 * polygon_conncections[ i * 3 + 1 ] );
        kvs::Vector3f v2( polygon_coords.pointer() + 3 * polygon_conncections[ i * 3 + 2 ] );

        kvs::Vector3f n0( polygon_normals.pointer() + 3 * polygon_conncections[ i * 3     ] );
        kvs::Vector3f n1( polygon_normals.pointer() + 3 * polygon_conncections[ i * 3 + 1 ] );
        kvs::Vector3f n2( polygon_normals.pointer() + 3 * polygon_conncections[ i * 3 + 2 ] );

        const float area = area_of_triangle( v0, v1, v2 );
        const float np_in_triangle_f = area * np_unit;
        size_t np_in_triangle = static_cast<size_t>( np_in_triangle_f );
        if ( np_in_triangle_f < 1.0f )
        {
            np_in_triangle = ( kvs::CellByCellParticleGenerator::GetRandomNumber() < np_in_triangle_f ) ? 1 : 0;
        }

        for ( size_t j = 0; j < np_in_triangle; j++ )
        {
            kvs::Vector3f coord = sample_in_triangle( v0, v1, v2 );
            coords.push_back( coord.x() );
            coords.push_back( coord.y() );
            coords.push_back( coord.z() );

            kvs::Vector3f normal = interp_in_triangle( coord, v0, n0, v1, n1, v2, n2 );
            normals.push_back( normal.x() );
            normals.push_back( normal.y() );
            normals.push_back( normal.z() );

            colors.push_back( 255 );
            colors.push_back( 255 );
            colors.push_back( 255 );
        }
    }

    BaseClass::setCoords( kvs::ValueArray<kvs::Real32>( coords ) );
    BaseClass::setColors( kvs::ValueArray<kvs::UInt8>( colors ) );
    BaseClass::setNormals( kvs::ValueArray<kvs::Real32>( normals ) );
    BaseClass::updateMinMaxCoords();

    std::cout << "generated particles : " << coords.size() / 3 << std::endl;
}

}
