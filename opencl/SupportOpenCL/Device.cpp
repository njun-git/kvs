//
//  Device.cpp
//  
//
//  Created by Jun Nishimura on 10/14/11.
//  Copyright (c) 2011 Jun Nishimura. All rights reserved.
//

#include "Device.h"

#include <kvs/Assert>
#include <kvs/Message>
#include "ErrorString.h"

namespace kvs
{

namespace cl
{

Device::Device( void ) :
    m_type( Default ),
    m_ndevices( 0 ),
    m_devices( NULL )
{
}

Device::Device(
    const kvs::cl::CLPlatform& platform,
    const DeviceType& type ) :
    m_type( type ),
    m_ndevices( 0 ),
    m_devices( NULL )
{
    this->initialize( platform );
}

Device::~Device( void )
{
    this->clear();
}

void Device::initialize( const kvs::cl::CLPlatform& platform )
{
    this->clear();

    const cl_device_type device_type = static_cast<cl_device_type>( m_type );
    cl_int result = clGetDeviceIDs( platform, device_type, 0, NULL, &m_ndevices );
    if ( result != CL_SUCCESS )
    {
        kvsMessageError( "OpenCL; %s.", kvs::cl::ErrorString( result ) );
        return;
    }

    if ( m_ndevices > 0 )
    {
        m_devices = new CLDevice[ m_ndevices ];
        result = clGetDeviceIDs( platform, CL_DEVICE_TYPE_ALL, m_ndevices, m_devices, NULL );
        if ( result != CL_SUCCESS )
        {
            kvsMessageError( "OpenCL; %s.", kvs::cl::ErrorString( result ) );
            this->clear();
        }
    }
}

void Device::clear( void )
{
    if ( m_devices )
    {
        delete [] m_devices;
        m_devices = NULL;
    }

    m_ndevices = 0;
}

void Device::setDeviceType( const DeviceType& type )
{
    m_type = type;
}

const size_t Device::ndevices( void ) const
{
    return( m_ndevices );
}

const CLDevice* Device::devices( void ) const
{
    return( m_devices );
}

const CLDevice& Device::device( const size_t index ) const
{
    KVS_ASSERT( index < static_cast<size_t>( m_ndevices ) );

    return( m_devices[index] );
}

const std::string Device::profile( const size_t index ) const
{
    return( this->get_device_info( index, CL_DEVICE_PROFILE ) );
}

const std::string Device::version( const size_t index ) const
{
    return( this->get_device_info( index, CL_DEVICE_VERSION ) );
}

const std::string Device::name( const size_t index ) const
{
    return( this->get_device_info( index, CL_DEVICE_NAME ) );
}

const std::string Device::vendor( const size_t index ) const
{
    return( this->get_device_info( index, CL_DEVICE_VENDOR ) );
}

const std::string Device::extensions( const size_t index ) const
{
    return( this->get_device_info( index, CL_DEVICE_EXTENSIONS ) );
}

const std::string Device::driverVersion( const size_t index ) const
{
    return( this->get_device_info( index, CL_DRIVER_VERSION ) );
}

const Device::DeviceType Device::type( const size_t index ) const
{
    KVS_ASSERT( index < static_cast<size_t>( m_ndevices ) );

    cl_device_type type;
    cl_int result = clGetDeviceInfo( m_devices[index], CL_DEVICE_TYPE, sizeof(type), &type, NULL );
    if ( result != CL_SUCCESS )
    {
        kvsMessageError( "OpenCL; %s.", kvs::cl::ErrorString( result ) );
    }
    return( DeviceType( type ) );
}

const std::string Device::typeString( const size_t index ) const
{
    DeviceType type = this->type( index );
    switch ( type )
    {
        case CPU: return( std::string( "CPU" ) ); break;
        case GPU: return( std::string( "GPU" ) ); break;
        case Accelerator: return( std::string( "Accelerator" ) ); break;
        case All: return( std::string( "All" ) ); break;

        default: break;
    }
    return( std::string( "Default" ) );
}

const bool Device::isAvailable( const size_t index ) const
{
    return( this->get_device_info_bool( index, CL_DEVICE_AVAILABLE ) );
}

const bool Device::isCompilerAvailable( const size_t index ) const
{
    return( this->get_device_info_bool( index, CL_DEVICE_COMPILER_AVAILABLE ) );
}

const bool Device::isEndianLittle( const size_t index ) const
{
    return( this->get_device_info_bool( index, CL_DEVICE_ENDIAN_LITTLE ) );
}

const bool Device::isErrorCorrectionSupport( const size_t index ) const
{
    return( this->get_device_info_bool( index, CL_DEVICE_ERROR_CORRECTION_SUPPORT ) );
}

const bool Device::isImageSupport( const size_t index ) const
{
    return( this->get_device_info_bool( index, CL_DEVICE_IMAGE_SUPPORT ) );
}

const std::string Device::get_device_info( const size_t index, const cl_device_info info ) const
{
    KVS_ASSERT( index < static_cast<size_t>( m_ndevices ) );

    char buf[1024];
    cl_int result = clGetDeviceInfo( m_devices[index], info, sizeof(buf), buf, NULL );
    if ( result != CL_SUCCESS )
    {
        kvsMessageError( "OpenCL; %s.", kvs::cl::ErrorString( result ) );
    }
    return( std::string( buf ) );
}

const bool Device::get_device_info_bool( const size_t index, const cl_device_info info ) const
{
    KVS_ASSERT( index < static_cast<size_t>( m_ndevices ) );

    cl_bool is_true;
    cl_int result = clGetDeviceInfo( m_devices[index], info, sizeof(is_true), &is_true, NULL );
    if ( result != CL_SUCCESS )
    {
        kvsMessageError( "OpenCL; %s.", kvs::cl::ErrorString( result ) );
    }
    return( is_true == CL_TRUE );
}

}

}
