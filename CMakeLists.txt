cmake_minimum_required(VERSION 3.3)
project(dbtest)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

include_directories(.)

add_subdirectory(SimpleService)
add_subdirectory(MessagesReply)
add_subdirectory(ObjectManaged)
