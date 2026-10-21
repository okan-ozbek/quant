cmake_minimum_required(VERSION 4.2)
project(quant)

set(CMAKE_CXX_STANDARD 23)

add_executable(quant_client
        networking/include/observa.h
        networking/include/base.h
        networking/include/client.h
        networking/include/socket.h
        networking/src/observa.cpp
        networking/src/base.cpp
        networking/src/client.cpp
        networking/src/socket.cpp
        networking/client_main.cpp
)
target_link_libraries(quant_client ws2_32)

add_executable(quant_server
        networking/include/observa.h
        networking/include/base.h
        networking/include/server.h
        networking/include/socket.h
        networking/src/observa.cpp
        networking/src/base.cpp
        networking/src/server.cpp
        networking/src/socket.cpp
        networking/server_main.cpp
)
target_link_libraries(quant_server ws2_32)
