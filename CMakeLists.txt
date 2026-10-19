cmake_minimum_required(VERSION 4.2)
project(quant)

set(CMAKE_CXX_STANDARD 23)

add_executable(quant_client
        networking/include/client.h
        networking/include/socket.h
        networking/client_main.cpp
        networking/src/client.cpp
        networking/src/base.cpp
        networking/include/base.h
)
target_link_libraries(quant_client ws2_32)

add_executable(quant_server
        networking/include/server.h
        networking/include/socket.h
        networking/server_main.cpp
        networking/src/server.cpp
        networking/src/base.cpp
        networking/include/base.h
)
target_link_libraries(quant_server ws2_32)
