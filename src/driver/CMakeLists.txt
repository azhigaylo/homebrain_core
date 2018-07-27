cmake_minimum_required(VERSION 3.4.0)

project(driver)

message("drivers...")

#find_package()

add_library(${PROJECT_NAME} STATIC
    ${CMAKE_CURRENT_SOURCE_DIR}/serial/CxSerialDriver.cpp
    )

target_include_directories(${PROJECT_NAME}
    PUBLIC  ${CMAKE_CURRENT_SOURCE_DIR}/serial/include
    )

target_link_libraries(${PROJECT_NAME}
    pal
    )