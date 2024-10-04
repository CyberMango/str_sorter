cmake_minimum_required(VERSION 3.5.0)
project(sandbox VERSION 0.1.0 LANGUAGES C CXX)
set(CMAKE_CXX_STANDARD 20)

if(MSVC)
    add_compile_options(/W4 /WX)
else()
    add_compile_options(-Wall -Wextra -Werror -O0)
endif()

add_executable(str_sorter main.cpp)
target_link_libraries(str_sorter ipc server client)

add_executable(project_test test.cpp)
target_link_libraries(project_test ipc server client)

file(GLOB ipc_sources ipc/socket/*.cpp)
file(GLOB ipc_headers ipc/*.hpp ipc/socket/*.hpp)
add_library(ipc STATIC ${ipc_sources} ${ipc_headers})
target_include_directories(ipc PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/ipc)

add_executable(ipc_socket_test ipc/socket/test/test.cpp)
target_link_libraries(ipc_socket_test ipc)
# set_target_properties(ipc_socket_test PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/tests")

file(GLOB db_sources server/db/*.cpp)
file(GLOB db_headers server/db/*.hpp)
add_library(db STATIC ${db_sources} ${server_headers})
target_include_directories(db PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/server/db)

file(GLOB server_sources server/*.cpp)
file(GLOB server_headers server/*.hpp)
add_library(server STATIC ${server_sources} ${server_headers})
target_include_directories(server PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/server)
target_link_libraries(server ipc db)

add_library(client STATIC client/str_client.cpp client/str_client.hpp)
target_include_directories(client PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/client)
target_link_libraries(client ipc)
