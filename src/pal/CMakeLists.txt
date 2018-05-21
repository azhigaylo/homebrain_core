cmake_minimum_required(VERSION 3.4.0)

project(pal)

message("pal...")

find_package(AutomotiveDlt 2.15)

add_library(${PROJECT_NAME} STATIC
    ${CMAKE_CURRENT_SOURCE_DIR}/common/slog.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/common/utils.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/os_wrapper/CxMutex.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/os_wrapper/CxMutexLocker.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/os_wrapper/CxMutexRecursive.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/os_wrapper/CxQueue.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/os_wrapper/CxSysTimer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/os_wrapper/CxThreadIO.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/os_wrapper/IxDriver.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/os_wrapper/IxRunnable.cpp
    )

target_include_directories(${PROJECT_NAME}
    PUBLIC  ${AutomotiveDlt_INCLUDE_DIR}
    PUBLIC  ${CMAKE_CURRENT_SOURCE_DIR}/common/include
    PUBLIC  ${CMAKE_CURRENT_SOURCE_DIR}/os_wrapper/include
    )

target_link_libraries(${PROJECT_NAME}
    pthread
    rt
    ${AutomotiveDlt_LIBRARY}
    )

