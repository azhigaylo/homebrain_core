cmake_minimum_required(VERSION 3.7.2)

#modules
set(usb_source_plugin "mswu_usb_source")
set(net_source_plugin "mswu_net_source")

project(SwuMaster)

if(CMAKE_COMPILER_IS_GNUCXX OR (${CMAKE_CXX_COMPILER_ID} STREQUAL "Clang"))
    enable_cxx_compiler_flag_if_supported(-Werror=all)
    enable_cxx_compiler_flag_if_supported(-Werror=extra)
    enable_cxx_compiler_flag_if_supported(-Werror=pedantic)
    enable_cxx_compiler_flag_if_supported(-Werror=shadow)
    enable_cxx_compiler_flag_if_supported(-Werror=cast-align)
    enable_cxx_compiler_flag_if_supported(-Werror=write-strings)
    enable_cxx_compiler_flag_if_supported(-Werror=conversion)
    enable_cxx_compiler_flag_if_supported(-Werror=float-equal)
elseif(MSVC)
    enable_cxx_compiler_flag_if_supported(-WX)
    enable_cxx_compiler_flag_if_supported(-W2)
else()
    message(FATAL_ERROR "Unknown compiller '${CMAKE_CXX_COMPILER_ID}'")
endif()

add_subdirectory(plugman)
add_subdirectory(modules)
add_subdirectory(manager)

find_package(Boost COMPONENTS program_options REQUIRED)

add_executable(${PROJECT_NAME}
    ${CMAKE_CURRENT_SOURCE_DIR}/MSWUpdate.cpp
    )

target_link_libraries(${PROJECT_NAME} 
    PRIVATE Boost::program_options
    PRIVATE mswu_plugman
    PRIVATE mswu_modules
    PRIVATE mswu_manager
    PRIVATE elogger
    )

install(TARGETS
    ${PROJECT_NAME}
    RUNTIME DESTINATION bin/
    )

use_ipc_api(${PROJECT_NAME} SW_UPDATE)