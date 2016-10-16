# Distributed under the Boost Software License, Version 1.0.
# See http://www.boost.org/LICENSE_1_0.txt

cmake_minimum_required(VERSION 3.5)
project(BoostCGI LANGUAGES CXX)

# header only library, build tests only
add_subdirectory(libs/cgi/test)

set(BoostCGI_USED_CXX_FEATURES cxx_auto_type)


#set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/../ryppl-cmake/Modules/")

# Ryppl does not work for me so far 8-(
#include(Ryppl)

#ryppl_add_test_subdirectory(libs/process/test)

#ryppl_export(
#  DEPENDS
#    BoostConfig
#    BoostCore
#    BoostException
#    BoostFunsion
#    BoostIterator
#    BoostStaticAssert
#  RECOMMENDS
#  INCLUDE_DIRECTORIES
#    .
#)
