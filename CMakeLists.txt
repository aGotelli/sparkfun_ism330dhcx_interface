cmake_minimum_required(VERSION 3.10)
project(sparkfun_ism330dhcx_interface)

set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_STANDARD 20)

# Platform detection
if(WIN32)
    message(STATUS "Building for Windows platform")
    set(PLATFORM "windows")
    add_definitions(-D_WIN32)
elseif(UNIX AND NOT APPLE)
    message(STATUS "Building for Linux platform")
    set(PLATFORM "linux")
    add_definitions(-D__linux__)
else()
    message(FATAL_ERROR "Unsupported platform. Only Windows and Linux are supported.")
endif()

# Include directories
include_directories(${PROJECT_SOURCE_DIR}/include)
include_directories(${PROJECT_SOURCE_DIR}/include/platform/${PLATFORM})
include_directories(${PROJECT_SOURCE_DIR}/include/sensor_api)
include_directories(${PROJECT_SOURCE_DIR}/include/sensor_api/st_src)

# Common source files
file(GLOB COMMON_H_FILES 
    ${PROJECT_SOURCE_DIR}/include/*.h
    ${PROJECT_SOURCE_DIR}/include/sensor_api/*.h
    ${PROJECT_SOURCE_DIR}/include/sensor_api/st_src/*.h
)

file(GLOB COMMON_CPP_FILES 
    ${PROJECT_SOURCE_DIR}/src/sensor_api/*.cpp
    ${PROJECT_SOURCE_DIR}/src/sensor_api/st_src/*.c
)

# Platform-specific source files
file(GLOB PLATFORM_H_FILES 
    ${PROJECT_SOURCE_DIR}/include/platform/${PLATFORM}/*.h
)

file(GLOB PLATFORM_CPP_FILES 
    ${PROJECT_SOURCE_DIR}/src/platform/${PLATFORM}/*.cpp
)

# All source files
set(ALL_H_FILES ${COMMON_H_FILES} ${PLATFORM_H_FILES})
set(ALL_CPP_FILES ${COMMON_CPP_FILES} ${PLATFORM_CPP_FILES})

# Remove problematic files based on platform
if(WIN32)
    # Remove Linux-specific files that cause issues on Windows
    list(FILTER ALL_CPP_FILES EXCLUDE REGEX ".*gyro\\.cpp$")
    list(FILTER ALL_H_FILES EXCLUDE REGEX ".*gyro\\.h$")
endif()

message(STATUS "Platform: ${PLATFORM}")
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
set(BOOST_LIBS date_time system)
find_package(Boost COMPONENTS ${BOOST_LIBS} QUIET)
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