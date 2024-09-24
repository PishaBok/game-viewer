cmake_minimum_required(VERSION 3.5)

project(Client LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets Sql Network)

add_subdirectory(../lib CommonLib)

add_executable(Client)

target_sources(Client PRIVATE
        main.cpp
        src/socket.cpp  include/socket.hpp
        src/window.cpp  include/window.hpp
)

target_include_directories(Client PUBLIC 
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(Client PRIVATE CommonLib Qt6::Widgets Qt6::Sql Qt6::Network)