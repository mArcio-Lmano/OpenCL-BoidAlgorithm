cmake_minimum_required(VERSION 3.28.3)

project(open_cl)

add_executable(${PROJECT_NAME} main.cpp)

find_package(OpenCL REQUIRED)
target_link_libraries(${PROJECT_NAME} OpenCL::OpenCL)
