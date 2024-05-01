cmake_minimum_required(VERSION 3.15)

# vcpkg initialization
if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
    set(bootstrap_script "${CMAKE_SOURCE_DIR}/vcpkg/bootstrap-vcpkg.bat")
else()
    set(bootstrap_script "${CMAKE_SOURCE_DIR}/vcpkg/bootstrap-vcpkg.sh")
endif()

execute_process(COMMAND "${bootstrap_script}")
set(CMAKE_TOOLCHAIN_FILE "${CMAKE_SOURCE_DIR}/vcpkg/scripts/buildsystems/vcpkg.cmake")

# project definition
project(template-type-deduction)

find_package(boost_type_index REQUIRED CONFIG)

add_executable(${PROJECT_NAME} main.cpp)

target_link_libraries(${PROJECT_NAME} Boost::type_index)