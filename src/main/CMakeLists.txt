cmake_minimum_required(VERSION 3.7.2)

add_subdirectory(src/utils)
add_subdirectory(src/common)
add_subdirectory(src/ctrl_provider)
add_subdirectory(src/data_provider)

project(swu_manager)

find_package(Boost REQUIRED thread system iostreams)

add_library(${PROJECT_NAME} STATIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src/SWUpdateManager.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/UpdateManagerFsm.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/StatusProvider.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ManifestParser.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/DeviceUploader.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/SWUpdateManagerImpl.cpp
    )

target_include_directories(${PROJECT_NAME}
    PUBLIC  ${CMAKE_CURRENT_SOURCE_DIR}/include
    )

target_link_libraries(${PROJECT_NAME}
    PUBLIC Boost::boost
    PUBLIC Boost::thread
    PUBLIC Boost::system
    PUBLIC Boost::iostreams
    PRIVATE elogger
    PRIVATE swu_plugman
    PRIVATE swu_modules
    PRIVATE ${PROJECT_NAME}_utils
    PRIVATE ${PROJECT_NAME}_common
    PRIVATE ${PROJECT_NAME}_ctrl_provider
    PRIVATE ${PROJECT_NAME}_data_provider
    )
    
