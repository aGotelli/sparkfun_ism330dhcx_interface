#include "ch341_wrapper.h"

CH341Wrapper::CH341Wrapper() : hDLL(nullptr), 
    CH341OpenDevice(nullptr), CH341CloseDevice(nullptr),
    CH341GetVersion(nullptr), CH341SetStream(nullptr),
    CH341ReadI2C(nullptr), CH341WriteI2C(nullptr),
    CH341StreamSPI4(nullptr), CH341StreamSPI5(nullptr),
    CH341SetOutput(nullptr), CH341GetInput(nullptr),
    CH341ReadData(nullptr), CH341WriteData(nullptr) {
}

CH341Wrapper::~CH341Wrapper() {
    UnloadDLL();
}

bool CH341Wrapper::LoadDLL(const std::string& dllPath) {
    // Load the DLL
    hDLL = LoadLibraryA(dllPath.c_str());
    if (!hDLL) {
        std::cerr << "Failed to load " << dllPath << ". Error: " << GetLastError() << std::endl;
        return false;
    }
    
    // Get function addresses - only for functions that exist
    CH341OpenDevice = (CH341OpenDevice_t)GetProcAddress(hDLL, "CH341OpenDevice");
    CH341CloseDevice = (CH341CloseDevice_t)GetProcAddress(hDLL, "CH341CloseDevice");
    CH341GetVersion = (CH341GetVersion_t)GetProcAddress(hDLL, "CH341GetVersion");
    CH341SetStream = (CH341SetStream_t)GetProcAddress(hDLL, "CH341SetStream");
    CH341ReadI2C = (CH341ReadI2C_t)GetProcAddress(hDLL, "CH341ReadI2C");
    CH341WriteI2C = (CH341WriteI2C_t)GetProcAddress(hDLL, "CH341WriteI2C");
    CH341StreamSPI4 = (CH341StreamSPI4_t)GetProcAddress(hDLL, "CH341StreamSPI4");
    CH341StreamSPI5 = (CH341StreamSPI5_t)GetProcAddress(hDLL, "CH341StreamSPI5");
    CH341SetOutput = (CH341SetOutput_t)GetProcAddress(hDLL, "CH341SetOutput");
    CH341GetInput = (CH341GetInput_t)GetProcAddress(hDLL, "CH341GetInput");
    CH341ReadData = (CH341ReadData_t)GetProcAddress(hDLL, "CH341ReadData");
    CH341WriteData = (CH341WriteData_t)GetProcAddress(hDLL, "CH341WriteData");
    
    // Check if essential functions were loaded successfully
    if (!CH341OpenDevice || !CH341CloseDevice) {
        std::cerr << "Failed to get essential function addresses from DLL" << std::endl;
        UnloadDLL();
        return false;
    }
    
    std::cout << "CH341 DLL loaded successfully with " << 
                 (CH341OpenDevice ? 1 : 0) + (CH341CloseDevice ? 1 : 0) + 
                 (CH341GetVersion ? 1 : 0) + (CH341SetStream ? 1 : 0) +
                 (CH341ReadI2C ? 1 : 0) + (CH341WriteI2C ? 1 : 0) +
                 (CH341StreamSPI4 ? 1 : 0) + (CH341StreamSPI5 ? 1 : 0) +
                 (CH341SetOutput ? 1 : 0) + (CH341GetInput ? 1 : 0) +
                 (CH341ReadData ? 1 : 0) + (CH341WriteData ? 1 : 0) 
              << " functions available" << std::endl;
    return true;
}

void CH341Wrapper::UnloadDLL() {
    if (hDLL) {
        FreeLibrary(hDLL);
        hDLL = nullptr;
        CH341OpenDevice = nullptr;
        CH341CloseDevice = nullptr;
        CH341GetVersion = nullptr;
        CH341SetStream = nullptr;
        CH341ReadI2C = nullptr;
        CH341WriteI2C = nullptr;
        CH341StreamSPI4 = nullptr;
        CH341StreamSPI5 = nullptr;
        CH341SetOutput = nullptr;
        CH341GetInput = nullptr;
        CH341ReadData = nullptr;
        CH341WriteData = nullptr;
    }
}

bool CH341Wrapper::OpenDevice(unsigned long index) {
    if (!CH341OpenDevice) return false;
    return CH341OpenDevice(index);
}

void CH341Wrapper::CloseDevice(unsigned long index) {
    if (CH341CloseDevice) {
        CH341CloseDevice(index);
    }
}

bool CH341Wrapper::ReadData(unsigned long index, void* buffer, unsigned long* length) {
    if (!CH341ReadData) return false;
    return CH341ReadData(index, buffer, length);
}

bool CH341Wrapper::WriteData(unsigned long index, void* buffer, unsigned long length) {
    if (!CH341WriteData) return false;
    return CH341WriteData(index, buffer, length);
}

unsigned long CH341Wrapper::GetVersion() {
    if (!CH341GetVersion) return 0;
    return CH341GetVersion();
}

bool CH341Wrapper::SetStream(unsigned long index, unsigned long mode) {
    if (!CH341SetStream) return false;
    return CH341SetStream(index, mode);
}

bool CH341Wrapper::ReadI2C(unsigned long index, unsigned char device, unsigned char addr, unsigned char* buffer, unsigned long length) {
    if (!CH341ReadI2C) return false;
    return CH341ReadI2C(index, device, addr, buffer, length);
}

bool CH341Wrapper::WriteI2C(unsigned long index, unsigned char device, unsigned char addr, unsigned char* buffer, unsigned long length) {
    if (!CH341WriteI2C) return false;
    return CH341WriteI2C(index, device, addr, buffer, length);
}

bool CH341Wrapper::StreamSPI4(unsigned long index, unsigned long chipSelect, unsigned long length, void* ioBuffer) {
    if (!CH341StreamSPI4) return false;
    return CH341StreamSPI4(index, chipSelect, length, ioBuffer);
}

bool CH341Wrapper::StreamSPI5(unsigned long index, unsigned long chipSelect, unsigned long length, void* iBuffer, void* oBuffer) {
    if (!CH341StreamSPI5) return false;
    return CH341StreamSPI5(index, chipSelect, length, iBuffer, oBuffer);
}

bool CH341Wrapper::SetOutput(unsigned long index, unsigned long enable, unsigned long setDirOut, unsigned long setDataOut) {
    if (!CH341SetOutput) return false;
    return CH341SetOutput(index, enable, setDirOut, setDataOut);
}

bool CH341Wrapper::GetInput(unsigned long index, unsigned long* status) {
    if (!CH341GetInput) return false;
    return CH341GetInput(index, status);
}
