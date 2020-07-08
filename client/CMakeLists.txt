CMAKE_MINIMUM_REQUIRED(VERSION 3.15)
project(client)

set(CMAKE_CXX_STANDARD 14)


include_directories(include ${Boost_INCLUDE_DIRS})
add_executable(client
        src/main.cpp
        src/client.cpp src/confighandler.cpp include/confighandler.h)

target_link_libraries(client
        ${Boost_LIBRARIES}
        ${LIBCONFIGPP_LIBRARY})
