cmake_minimum_required(VERSION 3.7.2)

add_subdirectory(driver)
add_subdirectory(framework)
add_subdirectory(pal)
add_subdirectory(project)
add_subdirectory(startup)

find_package(Boost REQUIRED)

add_executable(${PROJECT_NAME}
    ${CMAKE_CURRENT_SOURCE_DIR}/main/HomeBrainVx01.cpp
    )

target_link_libraries(${PROJECT_NAME}
    startup
    )