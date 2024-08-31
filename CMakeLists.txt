cmake_minimum_required(VERSION 3.5.0)
project(sandbox VERSION 0.1.0 LANGUAGES C CXX)
set(CMAKE_CXX_STANDARD 20)

add_executable(deepwsl main.cpp)
target_link_libraries(deepwsl ipc)
# target_link_libraries(deepwsl server)
target_link_libraries(deepwsl client)

add_library(ipc STATIC ipc/ipc_server_socket.cpp ipc/ipc_server_socket.hpp)
target_include_directories(ipc PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/ipc)

# add_library(server STATIC server/requests_handler.cpp server/requests_handler.hpp)
# target_include_directories(server PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/server)
# target_link_libraries(server ipc)

add_library(client STATIC client/simple_client.cpp client/simple_client.hpp)
target_include_directories(client PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/client)
target_link_libraries(client ipc)
# target_link_libraries(client server)