CMAKE_MINIMUM_REQUIRED(VERSION 3.15)
project(client)

set(CMAKE_CXX_STANDARD 14)

find_package(Boost COMPONENTS system coroutine filesystem REQUIRED)
FIND_LIBRARY(LIBCONFIGPP_LIBRARY config++
        /usr/local/lib
        /usr/lib
        )

include_directories(include ${Boost_INCLUDE_DIRS})
add_executable(client
        src/main.cpp
        src/client.cpp src/confighandler.cpp include/confighandler.h)

target_link_libraries(client
        ${Boost_LIBRARIES}
        ${LIBCONFIGPP_LIBRARY})
