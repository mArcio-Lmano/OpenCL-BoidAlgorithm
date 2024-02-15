cmake_minimum_required(VERSION 3.28.3)

project(open_cl)

add_executable(${PROJECT_NAME} main.cpp)

# Set the path to SFML installation directory
# set(SFML_DIR /home/talocha/C++/SFML-2.6.1)

# Find SFML
find_package(SFML 2.6.1 COMPONENTS graphics window system  REQUIRED)

# OpenCL
find_package(OpenCL REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE OpenCL::OpenCL)
# SFML 
target_link_libraries(${PROJECT_NAME} PRIVATE sfml-graphics sfml-window sfml-system)
