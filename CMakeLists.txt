cmake_minimum_required(VERSION 3.25)
project(LucidLogging)


set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(APP_NAME logging_app)

find_package(arena_sdk QUIET)

if(NOT ${arena_sdk_FOUND})
    include("${CMAKE_CURRENT_SOURCE_DIR}/cmake/Findarena_sdk.cmake")
endif()

add_executable(${APP_NAME} src/main.cpp)

target_include_directories(
        ${APP_NAME}
        PRIVATE ${arena_sdk_INCLUDES}
        ${OpenCV_INCLUDE_DIRS}
        )

target_link_libraries(
        ${APP_NAME}
        ${OpenCV_LIBRARIES}
        ${arena_sdk_LIBRARIES}
        )       

