cmake_minimum_required(VERSION 3.4.0)

project(framework)

message("framework...")

#find_package()

add_library(${PROJECT_NAME} STATIC
     ${CMAKE_CURRENT_SOURCE_DIR}/devctrl/CxLogDevice.cpp
     ${CMAKE_CURRENT_SOURCE_DIR}/devctrl/CxLogDeviceManager.cpp
     ${CMAKE_CURRENT_SOURCE_DIR}/eventpool/CxEventDispatcher.cpp
     ${CMAKE_CURRENT_SOURCE_DIR}/eventpool/IxEventConsumer.cpp
     ${CMAKE_CURRENT_SOURCE_DIR}/eventpool/IxEventProducer.cpp
     ${CMAKE_CURRENT_SOURCE_DIR}/interface/CxInterface.cpp
     ${CMAKE_CURRENT_SOURCE_DIR}/interface/CxInterfaceManager.cpp
     ${CMAKE_CURRENT_SOURCE_DIR}/parser/CxIniFileParser.cpp
     ${CMAKE_CURRENT_SOURCE_DIR}/staticpool/CxStaticPool.cpp
    )

target_include_directories(${PROJECT_NAME}
    PUBLIC  ${CMAKE_CURRENT_SOURCE_DIR}/devctrl/include
    PUBLIC  ${CMAKE_CURRENT_SOURCE_DIR}/eventpool/include
    PUBLIC  ${CMAKE_CURRENT_SOURCE_DIR}/interface/include
    PUBLIC  ${CMAKE_CURRENT_SOURCE_DIR}/parser/include
    PUBLIC  ${CMAKE_CURRENT_SOURCE_DIR}/staticpool/include
    )

target_link_libraries(${PROJECT_NAME}
    pal
    )
