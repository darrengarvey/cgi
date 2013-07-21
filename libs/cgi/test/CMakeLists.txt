# Distributed under the Boost Software License, Version 1.0.
# See http://www.boost.org/LICENSE_1_0.txt

if(EXISTS "${PROJECT_SOURCE_DIR}/../boost_1_48_0")
    set(BOOST_ROOT "${PROJECT_SOURCE_DIR}/../boost_1_48_0")
    set(BOOST_INCLUDEDIR "${PROJECT_SOURCE_DIR}/../boost_1_48_0")
    set(BOOST_LIBRARYDIR "${PROJECT_SOURCE_DIR}/../boost_1_48_0/stage/lib")
    message("setting BOOST_ROOT to: ${BOOST_ROOT}")
else()
    set(BOOST_LIBRARYDIR "/usr/lib")
endif()

find_package(Boost COMPONENTS filesystem thread iostreams date_time system unit_test_framework REQUIRED)

include_directories(
    .
    ${PROJECT_SOURCE_DIR}
    ${Boost_INCLUDE_DIRS}
    )

link_directories(
    "${BOOST_LIBRARYDIR}"
    )

if(MSVC)
    add_definitions("-D_SCL_SECURE_NO_WARNINGS")
    add_definitions("-D_CRT_SECURE_NO_DEPRECATE")

#boost_filesystem-vc100-mt-gd-1_48.dll
    # Execute the INSTALL project to copy the required boost libs to the Debug and Release directories.

    get_filename_component(FS_BASE_NAME_REL ${Boost_FILESYSTEM_LIBRARY_RELEASE} NAME_WE)
    get_filename_component(FS_BASE_NAME_DBG ${Boost_FILESYSTEM_LIBRARY_DEBUG} NAME_WE)
    get_filename_component(THRD_BASE_NAME_REL ${Boost_THREAD_LIBRARY_RELEASE} NAME_WE)
    get_filename_component(THRD_BASE_NAME_DBG ${Boost_THREAD_LIBRARY_DEBUG} NAME_WE)
    get_filename_component(DT_BASE_NAME_REL ${Boost_DATE_TIME_LIBRARY_RELEASE} NAME_WE)
    get_filename_component(DT_BASE_NAME_DBG ${Boost_DATE_TIME_LIBRARY_DEBUG} NAME_WE)
    get_filename_component(SYS_BASE_NAME_REL ${Boost_SYSTEM_LIBRARY_RELEASE} NAME_WE)
    get_filename_component(SYS_BASE_NAME_DBG ${Boost_SYSTEM_LIBRARY_DEBUG} NAME_WE)
    get_filename_component(UTF_BASE_NAME_REL ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY_RELEASE} NAME_WE)
    get_filename_component(UTF_BASE_NAME_DBG ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY_DEBUG} NAME_WE)

    install(FILES ${BOOST_LIBRARYDIR}/${FS_BASE_NAME_DBG}.dll
                  ${BOOST_LIBRARYDIR}/${THRD_BASE_NAME_DBG}.dll
                  ${BOOST_LIBRARYDIR}/${DT_BASE_NAME_DBG}.dll
                  ${BOOST_LIBRARYDIR}/${SYS_BASE_NAME_DBG}.dll
                  ${BOOST_LIBRARYDIR}/${UTF_BASE_NAME_DBG}.dll
        DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/Debug
        CONFIGURATIONS Debug
    )
    install(FILES ${BOOST_LIBRARYDIR}/${FS_BASE_NAME_REL}.dll
                  ${BOOST_LIBRARYDIR}/${THRD_BASE_NAME_REL}.dll
                  ${BOOST_LIBRARYDIR}/${DT_BASE_NAME_REL}.dll
                  ${BOOST_LIBRARYDIR}/${SYS_BASE_NAME_REL}.dll
                  ${BOOST_LIBRARYDIR}/${UTF_BASE_NAME_REL}.dll
        DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/Release
        CONFIGURATIONS Release RelWithDebInfo
    )
    
endif()

###
### "compile only" for the subdirectory compile:
###

#error: there is no file boost/cgi/acgi/acceptor.hpp
#add_executable(acgi_acceptor compile/acgi_acceptor.cpp)
#target_link_libraries(acgi_acceptor ${Boost_LIBRARIES})

add_executable(async_stdio_connection compile/async_stdio_connection.cpp)
target_link_libraries(async_stdio_connection ${Boost_LIBRARIES})

add_executable(cgi_header_check compile/cgi_header_check.cpp)
target_link_libraries(cgi_header_check ${Boost_LIBRARIES})

#acgi_service.cpp
#acgi_request.cpp

add_executable(fcgi_request compile/fcgi_request.cpp)
target_link_libraries(fcgi_request ${Boost_LIBRARIES})

#error: boost/cgi/acgi/service.hpp: No such file or directory
#add_executable(cgi_service compile/cgi_service.cpp)
#target_link_libraries(cgi_service ${Boost_LIBRARIES})

add_executable(cgi_request compile/cgi_request.cpp)
target_link_libraries(cgi_request ${Boost_LIBRARIES})

add_executable(fcgi_service compile/fcgi_service.cpp)
target_link_libraries(fcgi_service ${Boost_LIBRARIES})

add_executable(fcgi_acceptor compile/fcgi_acceptor.cpp)
target_link_libraries(fcgi_acceptor ${Boost_LIBRARIES})

# error: 'async_cgi' is not a member of 'boost::cgi::common::tags'
#add_executable(is_async_test compile/is_async_test.cpp)
#target_link_libraries(is_async_test ${Boost_LIBRARIES})

add_executable(response compile/response.cpp)
target_link_libraries(response ${Boost_LIBRARIES})

add_executable(scgi_acceptor compile/scgi_acceptor.cpp)
target_link_libraries(scgi_acceptor ${Boost_LIBRARIES})

add_executable(stdio_connection compile/stdio_connection.cpp)
target_link_libraries(stdio_connection ${Boost_LIBRARIES})

# boost/cgi/scgi/request_service.ipp:147: error: no matching function for call to 'boost::cgi::common::basic_connection<boost::cgi::common::tags::tcp_socket>::read_some(boost::asio::mutable_buffers_1)'
#add_executable(scgi_request compile/scgi_request.cpp)
#target_link_libraries(scgi_request ${Boost_LIBRARIES})

add_executable(scgi_service compile/scgi_service.cpp)
target_link_libraries(scgi_service ${Boost_LIBRARIES})

# boost/cgi/connections/shareable_tcp_socket.hpp:58: error: 'request_type' in 'boost::cgi::common::basic_connection<boost::cgi::common::tags::shareable_tcp_socket>::traits {aka struct boost::cgi::common::protocol_traits<boost::cgi::common::tags::fcgi>}' does not name a type
#add_executable(shareable_tcp_connection compile/shareable_tcp_connection.cpp)
#target_link_libraries(shareable_tcp_connection ${Boost_LIBRARIES})

add_executable(tcp_connection compile/tcp_connection.cpp)
target_link_libraries(tcp_connection ${Boost_LIBRARIES})


###
### "compile and run" for the subdirectory run:
###

# error: boost/cgi/acgi.hpp: No such file or directory
#add_executable(acgi_simple_request run/acgi_simple_request.cpp)
#target_link_libraries(acgi_simple_request ${Boost_LIBRARIES})

# run/stdio_connection.cpp:2: error: boost/cgi/buffer.hpp: No such file or directory
# run/stdio_connection.cpp

# fails to load data_map_proxy.hpp
# run/data_map_proxy.cpp
# run/request_data.cpp

# run/has_key.cpp:3: error: boost/cgi/utility/has_key.hpp: No such file or directory
# run/has_key.cpp

# run/sessions.cpp:24: error: 'session_manager' has no member named 'start' [and some more like these]
# run/sessions.cpp

# these are the compiling and running unit tests
set(UNITTEST_SRC run/cgi_test.cpp
                 run/map_test.cpp run/name_test.cpp
                 run/parse_options.cpp run/request_test_template.hpp
                 )

add_executable(testcgi ${UNITTEST_SRC})
target_link_libraries(testcgi ${Boost_LIBRARIES})

# these are the compiling and but not successfully running unit tests.
# most failures are cookie related
set(UNITTEST_FAILING run/cgi_simple_request.cpp run/cookie.cpp run/hello_world.cpp
                     run/response.cpp)

add_executable(testcgifailing ${UNITTEST_FAILING})
target_link_libraries(testcgifailing ${Boost_LIBRARIES})

#include(Ryppl)
#include(RypplTestSuite)

#ryppl_find_and_use_package(BoostCore)
#ryppl_find_and_use_package(BoostTest)
#ryppl_find_and_use_package(BoostTimer)

## Search tests
#ryppl_test_suite(
#  RUN
#    basic_compile_test.cpp
#    basic_run_test.cpp
#    double_path_fail.cpp
#  COMPILE
#    child_process.cpp
#  NO_SINGLE_TARGET
#  )
