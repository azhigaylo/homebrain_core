cmake_minimum_required(VERSION 3.7.2)

project(startup)

message("startup...")

find_package(Boost REQUIRED)

add_library(${PROJECT_NAME} STATIC
    ${CMAKE_CURRENT_SOURCE_DIR}/CxLauncher.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/CxUsoCfgLoader.cpp
    )

target_include_directories(${PROJECT_NAME}
    PUBLIC  ${CMAKE_CURRENT_SOURCE_DIR}/include
    )

target_link_libraries(${PROJECT_NAME}
    PRIVATE Boost::boost
    PUBLIC driver
    PUBLIC framework
    PUBLIC pal
    PUBLIC project
    )