#pragma once
#include <windows.h>
#include <iostream>
#include <string>

class CH341Wrapper {
private:
    HMODULE hDLL;
    
    // Function pointers - Common CH341 functions (verify with actual DLL exports)
    // Device management
    typedef BOOL (*CH341OpenDevice_t)(ULONG iIndex);
    typedef void (*CH341CloseDevice_t)(ULONG iIndex);
    typedef ULONG (*CH341GetVersion_t)();
    typedef ULONG (*CH341GetDeviceCount_t)();
    
    // I2C functions
    typedef BOOL (*CH341SetStream_t)(ULONG iIndex, ULONG iMode);
    typedef BOOL (*CH341ReadI2C_t)(ULONG iIndex, UCHAR iDevice, UCHAR iAddr, PUCHAR oBuffer, ULONG iLength);
    typedef BOOL (*CH341WriteI2C_t)(ULONG iIndex, UCHAR iDevice, UCHAR iAddr, PUCHAR iBuffer, ULONG iLength);
    
    // SPI functions  
    typedef BOOL (*CH341StreamSPI4_t)(ULONG iIndex, ULONG iChipSelect, ULONG iLength, PVOID ioBuffer);
    typedef BOOL (*CH341StreamSPI5_t)(ULONG iIndex, ULONG iChipSelect, ULONG iLength, PVOID iBuffer, PVOID oBuffer);
    
    // GPIO functions
    typedef BOOL (*CH341SetOutput_t)(ULONG iIndex, ULONG iEnable, ULONG iSetDirOut, ULONG iSetDataOut);
    typedef BOOL (*CH341GetInput_t)(ULONG iIndex, PULONG iStatus);
    
    // Generic data transfer
    typedef BOOL (*CH341ReadData_t)(ULONG iIndex, PVOID oBuffer, PULONG ioLength);
    typedef BOOL (*CH341WriteData_t)(ULONG iIndex, PVOID iBuffer, ULONG iLength);
    
    // Function pointers
    CH341OpenDevice_t CH341OpenDevice;
    CH341CloseDevice_t CH341CloseDevice;
    CH341GetVersion_t CH341GetVersion;
    CH341GetDeviceCount_t CH341GetDeviceCount;
    CH341SetStream_t CH341SetStream;
    CH341ReadI2C_t CH341ReadI2C;
    CH341WriteI2C_t CH341WriteI2C;
    CH341StreamSPI4_t CH341StreamSPI4;
    CH341StreamSPI5_t CH341StreamSPI5;
    CH341SetOutput_t CH341SetOutput;
    CH341GetInput_t CH341GetInput;
    CH341ReadData_t CH341ReadData;
    CH341WriteData_t CH341WriteData;

public:
    CH341Wrapper();
    ~CH341Wrapper();
    
    bool LoadDLL(const std::string& dllPath = "CH341DLLA64.dll");
    void UnloadDLL();
    bool IsLoaded() const { return hDLL != nullptr; }
    
    // Wrapper functions
    bool OpenDevice(unsigned long index);
    void CloseDevice(unsigned long index);
    unsigned long GetVersion();
    unsigned long GetDeviceCount();
    bool SetStream(unsigned long index, unsigned long mode);
    bool ReadI2C(unsigned long index, unsigned char device, unsigned char addr, unsigned char* buffer, unsigned long length);
    bool WriteI2C(unsigned long index, unsigned char device, unsigned char addr, unsigned char* buffer, unsigned long length);
    bool StreamSPI4(unsigned long index, unsigned long chipSelect, unsigned long length, void* ioBuffer);
    bool StreamSPI5(unsigned long index, unsigned long chipSelect, unsigned long length, void* iBuffer, void* oBuffer);
    bool SetOutput(unsigned long index, unsigned long enable, unsigned long setDirOut, unsigned long setDataOut);
    bool GetInput(unsigned long index, unsigned long* status);
    bool ReadData(unsigned long index, void* buffer, unsigned long* length);
    bool WriteData(unsigned long index, void* buffer, unsigned long length);
};
