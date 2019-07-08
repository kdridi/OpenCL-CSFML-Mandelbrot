cmake_minimum_required(VERSION 3.11.2)
project(mandelbrot)

set(CMAKE_C_STANDARD 11)

set(CSFML_FOUND True)
set(CSFML_INCLUDE_DIRS "/usr/local/include")
set(CSFML_LIBRARY_DIRS "/usr/local/lib")
set(CSFML_CFLAGS_OTHER "-Wno-deprecated-declarations")
set(CSFML_LIBRARIES "csfml-graphics")

if(CSFML_FOUND)
    include_directories(${CSFML_INCLUDE_DIRS})
    link_directories(${CSFML_LIBRARY_DIRS})
    add_definitions(${CSFML_CFLAGS_OTHER})
    set(LIBRARIES ${LIBRARIES} ${CSFML_LIBRARIES})
endif()


find_package(OpenCL)
if(OpenCL_FOUND)
    include_directories(${OpenCL_INCLUDE_DIRS})
    link_directories(${OpenCL_LIBRARIES})
    add_definitions("-DCL_SILENCE_DEPRECATION=1")
    set(LIBRARIES ${LIBRARIES} ${OpenCL_LIBRARIES})
endif()

add_executable(${PROJECT_NAME} main.c)
target_link_libraries(${PROJECT_NAME} ${LIBRARIES})