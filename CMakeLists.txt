cmake_minimum_required(VERSION 3.20)
project(ProducerConsumer)

set(CMAKE_CXX_STANDARD 14)

add_executable(ProducerConsumer main.cpp)

find_package(Threads REQUIRED)

target_link_libraries(ProducerConsumer
        ${CMAKE_THREAD_LIBS_INIT}
        )
