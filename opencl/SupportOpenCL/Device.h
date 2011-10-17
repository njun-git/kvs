//
//  Device.h
//  
//
//  Created by Jun Nishimura on 10/14/11.
//  Copyright (c) 2011 Jun Nishimura. All rights reserved.
//

#ifndef KVS__OPENCL__DEVICE_H_INCLUDE
#define KVS__OPENCL__DEVICE_H_INCLUDE

#include <kvs/ClassName>

#include "OpenCL.h"
#include "Platform.h"

namespace kvs
{

namespace cl
{

typedef cl_device_id CLDevice;

class Device
{

    // Class name.
    kvsClassName( kvs::cl::Device );

public:

    enum DeviceType
    {
        CPU         = CL_DEVICE_TYPE_CPU,
        GPU         = CL_DEVICE_TYPE_GPU,
        Accelerator = CL_DEVICE_TYPE_ACCELERATOR,
        Default     = CL_DEVICE_TYPE_DEFAULT,
        All         = CL_DEVICE_TYPE_ALL,
    };

protected:

    DeviceType  m_type;
    cl_uint     m_ndevices;
    CLDevice*   m_devices;

public:

    Device( void );

    Device( const kvs::cl::CLPlatform& platform, const DeviceType& type = Default );

    virtual ~Device( void );

public:

    void initialize( const kvs::cl::CLPlatform& platform );

    void clear( void );

public:

    void setDeviceType( const DeviceType& type );

public:

    const size_t ndevices( void ) const;

    const CLDevice* devices( void ) const;

    const CLDevice& device( const size_t index = 0 ) const;

public:

    const std::string profile( const size_t index = 0 ) const;

    const std::string version( const size_t index = 0 ) const;

    const std::string name( const size_t index = 0 ) const;

    const std::string vendor( const size_t index = 0 ) const;

    const std::string extensions( const size_t index = 0 ) const;

    const std::string driverVersion( const size_t index = 0 ) const;

    const DeviceType type( const size_t index = 0 ) const;

    const std::string typeString( const size_t index = 0 ) const;

public:

    const bool isAvailable( const size_t index = 0 ) const;

    const bool isCompilerAvailable( const size_t index = 0 ) const;

    const bool isEndianLittle( const size_t index = 0 ) const;

    const bool isErrorCorrectionSupport( const size_t index = 0 ) const;

    const bool isImageSupport( const size_t index = 0 ) const;

private:

    const std::string get_device_info( const size_t index, const cl_device_info info ) const;

    const bool get_device_info_bool( const size_t index, const cl_device_info info ) const;

};

}

}

#endif
