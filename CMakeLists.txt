cmake_minimum_required(VERSION 3.10)
project(client)

set(CMAKE_CXX_STANDARD 17)

add_executable(client main.cpp MessageMap.cpp MessageMap.h WindowClass.cpp WindowClass.h Window.cpp Window.h WinApiFunctions.h WinApiFunctions.cpp MenuClass.cpp MenuClass.h DoubleBuffering.cpp DoubleBuffering.h Template.h DialogClass.cpp DialogClass.h)
if(WIN32)
    target_link_libraries(client wsock32 ws2_32 iphlpapi)
endif()