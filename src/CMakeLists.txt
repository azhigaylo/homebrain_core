cmake_minimum_required(VERSION 3.7.2)

project(swu VERSION 1.0.0)

# Build swu for target platforms only (i.e. when cross-compiling)
if(NOT CMAKE_CROSSCOMPILING)
    message(WARNING "SWU package is intended for target only, skipping build")
    return()
endif()

add_subdirectory(src/slave)
add_subdirectory(src/master)
add_subdirectory(test)

install(FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/config/swu_master.json
    ${CMAKE_CURRENT_SOURCE_DIR}/config/swu_slave.json
    DESTINATION etc/SWU
    )

if (WITH_ROOTFS)
    install(DIRECTORY
        ${CMAKE_CURRENT_SOURCE_DIR}/deploy/
        DESTINATION ${CMAKE_INSTALL_ROOT}/etc/systemd/system/
        USE_SOURCE_PERMISSIONS
        FILES_MATCHING PATTERN "*"
        )
else (WITH_ROOTFS)
    install(FILES
        ${CMAKE_CURRENT_SOURCE_DIR}/deploy/swu_slave.service
        ${CMAKE_CURRENT_SOURCE_DIR}/deploy/swu_master.service
        DESTINATION etc/systemd/
        )
endif (WITH_ROOTFS)

#must come in the end of main project's CMakeLists
add_package(swu ${CMAKE_CURRENT_SOURCE_DIR}/pkg/CMakeLists.txt.in)

