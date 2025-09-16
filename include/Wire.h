#pragma once

// Platform detection and include appropriate implementation
#ifdef _WIN32
    // Windows platform - use CH341 USB-to-I2C interface
    #include "platform/windows/TwoWire_windows.h"
#elif defined(__linux__)
    // Linux platform - use native I2C interface
    #include "platform/linux/TwoWire_linux.h"
#else
    #error "Unsupported platform. Only Windows and Linux are supported."
#endif

// The TwoWire class is defined in the platform-specific headers above
// This ensures a consistent API across platforms while using different implementations