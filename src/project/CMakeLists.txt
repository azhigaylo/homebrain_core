cmake_minimum_required(VERSION 3.7.2)

project(project)

message("project...")

find_package(Boost REQUIRED)

add_library(${PROJECT_NAME} STATIC
    ${CMAKE_CURRENT_SOURCE_DIR}/interfaces/CxModBusMaster.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/interfaces/CxModBusSlave.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/interfaces/MBusDB.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/processor/CxUsoProcessor.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/provider/CxDataProvider.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/uso/CxLogDev_ExtMod.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/uso/CxLogDev_MA16.cpp
    )

target_include_directories(${PROJECT_NAME}
    PUBLIC  ${CMAKE_CURRENT_SOURCE_DIR}/events
    PUBLIC  ${CMAKE_CURRENT_SOURCE_DIR}/interfaces/include
    PUBLIC  ${CMAKE_CURRENT_SOURCE_DIR}/processor/include
    PUBLIC  ${CMAKE_CURRENT_SOURCE_DIR}/provider/include
    PUBLIC  ${CMAKE_CURRENT_SOURCE_DIR}/uso/include
    )

target_link_libraries(${PROJECT_NAME}
    PRIVATE Boost::boost
    pal
    framework
    driver
    )