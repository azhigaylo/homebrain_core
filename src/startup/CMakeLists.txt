cmake_minimum_required(VERSION 3.4.0)

project(startup)

message("startup...")

#find_package()

add_library(${PROJECT_NAME} STATIC
    ${CMAKE_CURRENT_SOURCE_DIR}/CxLauncher.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/CxUsoCfgLoader.cpp
    )

target_include_directories(${PROJECT_NAME}
    PUBLIC  ${CMAKE_CURRENT_SOURCE_DIR}/include
    )

target_link_libraries(${PROJECT_NAME}
    PUBLIC driver
    PUBLIC framework
    PUBLIC pal
    PUBLIC project
    )