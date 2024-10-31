cmake_minimum_required(VERSION 3.10)

project(Server VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets Sql Network Concurrent) 

add_subdirectory(../libcommon CommonLib)

file(GLOB_RECURSE PROJECT_SOURCES 
        "server/*.cpp"          "server/*.hpp" 
        "server/requests/*.cpp"    "server/requests/*.hpp" 
)

add_executable(Server main.cpp ${PROJECT_SOURCES})

target_include_directories(Server PRIVATE 
        ${CMAKE_SOURCE_DIR}
)

target_link_libraries(Server PRIVATE CommonLib Qt6::Widgets Qt6::Sql Qt6::Network Qt6::Concurrent)