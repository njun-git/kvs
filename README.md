kvs repository
=============

This repository is sample source code using KVS.

[KVS : Kyoto Visualization System](http://code.google.com/p/kvs/) is is a multi-platform, open-source C++ library for developing scientific visualization applications.

Folders
------

### opengl

OpenGL example using KVS.

You need to install [KVS](http://code.google.com/p/kvs/).

### glew

GLSL example using KVS.

You need to install [glew](http://glew.sourceforge.net/), and reinstall [KVS](http://code.google.com/p/kvs/) with KVS_SUPPORT_GLEW=1 in kvs.conf.

### tdw

Library for tiled display wall using KVS.

To install library:

    $ cd tdw/TDW
    $ kvsmake -G
    $ kvsmake lib

libTDW.a is created. This directory is include path and also library path.

To compile example:

    $ cd tdw/Example/SimpleTriangle
    $ kvsmake -G
    $ kvsmake

To execute example application in local:

    $ ./run_local.sh

You need to install [KVS](http://code.google.com/p/kvs/).
The supported platform are only Mac and Linux.

### viz

Visualization example using KVS.

You need to install [glew](http://glew.sourceforge.net/), and reinstall [KVS](http://code.google.com/p/kvs/) with KVS_SUPPORT_GLEW=1 in kvs.conf.

Usage
-----

### Compile source code

To compile source code:

    $ cd (each source path)
    $ kvsmake -G
    $ kvsmake

### Execution

To execute application:

    $ ./(application name)

Operation Check
------------

These source codes are only checked in Mac OS and Ubuntu.

