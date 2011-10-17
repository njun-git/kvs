#!/usr/bin/python

import sys
import os


#=============================================================================
#  Initialize.
#=============================================================================
# source_path      = sys.argv[1]
# destination_path = sys.argv[2]

header_list = []

# f = open( "../Source/" + source_path + "/KVS_HEADER_LIST", "r" )
f = open( "KVS_HEADER_LIST", "r" )
for line in f.readlines():
    header_list.append( line.rstrip( "\n" ) )


#=============================================================================
#  Configure include header.
#=============================================================================
for header_path in header_list:
    header_name = os.path.basename( header_path )
#     sys.stdout = open( "../Source/" + destination_path + "/" + header_name, "w" )
    sys.stdout = open( "../kvs/tdw/" + header_name, "w" )
    print "#include <%s.h>" % ( header_path )


#=============================================================================
#  Configure include header list.
#=============================================================================
# sys.stdout = open( "../Source/kvs" + source_path, "w" )
sys.stdout = open( "../kvsSupportTDW", "w" )
for header_path in header_list:
     print "#include <kvs/tdw/%s.h>" % ( header_path )
