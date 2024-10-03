cmake_minimum_required(VERSION 3.5.0)
project(sandbox VERSION 0.1.0 LANGUAGES C CXX)
set(CMAKE_CXX_STANDARD 20)

# add_executable(str_sorter main.cpp)
# target_link_libraries(str_sorter ipc)
# target_link_libraries(str_sorter server)
# target_link_libraries(str_sorter client)

file(GLOB ipc_headers ipc/*.hpp)
add_library(ipc STATIC ipc/socket_server.cpp ipc/socket_client.cpp ipc/socket_common.cpp ${ipc_headers})
target_include_directories(ipc PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/ipc)

add_executable(ipc_test ipc/test.cpp)
target_link_libraries(ipc_test ipc)

file(GLOB server_headers server/*.hpp)
add_library(server STATIC ${server_sources} ${server_headers})
target_include_directories(server PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/server)
target_link_libraries(server ipc)

add_library(client STATIC client/str_client.cpp client/str_client.hpp)
target_include_directories(client PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/client)
target_link_libraries(client ipc)