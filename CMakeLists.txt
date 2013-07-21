# Distributed under the Boost Software License, Version 1.0.
# See http://www.boost.org/LICENSE_1_0.txt

cmake_minimum_required(VERSION 2.8)
project(BoostCGI)

# header only library, build tests only
add_subdirectory(libs/cgi/test)

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
