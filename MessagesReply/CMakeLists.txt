project(MessagesReply)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES main.cpp Service.h ../Helpers.h ../MyServices.h)

include_directories(
        /usr/local/include
        /usr/include/dbus-1.0
        /usr/lib/x86_64-linux-gnu/dbus-1.0/include
)

link_directories(
        /usr/local/lib
)

add_executable(MessagesReply ${SOURCE_FILES})

target_link_libraries(MessagesReply
        dbus-cpp
        dbus-1
        pthread
)

