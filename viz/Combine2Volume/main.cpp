//
//  main.cpp
//  
//
//  Created by njun on 11/25/7.
//  Copyright 2011 Jun Nishimura. All rights reserved.
//

#include <kvs/UnstructuredVolumeImporter>
#include <kvs/UnstructuredVectorToScalar>
#include <kvs/UnstructuredVolumeExporter>

template <typename T>
kvs::ValueArray<float> AnyValueArrayToValueArray( const kvs::AnyValueArray& values )
{
    const size_t value_size = values.size();
    const T* src_values = static_cast<const T*>( values.pointer() );
    kvs::ValueArray<float> dst_values( value_size );
    for ( size_t i = 0; i < value_size; i++ )
    {
        dst_values[i] = static_cast<float>( src_values[i] );
    }

    return( dst_values );
}

template <typename T>
kvs::AnyValueArray Combine2AnyValueArray(
    const kvs::AnyValueArray& values1,
    const kvs::AnyValueArray& values2 )
{
    const size_t value_size = values1.size();
    const T* src_values1 = static_cast<const T*>( values1.pointer() );
    const T* src_values2 = static_cast<const T*>( values2.pointer() );
    kvs::ValueArray<T> dst_values( 2 * value_size );
    for ( size_t i = 0; i < value_size; i++ )
    {
        dst_values[ i * 2 + 0 ] = src_values1[i];
        dst_values[ i * 2 + 1 ] = src_values2[i];
    }

    return( kvs::AnyValueArray( dst_values ) );
}

int main( int argc, char** argv )
{
    if ( argc < 5 )
    {
        kvsMessageError( "Please input : ./Combine2Volume [file path volume1] [file path volume2] > [output file path]" );
        return( - 1 );
    }

    if ( std::string( argv[3] ) != ">" )
    {
        kvsMessageError( "Please input : ./Combine2Volume [file path volume1] [file path volume2] > [output file path]" );
        return( - 1 );
    }

    kvs::UnstructuredVolumeObject* volume1 = new kvs::UnstructuredVolumeImporter( argv[1] );
    if ( volume1->veclen() == 3 )
    {
        kvs::UnstructuredVolumeObject* volume_scalar = new kvs::UnstructuredVectorToScalar( volume1 );
        delete volume1;
        volume1 = volume_scalar;
    }

    kvs::UnstructuredVolumeObject* volume2 = new kvs::UnstructuredVolumeImporter( argv[2] );
    if ( volume2->veclen() == 3 )
    {
        kvs::UnstructuredVolumeObject* volume_scalar = new kvs::UnstructuredVectorToScalar( volume2 );
        delete volume2;
        volume2 = volume_scalar;
    }

    kvs::AnyValueArray values1 = volume1->values();
    kvs::AnyValueArray values2 = volume2->values();
    if ( values1.size() != values2.size() )
    {
        kvsMessageError( "Value size is not same." );
        delete volume1;
        delete volume2;
        return( - 1 );
    }

    kvs::AnyValueArray values12;
    const std::type_info& type1 = volume1->values().typeInfo()->type();
    const std::type_info& type2 = volume2->values().typeInfo()->type();
    if ( type1 != type2 )
    {
        kvs::ValueArray<float> values_f1;
        if (      type1 == typeid( kvs::Int8   ) ) values_f1 = AnyValueArrayToValueArray<kvs::Int8>( values1 );
        else if ( type1 == typeid( kvs::Int16  ) ) values_f1 = AnyValueArrayToValueArray<kvs::Int16>( values1 );
        else if ( type1 == typeid( kvs::Int32  ) ) values_f1 = AnyValueArrayToValueArray<kvs::Int32>( values1 );
        else if ( type1 == typeid( kvs::Int64  ) ) values_f1 = AnyValueArrayToValueArray<kvs::Int64>( values1 );
        else if ( type1 == typeid( kvs::UInt8  ) ) values_f1 = AnyValueArrayToValueArray<kvs::UInt8>( values1 );
        else if ( type1 == typeid( kvs::UInt16 ) ) values_f1 = AnyValueArrayToValueArray<kvs::UInt16>( values1 );
        else if ( type1 == typeid( kvs::UInt32 ) ) values_f1 = AnyValueArrayToValueArray<kvs::UInt32>( values1 );
        else if ( type1 == typeid( kvs::UInt64 ) ) values_f1 = AnyValueArrayToValueArray<kvs::UInt64>( values1 );
        else if ( type1 == typeid( kvs::Real32 ) ) values_f1 = AnyValueArrayToValueArray<kvs::Real32>( values1 );
        else if ( type1 == typeid( kvs::Real64 ) ) values_f1 = AnyValueArrayToValueArray<kvs::Real64>( values1 );
        else
        {
            kvsMessageError( "Unsupported data type '%s'.", volume1->values().typeInfo()->typeName() );
            delete volume1;
            delete volume2;
            return( - 1 );
        }

        kvs::ValueArray<float> values_f2;
        if (      type2 == typeid( kvs::Int8   ) ) values_f2 = AnyValueArrayToValueArray<kvs::Int8>( values2 );
        else if ( type2 == typeid( kvs::Int16  ) ) values_f2 = AnyValueArrayToValueArray<kvs::Int16>( values2 );
        else if ( type2 == typeid( kvs::Int32  ) ) values_f2 = AnyValueArrayToValueArray<kvs::Int32>( values2 );
        else if ( type2 == typeid( kvs::Int64  ) ) values_f2 = AnyValueArrayToValueArray<kvs::Int64>( values2 );
        else if ( type2 == typeid( kvs::UInt8  ) ) values_f2 = AnyValueArrayToValueArray<kvs::UInt8>( values2 );
        else if ( type2 == typeid( kvs::UInt16 ) ) values_f2 = AnyValueArrayToValueArray<kvs::UInt16>( values2 );
        else if ( type2 == typeid( kvs::UInt32 ) ) values_f2 = AnyValueArrayToValueArray<kvs::UInt32>( values2 );
        else if ( type2 == typeid( kvs::UInt64 ) ) values_f2 = AnyValueArrayToValueArray<kvs::UInt64>( values2 );
        else if ( type2 == typeid( kvs::Real32 ) ) values_f2 = AnyValueArrayToValueArray<kvs::Real32>( values2 );
        else if ( type2 == typeid( kvs::Real64 ) ) values_f2 = AnyValueArrayToValueArray<kvs::Real64>( values2 );
        else
        {
            kvsMessageError( "Unsupported data type '%s'.", volume2->values().typeInfo()->typeName() );
            delete volume1;
            delete volume2;
            return( - 1 );
        }

        values12 = Combine2AnyValueArray<float>( kvs::AnyValueArray( values_f1 ), kvs::AnyValueArray( values_f2 ) );
    }
    else
    {
        if (      type1 == typeid( kvs::Int8   ) ) values12 = Combine2AnyValueArray<kvs::Int8>( values1, values2 );
        else if ( type1 == typeid( kvs::Int16  ) ) values12 = Combine2AnyValueArray<kvs::Int16>( values1, values2 );
        else if ( type1 == typeid( kvs::Int32  ) ) values12 = Combine2AnyValueArray<kvs::Int32>( values1, values2 );
        else if ( type1 == typeid( kvs::Int64  ) ) values12 = Combine2AnyValueArray<kvs::Int64>( values1, values2 );
        else if ( type1 == typeid( kvs::UInt8  ) ) values12 = Combine2AnyValueArray<kvs::UInt8>( values1, values2 );
        else if ( type1 == typeid( kvs::UInt16 ) ) values12 = Combine2AnyValueArray<kvs::UInt16>( values1, values2 );
        else if ( type1 == typeid( kvs::UInt32 ) ) values12 = Combine2AnyValueArray<kvs::UInt32>( values1, values2 );
        else if ( type1 == typeid( kvs::UInt64 ) ) values12 = Combine2AnyValueArray<kvs::UInt64>( values1, values2 );
        else if ( type1 == typeid( kvs::Real32 ) ) values12 = Combine2AnyValueArray<kvs::Real32>( values1, values2 );
        else if ( type1 == typeid( kvs::Real64 ) ) values12 = Combine2AnyValueArray<kvs::Real64>( values1, values2 );
        else
        {
            kvsMessageError( "Unsupported data type '%s'.", volume1->values().typeInfo()->typeName() );
            delete volume1;
            delete volume2;
            return( - 1 );
        }
    }

    kvs::UnstructuredVolumeObject* volume12 = new kvs::UnstructuredVolumeObject();
    volume12->setVeclen( 2 );
    volume12->setNNodes( volume1->nnodes() );
    volume12->setNCells( volume1->ncells() );
    volume12->setCellType( volume1->cellType() );
    volume12->setCoords( volume1->coords() );
    volume12->setConnections( volume1->connections() );
    volume12->setValues( values12 );

    kvs::KVSMLObjectUnstructuredVolume* kvsml = new kvs::UnstructuredVolumeExporter<kvs::KVSMLObjectUnstructuredVolume>( volume12 );
    kvsml->setWritingDataType( kvs::KVSMLObjectUnstructuredVolume::ExternalBinary );
    kvsml->write( argv[4] );

    delete volume1;
    delete volume2;
    delete volume12;
    delete kvsml;

    return( 0 );
}
