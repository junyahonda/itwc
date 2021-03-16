cmake_minimum_required(VERSION 3.10.0)
project(gerrit-masters)

# Set C++ standard to 17 for decomposition declaration in make_response.cc
set (CMAKE_CXX_STANDARD 17)

# Make sure developers do not run cmake in the main project directory, to keep
# build artifacts from becoming clutter
if (${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})
    message(FATAL_ERROR "In-source builds not allowed.
    Please make a new directory (called a build directory) and run CMake from there.
    You may need to remove CMakeCache.txt." )
endif()
# Turn on debug builds if we are building in a devel shell
if (CMAKE_BUILD_TYPE STREQUAL "" AND "$ENV{DEVEL_SHELL}" STREQUAL "1")
    message(STATUS "Setting debug build type by default in devel shell")
    set(CMAKE_BUILD_TYPE Debug)
endif()
# Output binaries to a sub directory "bin"
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
# Enable cmake testing
include(CTest)
enable_testing()
# Enable GoogleTest
include(GoogleTest)
add_subdirectory(/usr/src/googletest googletest)
# Enable Boost
# Use static libraries so binaries can be deployed without a full boost install
set(Boost_USE_STATIC_LIBS ON)
find_package(Boost 1.50 REQUIRED COMPONENTS system log)
message(STATUS "Boost version: ${Boost_VERSION}")
include_directories(include)
find_program(BASH_PROGRAM bash)



# Update name and srcs

# source files
add_library(base_request_handler src/base_request_handler.cc)
add_library(boost_networking_injector src/boost_networking_injector.cc)
add_library(config_parser src/config_parser.cc)
add_library(data_structures src/data_structures.cc)
add_library(echo_request_handler src/echo_request_handler.cc)
add_library(handle_dispatch src/handle_dispatch.cc)
add_library(health_request_handler src/health_request_handler.cc)
add_library(html_parser src/html_parser.cc)
add_library(itwc_handler src/itwc_handler.cc)
add_library(log_handler src/log_handler.cc)
add_library(make_response src/make_response.cc)
add_library(mime_types src/mime_types.cc)
add_library(not_found_handler src/not_found_handler.cc)
add_library(request_validation src/request_validation.cc)
add_library(response_parser src/response_parser.cc)
add_library(reverse_proxy_handler src/reverse_proxy_handler)
add_library(server_configuration src/server_configuration.cc)
add_library(server_main src/server_main.cc)
add_library(server_server src/server.cc)
add_library(session src/session.cc)
add_library(session_manager src/session_manager.cc)
add_library(static_request_handler src/static_request_handler.cc)
add_library(status_request_handler src/status_request_handler.cc)


target_link_libraries(base_request_handler mime_types make_response)
target_link_libraries(echo_request_handler base_request_handler)
target_link_libraries(health_request_handler base_request_handler data_structures)
target_link_libraries(itwc_handler base_request_handler)
target_link_libraries(static_request_handler base_request_handler)
target_link_libraries(reverse_proxy_handler base_request_handler response_parser html_parser boost_networking_injector Boost::system Boost::log_setup Boost::log)
target_link_libraries(boost_networking_injector response_parser reverse_proxy_handler)
target_link_libraries(server_server static_request_handler echo_request_handler reverse_proxy_handler)





# Update executable name, srcs, and deps
add_executable(server src/server_main.cc)
target_link_libraries(server server_server session_manager session config_parser request_validation log_handler Boost::system Boost::log_setup Boost::log server_configuration mime_types make_response base_request_handler data_structures echo_request_handler static_request_handler not_found_handler reverse_proxy_handler handle_dispatch status_request_handler health_request_handler itwc_handler)

# Update test executable name, srcs, and deps
add_executable(config_parser_test tests/config_parser_test.cc)
target_link_libraries(config_parser_test config_parser log_handler Boost::log_setup Boost::log gtest_main)
add_executable(log_handler_test tests/log_handler_test.cc)
target_link_libraries(log_handler_test log_handler Boost::log_setup Boost::log gtest_main )

#add_executable(validation_test tests/validation_test.cc)
#target_link_libraries(validation_test request_validation log_handler Boost::log_setup Boost::log gtest_main server_configuration)
#add_executable(session_test tests/server_session_test.cc)
#target_link_libraries(session_test server_session request_validation log_handler #Boost::system Boost::log_setup Boost::log gtest_main server_configuration)
#
#add_executable(server_test tests/server_test.cc)
#target_link_libraries(server_test server_server server_session request_validation #log_handler Boost::system Boost::log_setup Boost::log gtest_main server_configuration)

add_executable(mime_type_test tests/mime_type_test.cc)
target_link_libraries(mime_type_test mime_types gtest_main)

add_executable(request_validation_test tests/request_validation_test/request_validation_test.cc)
target_link_libraries(request_validation_test request_validation gtest_main)

add_executable(handle_dispatch_test tests/handle_dispatch_test.cc)
target_link_libraries(handle_dispatch_test handle_dispatch echo_request_handler static_request_handler config_parser log_handler make_response Boost::system Boost::log_setup Boost::log gtest_main)

#add_executable(request_handler_test tests/request_handler_test.cc)
#target_link_libraries(request_handler_test base_request_handler echo_request_handler static_request_handler log_handler data_structures make_response Boost::system Boost::log_setup Boost::log gtest_main)

add_executable(response_parser_test tests/response_parser_test.cc)
target_link_libraries(response_parser_test response_parser gtest_main Boost::system)

add_executable(reverse_proxy_handler_test tests/reverse_proxy_handler_test.cc)
target_link_libraries(reverse_proxy_handler_test reverse_proxy_handler data_structures config_parser config_parser log_handler Boost::system Boost::log_setup Boost::log gtest_main)

add_executable(html_parser_test tests/html_parser_test.cc)
target_link_libraries(html_parser_test html_parser gtest_main)

add_executable(health_handler_test tests/health_handler_test.cc)
target_link_libraries(health_handler_test health_request_handler data_structures config_parser log_handler Boost::system Boost::log_setup Boost::log gtest_main)

add_executable(data_structures_test tests/data_structures_test/data_structures_test.cc)
target_link_libraries(data_structures_test config_parser data_structures gtest_main)

add_executable(echo_handler_test tests/echo_handler_test/echo_handler_test.cc)
target_link_libraries(echo_handler_test mime_types make_response base_request_handler echo_request_handler data_structures config_parser log_handler Boost::system Boost::log_setup Boost::log gtest_main)

add_executable(static_handler_test tests/static_handler_test/static_handler_test.cc)
target_link_libraries(static_handler_test mime_types make_response base_request_handler static_request_handler data_structures config_parser log_handler Boost::system Boost::log_setup Boost::log gtest_main)

add_executable(status_handler_test tests/status_handler_test/status_handler_test.cc)
target_link_libraries(status_handler_test status_request_handler mime_types make_response base_request_handler data_structures config_parser log_handler Boost::system Boost::log_setup Boost::log gtest_main)

add_executable(make_response_test tests/make_response_test/make_response_test.cc)
target_link_libraries(make_response_test make_response mime_types base_request_handler data_structures log_handler Boost::system Boost::log_setup Boost::log gtest_main)

add_executable(not_found_test tests/not_found_test/not_found_test.cc)
target_link_libraries(not_found_test not_found_handler mime_types make_response base_request_handler data_structures config_parser log_handler Boost::system Boost::log_setup Boost::log gtest_main)

add_executable(itwc_test tests/itwc_test/itwc_handler_test.cc)
target_link_libraries(itwc_test mime_types make_response base_request_handler itwc_handler data_structures config_parser log_handler Boost::system Boost::log_setup Boost::log gtest_main)

# Update with test binary
gtest_discover_tests(config_parser_test WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests)
gtest_discover_tests(log_handler_test WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests)
#gtest_discover_tests(validation_test WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests)
#gtest_discover_tests(session_test WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests)
#gtest_discover_tests(server_test WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests)
add_test(NAME integration_test COMMAND bash ${CMAKE_CURRENT_SOURCE_DIR}/tests/test.sh)
add_test(NAME reverse_proxy_integration_test COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/tests/reverse_proxy_integration_test.sh -s "$<TARGET_FILE:server>" WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests)

gtest_discover_tests(mime_type_test WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests)
gtest_discover_tests(request_validation_test WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests/request_validation_test)
#gtest_discover_tests(request_handler_test WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests)
gtest_discover_tests(handle_dispatch_test WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests)
gtest_discover_tests(response_parser_test WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests)
gtest_discover_tests(html_parser_test WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests)
gtest_discover_tests(reverse_proxy_handler_test WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests)

gtest_discover_tests(health_handler_test WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests/)

gtest_discover_tests(data_structures_test WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests/data_structures_test/)

gtest_discover_tests(echo_handler_test WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests/echo_handler_test/)

gtest_discover_tests(static_handler_test WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests/static_handler_test/)

gtest_discover_tests(status_handler_test WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests/status_handler_test/)

gtest_discover_tests(make_response_test WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests/make_response_test/)

gtest_discover_tests(not_found_test WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests/not_found_test/)

gtest_discover_tests(itwc_test WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests/itwc_test/)

# Update with target/test targets
include(cmake/CodeCoverageReportConfig.cmake)
#generate_coverage_report(TARGETS server_main server server_server server_session config_parser request_validation log_handler server_configuration TESTS config_parser_test validation_test session_test server_test)



generate_coverage_report(TARGETS server_main server server_server session session_manager config_parser request_validation log_handler server_configuration mime_types make_response base_request_handler data_structures not_found_handler echo_request_handler static_request_handler status_request_handler reverse_proxy_handler health_request_handler handle_dispatch response_parser html_parser itwc_handler TESTS config_parser_test log_handler_test mime_type_test request_validation_test handle_dispatch_test response_parser_test html_parser_test reverse_proxy_handler_test health_handler_test data_structures_test echo_handler_test static_handler_test status_handler_test make_response_test not_found_test itwc_test)
