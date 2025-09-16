# Tests CMakeLists.txt

# Test executables are platform-specific
if(WIN32)
    # Windows tests - uses CH341 wrapper
    add_executable(test_ch341 test_ch341.cpp ${ALL_CPP_FILES})
    if(Boost_FOUND)
        target_link_libraries(test_ch341 ${Boost_LIBRARIES})
    endif()
    target_link_libraries(test_ch341 ${CMAKE_THREAD_LIBS_INIT})

    # Copy DLL for test executable
    add_custom_command(TARGET test_ch341 POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${PROJECT_SOURCE_DIR}/lib/CH341DLLA64.dll"
        $<TARGET_FILE_DIR:test_ch341>
    )

    # Unified API test
    add_executable(test_unified_api test_unified_api.cpp ${ALL_CPP_FILES})
    if(Boost_FOUND)
        target_link_libraries(test_unified_api ${Boost_LIBRARIES})
    endif()
    target_link_libraries(test_unified_api ${CMAKE_THREAD_LIBS_INIT})

    # Copy DLL for unified API test
    add_custom_command(TARGET test_unified_api POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${PROJECT_SOURCE_DIR}/lib/CH341DLLA64.dll"
        $<TARGET_FILE_DIR:test_unified_api>
    )

    # Function discovery test
    add_executable(test_dll_functions test_dll_functions.cpp)
    
    # Copy DLL for function test executable
    add_custom_command(TARGET test_dll_functions POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${PROJECT_SOURCE_DIR}/lib/CH341DLLA64.dll"
        $<TARGET_FILE_DIR:test_dll_functions>
    )

    # Simple unified test for debugging
    add_executable(test_simple_unified test_simple_unified.cpp ${ALL_CPP_FILES})
    if(Boost_FOUND)
        target_link_libraries(test_simple_unified ${Boost_LIBRARIES})
    endif()
    target_link_libraries(test_simple_unified ${CMAKE_THREAD_LIBS_INIT})

    # Copy DLL for simple unified test
    add_custom_command(TARGET test_simple_unified POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${PROJECT_SOURCE_DIR}/lib/CH341DLLA64.dll"
        $<TARGET_FILE_DIR:test_simple_unified>
    )

elseif(UNIX AND NOT APPLE)
    # Linux tests - uses native I2C
    add_executable(test_ch341 test_ch341.cpp ${ALL_CPP_FILES})
    if(Boost_FOUND)
        target_link_libraries(test_ch341 ${Boost_LIBRARIES})
    endif()
    target_link_libraries(test_ch341 ${CMAKE_THREAD_LIBS_INIT})

    # Unified API test
    add_executable(test_unified_api test_unified_api.cpp ${ALL_CPP_FILES})
    if(Boost_FOUND)
        target_link_libraries(test_unified_api ${Boost_LIBRARIES})
    endif()
    target_link_libraries(test_unified_api ${CMAKE_THREAD_LIBS_INIT})

    # Note: test_dll_functions is Windows-specific, so not built on Linux
endif()

message(STATUS "Test executables configured for platform: ${PLATFORM}")
