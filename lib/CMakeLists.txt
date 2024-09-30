cmake_minimum_required(VERSION 3.10)

project(CommonLib VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets Sql Network)

add_library(CommonLib SHARED)

target_sources(CommonLib PRIVATE
        src/table_model.cpp     include/table_model.hpp
        src/functions.cpp       include/functions.hpp
)

target_include_directories(CommonLib PUBLIC 
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(CommonLib PRIVATE Qt6::Widgets Qt6::Sql Qt6::Network)
