cmake_minimum_required(VERSION 3.10)

project(Server VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets Sql Network Concurrent) 

add_subdirectory(../lib CommonLib)

add_executable(Server)

target_sources(Server PRIVATE
        main.cpp
        src/listener.cpp            include/listener.hpp
        src/database_manager.cpp    include/database_manager.hpp
        src/request.cpp             include/request.hpp
        src/response.cpp            include/response.hpp
        src/client_handler.cpp      include/client_handler.hpp
        src/request_handler.cpp     include/request_handler.hpp 
        src/log_manager.cpp         include/log_manager.hpp
)

target_include_directories(Server PRIVATE 
        ${CMAKE_SOURCE_DIR}/include
)

target_link_libraries(Server PRIVATE CommonLib Qt6::Widgets Qt6::Sql Qt6::Network Qt6::Concurrent)