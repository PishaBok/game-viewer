cmake_minimum_required(VERSION 3.10)

project(CommonLib VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets Sql Network) 
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Sql Network)

file(GLOB_RECURSE SOURCES 
        "libcommon/*.cpp"          "libcommon/*.hpp" 
        "libcommon/requests/*.cpp" "libcommon/requests/*.hpp"      
)

add_library(CommonLib SHARED ${SOURCES})

target_include_directories(CommonLib PUBLIC 
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(CommonLib Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::Sql Qt${QT_VERSION_MAJOR}::Network)
