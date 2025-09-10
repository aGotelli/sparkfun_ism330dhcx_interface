cmake_minimum_required(VERSION 3.1)
project(sparkfun_ism330dhcx_interface)

set(CMAKE_BUILD_TYPE Debug)

## Target
set(CMAKE_CXX_STANDARD 20)

include_directories(${PROJECT_SOURCE_DIR}/include)
include_directories(${PROJECT_SOURCE_DIR}/include/sensor_api)
include_directories(${PROJECT_SOURCE_DIR}/include/sensor_api/st_src)

file(GLOB_RECURSE H_FILES 
    ${PROJECT_SOURCE_DIR}/include/*.h
)
file(GLOB_RECURSE CPP_FILES 
    ${PROJECT_SOURCE_DIR}/src/*.cpp ${PROJECT_SOURCE_DIR}/src/*.c
)

message(STATUS "Header files:")
foreach(file ${H_FILES})
    message(STATUS "  ${file}")
endforeach()

message(STATUS "Source files:")
foreach(file ${CPP_FILES})
    message(STATUS "  ${file}")
endforeach()

add_executable(sparkfun_ism330dhcx main.cpp ${H_FILES} ${CPP_FILES})

## Link libraries
set(BOOST_LIBS date_time system)
find_package(Boost COMPONENTS ${BOOST_LIBS} REQUIRED)
target_link_libraries(sparkfun_ism330dhcx ${Boost_LIBRARIES})
find_package(Threads REQUIRED)
target_link_libraries(sparkfun_ism330dhcx ${CMAKE_THREAD_LIBS_INIT})