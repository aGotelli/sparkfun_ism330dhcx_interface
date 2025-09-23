cmake_minimum_required(VERSION 3.10)
project(sparkfun_ism330dhcx_interface)

set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_STANDARD 20)

# Platform detection for informational purposes and platform-specific settings
if(WIN32)
    message(STATUS "Building for Windows platform")
    add_definitions(-D_WIN32)
elseif(UNIX AND NOT APPLE)
    message(STATUS "Building for Linux platform")
    add_definitions(-D__linux__)
else()
    message(FATAL_ERROR "Unsupported platform. Only Windows and Linux are supported.")
endif()

# Include directories - simplified without platform subdirectories
include_directories(${PROJECT_SOURCE_DIR}/include)
include_directories(${PROJECT_SOURCE_DIR}/include/sensor_api)
include_directories(${PROJECT_SOURCE_DIR}/include/sensor_api/st_src)

# All header files
file(GLOB ALL_H_FILES 
    ${PROJECT_SOURCE_DIR}/include/*.h
    ${PROJECT_SOURCE_DIR}/include/sensor_api/*.h
    ${PROJECT_SOURCE_DIR}/include/sensor_api/st_src/*.h
)

# All source files - no platform filtering, preprocessor directives handle platform differences
file(GLOB ALL_CPP_FILES 
    ${PROJECT_SOURCE_DIR}/src/*.cpp
    ${PROJECT_SOURCE_DIR}/src/sensor_api/*.cpp
    ${PROJECT_SOURCE_DIR}/src/sensor_api/st_src/*.c
)

message(STATUS "Header files:")
foreach(file ${ALL_H_FILES})
    message(STATUS "  ${file}")
endforeach()

message(STATUS "Source files:")
foreach(file ${ALL_CPP_FILES})
    message(STATUS "  ${file}")
endforeach()

# Main executable
add_executable(sparkfun_ism330dhcx main.cpp ${ALL_H_FILES} ${ALL_CPP_FILES})

# Optional libraries
find_package(Boost REQUIRED)
if(Boost_FOUND)
    target_link_libraries(sparkfun_ism330dhcx ${Boost_LIBRARIES})
    message(STATUS "Found Boost libraries")
else()
    message(WARNING "Boost libraries not found - building without Boost")
endif()

find_package(Threads REQUIRED)
target_link_libraries(sparkfun_ism330dhcx ${CMAKE_THREAD_LIBS_INIT})

# Platform-specific linking
if(WIN32)
    # Windows: Copy DLL to output directory for runtime
    add_custom_command(TARGET sparkfun_ism330dhcx POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${PROJECT_SOURCE_DIR}/lib/CH341DLLA64.dll"
        $<TARGET_FILE_DIR:sparkfun_ism330dhcx>
    )
endif()

# Test executables
add_subdirectory(tests)