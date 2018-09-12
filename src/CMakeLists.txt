cmake_minimum_required(VERSION 3.5)

project(http_server)

find_package (Threads)

add_compile_options(-Wall -Wextra -Wpedantic)
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG}" -ggdb )
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE}" -O3 -DNDEBUG -pipe)

#
# libmpm_X
#
add_subdirectory(mpm)


#
#
# Executable
# http_server
#
#
set(CORE_SRC "core/parse_request.c" "core/http.c" "core/process_request.c" "utility/utility.c" "core/generate_response_string.c" "config.c")

add_executable(concurrent_http_server_A concurrent_http_server.c utility/flag.c ${CORE_SRC})
# Link lib
target_link_libraries (concurrent_http_server_A LINK_PUBLIC libmpm_A ${CMAKE_THREAD_LIBS_INIT})
# Include Dir
target_include_directories (concurrent_http_server_A PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
# stdc++
target_compile_features(concurrent_http_server_A PUBLIC c_std_11)


add_executable(concurrent_http_server_B concurrent_http_server.c utility/flag.c ${CORE_SRC})
# Link lib
target_link_libraries (concurrent_http_server_B LINK_PUBLIC libmpm_B ${CMAKE_THREAD_LIBS_INIT})
# Include Dir
target_include_directories (concurrent_http_server_B PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
# stdc++
target_compile_features(concurrent_http_server_B PUBLIC c_std_11)


#add_executable(http_server http_server.c ${CORE_SRC})
# Include Dir
#target_include_directories (http_server PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
# stdc++
#target_compile_features(http_server PUBLIC c_std_11)


#
#
# Test
#
#
add_executable(test_config_c config.c)
target_compile_definitions(test_config_c PRIVATE TEST)
add_test(Test1 test_config_c)

#add_executable(test_parse_request_c core/parse_request.c utility/utility.c)
#target_compile_definitions(test_parse_request_c PRIVATE TEST)
#add_test(Test1 test_parse_request_c)

