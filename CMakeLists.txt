cmake_minimum_required(VERSION 3.7.2)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

# Contains definitions
include(HealthController)
 
project(${HEALTH_CONTROLLER_APPLICATION} VERSION 2.2.0)

if(CMAKE_COMPILER_IS_GNUCXX OR (${CMAKE_CXX_COMPILER_ID} MATCHES "Clang$"))
    enable_cxx_compiler_flag_if_supported(-Werror=all)
    enable_cxx_compiler_flag_if_supported(-Werror=extra)
    enable_cxx_compiler_flag_if_supported(-Werror=pedantic)
    enable_cxx_compiler_flag_if_supported(-Werror=shadow)
    enable_cxx_compiler_flag_if_supported(-Werror=cast-align)
    enable_cxx_compiler_flag_if_supported(-Werror=write-strings)
    enable_cxx_compiler_flag_if_supported(-Werror=conversion)
    enable_cxx_compiler_flag_if_supported(-Werror=float-equal)

    if(CMAKE_COMPILER_IS_GNUCXX)
        enable_cxx_compiler_flag_if_supported(-Werror=suggest-override)
    elseif(${CMAKE_CXX_COMPILER_ID} MATCHES "Clang$")
        enable_cxx_compiler_flag_if_supported(-Werror=inconsistent-missing-override)
    endif()

elseif(MSVC)
    enable_cxx_compiler_flag_if_supported(-WX)
    enable_cxx_compiler_flag_if_supported(-W2)
else()
    message(FATAL_ERROR "Unknown compiller '${CMAKE_CXX_COMPILER_ID}'")
endif()

set(CMAKE_CXX_VISIBILITY_PRESET hidden)

add_subdirectory(src)

if (WITH_ROOTFS)
    install(DIRECTORY
        ${CMAKE_CURRENT_SOURCE_DIR}/deploy/
        DESTINATION ${CMAKE_INSTALL_ROOT}/etc/systemd/system/
        USE_SOURCE_PERMISSIONS
        FILES_MATCHING PATTERN "*"
        )
else (WITH_ROOTFS)
    install(FILES
        ${CMAKE_CURRENT_SOURCE_DIR}/deploy/healthctrl.service
        DESTINATION etc/systemd/
        )
endif (WITH_ROOTFS)

#must come in the end of main project's CMakeLists
add_package(${PROJECT_NAME} ${CMAKE_CURRENT_SOURCE_DIR}/pkg/CMakeLists.txt.in)
