//
//  main.cpp
//  
//
//  Created by Jun Nishimura on 10/16/11.
//  Copyright (c) 2011 Jun Nishimura. All rights reserved.
//

#include <iostream>

#include <kvs/cl/OpenCL>
#include <kvs/cl/Platform>
#include <kvs/cl/Device>

int main( int argc, char** argv )
{
    kvs::cl::Platform platform;

    std::cout << kvs::cl::Description() << std::endl << std::endl;
    std::cout << "# of Platforms : " << platform.nplatforms() << std::endl;
    for ( size_t i = 0; i < platform.nplatforms(); i++ )
    {
        std::cout << "Platform [" << i << "] Information ..." << std::endl;
        std::cout << "  Profile : " << platform.profile(i) << std::endl;
        std::cout << "     Name : " << platform.name(i) << std::endl;
        std::cout << "  Version : " << platform.version(i) << std::endl;
        std::cout << "   Vendor : " << platform.vendor(i) << std::endl;
        std::cout << std::endl;

        kvs::cl::Device device( platform.platform(i), kvs::cl::Device::All );
        std::cout << "  # of Devices : " << device.ndevices() << std::endl;

        for ( size_t j = 0; j < device.ndevices(); j++ )
        {
            std::cout << "  Device [" << j << "] Information ..." << std::endl;
            std::cout << "    Profile : " << device.profile(j) << std::endl;
            std::cout << "       Name : " << device.name(j) << std::endl;
            std::cout << "    Version : " << device.version(j) << std::endl;
            std::cout << "     Vendor : " << device.vendor(j) << std::endl;
            std::cout << "       Type : " << device.typeString(j) << std::endl;
            std::cout << std::endl;
        }
    }

    return( EXIT_SUCCESS );
}
