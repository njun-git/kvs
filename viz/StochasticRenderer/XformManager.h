
#ifndef KVS__XFORM_MANAGER_H_INCLUDE
#define KVS__XFORM_MANAGER_H_INCLUDE

#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <kvs/Message>
#include <kvs/File>
#include <kvs/ScreenBase>
#include <kvs/ObjectManager>
#include <kvs/Xform>
#include <kvs/FileFormatBase>
#include <kvs/ClassName>

namespace kvs
{

class XformManager
{

    kvsClassName( kvs::XformManager );

protected:

    kvs::ScreenBase*    m_ref_screen;

public:

    XformManager( kvs::ScreenBase* screen );

    virtual ~XformManager( void );

public:

    const bool applyXform( const std::string& filename );

    const bool saveXform( const std::string& filename );

};

}

#endif
