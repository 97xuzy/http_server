cmake_minimum_required(VERSION 3.5)

project(http_server)

#
#
# Library
# libmpm_B
#
#
set(MPM_B_SRC "B/handle_request.c" "B/worker_process_B.c" "B/mpm_B.c" "mpm.c")

add_library (libmpm_B ${MPM_B_SRC})
target_compile_definitions(libmpm_B PRIVATE MPM_B)
# Link lib
target_link_libraries (libmpm_B LINK_PUBLIC ${CMAKE_THREAD_LIBS_INIT})
# Include Dir
target_include_directories (libmpm_B PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
# stdc++
target_compile_features(libmpm_B PUBLIC c_std_11)


#
#
# Library
# libmpm_A
#
#
set(MPM_A_SRC "A/worker_process_A.c" "A/mpm_A.c" "mpm.c")

add_library (libmpm_A ${MPM_A_SRC})
target_compile_definitions(libmpm_A PRIVATE MPM_A)
# Link lib
target_link_libraries (libmpm_A LINK_PUBLIC ${CMAKE_THREAD_LIBS_INIT})
# Include Dir
target_include_directories (libmpm_A PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
# stdc++
target_compile_features(libmpm_A PUBLIC c_std_11)



