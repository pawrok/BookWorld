cmake_minimum_required(VERSION 3.16.3)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_BUILD_TYPE debug)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++2a -lssl -lcrypto -lz -Llibs -g")

include(ExternalProject)

PROJECT("BookWorld")

find_package(OpenSSL REQUIRED)
find_package(ZLIB)


add_executable(${PROJECT_NAME} src/BookWorld.cpp)
add_subdirectory(src)

ExternalProject_Add(
    uSockets
    SOURCE_DIR ../3rd_party/uWebSockets/uSockets
    CONFIGURE_COMMAND ""
    BUILD_IN_SOURCE 1
    BUILD_COMMAND WITH_OPENSSL=1 make
    INSTALL_COMMAND ""
)

ExternalProject_Add(
    sqlite3
    SOURCE_DIR ../3rd_party/sqlite3
    CONFIGURE_COMMAND ""
    BUILD_IN_SOURCE 1
    BUILD_COMMAND gcc -DSQLITE_THREADSAFE=0 -DSQLITE_OMIT_LOAD_EXTENSION -c sqlite3.c -o ../../build/sqlite3.o
    INSTALL_COMMAND ""
)

target_include_directories(
    ${PROJECT_NAME}
    PUBLIC 
        ${CMAKE_SOURCE_DIR}/src 
        ${CMAKE_SOURCE_DIR}/3rd_party/uWebSockets/uSockets/src
        ${CMAKE_SOURCE_DIR}/3rd_party/uWebSockets/src
)

target_include_directories(
    SRC
    PUBLIC 
        ${CMAKE_SOURCE_DIR}/3rd_party/spdlog/include
        ${CMAKE_SOURCE_DIR}/3rd_party/sqlite3
        ${CMAKE_SOURCE_DIR}/3rd_party/fmt-8.0.1/include
)

target_link_directories(
    ${PROJECT_NAME} 
    PRIVATE src
    # PRIVATE ${CMAKE_SOURCE_DIR}/build
    # PRIVATE ${CMAKE_SOURCE_DIR}/3rd_party/uWebSockets/uSockets
)

# target_link_directories(${PROJECT_NAME} PRIVATE 3rd_party/uWebSockets/uSockets)

target_link_libraries(
    ${PROJECT_NAME}
    SRC
    ${CMAKE_SOURCE_DIR}/build/sqlite3.o
    ${CMAKE_SOURCE_DIR}/3rd_party/uWebSockets/uSockets/socket.o
    ${CMAKE_SOURCE_DIR}/3rd_party/uWebSockets/uSockets/openssl.o
    ${CMAKE_SOURCE_DIR}/3rd_party/uWebSockets/uSockets/loop.o
    ${CMAKE_SOURCE_DIR}/3rd_party/uWebSockets/uSockets/libuv.o
    ${CMAKE_SOURCE_DIR}/3rd_party/uWebSockets/uSockets/gcd.o
    ${CMAKE_SOURCE_DIR}/3rd_party/uWebSockets/uSockets/epoll_kqueue.o
    ${CMAKE_SOURCE_DIR}/3rd_party/uWebSockets/uSockets/context.o
    ${CMAKE_SOURCE_DIR}/3rd_party/uWebSockets/uSockets/bsd.o
    ${CMAKE_SOURCE_DIR}/3rd_party/uWebSockets/uSockets/sni_tree.o
    ${CMAKE_SOURCE_DIR}/3rd_party/uWebSockets/uSockets/wolfssl.o
    OpenSSL::SSL
    ZLIB::ZLIB
)