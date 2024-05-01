cmake_minimum_required(VERSION 3.15)

project(template-type-deduction)

find_package(boost_type_index REQUIRED CONFIG)

add_executable(${PROJECT_NAME} main.cpp)

target_link_libraries(${PROJECT_NAME} Boost::type_index)