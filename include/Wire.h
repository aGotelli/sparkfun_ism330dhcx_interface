#pragma once

#include <iostream>
#include <vector>
#include <queue>
#include <memory>

#ifdef _WIN32
    #include <windows.h>
    #include <string>
#elif defined(__linux__)
    #include <fcntl.h>
    #include <unistd.h>
    #include <sys/ioctl.h>
    #include <linux/i2c-dev.h>
#endif

#ifdef _WIN32
// CH341 Wrapper class for Windows
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
    CH341Wrapper() : hDLL(nullptr), 
        CH341OpenDevice(nullptr), CH341CloseDevice(nullptr),
        CH341GetVersion(nullptr), CH341SetStream(nullptr),
        CH341ReadI2C(nullptr), CH341WriteI2C(nullptr),
        CH341StreamSPI4(nullptr), CH341StreamSPI5(nullptr),
        CH341SetOutput(nullptr), CH341GetInput(nullptr),
        CH341ReadData(nullptr), CH341WriteData(nullptr) {
    }

    ~CH341Wrapper() {
        UnloadDLL();
    }
    
    bool LoadDLL(const std::string& dllPath = "CH341DLLA64.dll") {
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
    
    void UnloadDLL() {
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
    
    bool IsLoaded() const { return hDLL != nullptr; }
    
    // Wrapper functions
    bool OpenDevice(unsigned long index) {
        if (!CH341OpenDevice) return false;
        return CH341OpenDevice(index);
    }
    
    void CloseDevice(unsigned long index) {
        if (CH341CloseDevice) {
            CH341CloseDevice(index);
        }
    }
    
    unsigned long GetVersion() {
        if (!CH341GetVersion) return 0;
        return CH341GetVersion();
    }
    
    unsigned long GetDeviceCount() {
        if (!CH341GetDeviceCount) return 0;
        return CH341GetDeviceCount();
    }
    
    bool SetStream(unsigned long index, unsigned long mode) {
        if (!CH341SetStream) return false;
        return CH341SetStream(index, mode);
    }
    
    bool ReadI2C(unsigned long index, unsigned char device, unsigned char addr, unsigned char* buffer, unsigned long length) {
        if (!CH341ReadI2C) return false;
        return CH341ReadI2C(index, device, addr, buffer, length);
    }
    
    bool WriteI2C(unsigned long index, unsigned char device, unsigned char addr, unsigned char* buffer, unsigned long length) {
        if (!CH341WriteI2C) return false;
        return CH341WriteI2C(index, device, addr, buffer, length);
    }
    
    bool StreamSPI4(unsigned long index, unsigned long chipSelect, unsigned long length, void* ioBuffer) {
        if (!CH341StreamSPI4) return false;
        return CH341StreamSPI4(index, chipSelect, length, ioBuffer);
    }
    
    bool StreamSPI5(unsigned long index, unsigned long chipSelect, unsigned long length, void* iBuffer, void* oBuffer) {
        if (!CH341StreamSPI5) return false;
        return CH341StreamSPI5(index, chipSelect, length, iBuffer, oBuffer);
    }
    
    bool SetOutput(unsigned long index, unsigned long enable, unsigned long setDirOut, unsigned long setDataOut) {
        if (!CH341SetOutput) return false;
        return CH341SetOutput(index, enable, setDirOut, setDataOut);
    }
    
    bool GetInput(unsigned long index, unsigned long* status) {
        if (!CH341GetInput) return false;
        return CH341GetInput(index, status);
    }
    
    bool ReadData(unsigned long index, void* buffer, unsigned long* length) {
        if (!CH341ReadData) return false;
        return CH341ReadData(index, buffer, length);
    }
    
    bool WriteData(unsigned long index, void* buffer, unsigned long length) {
        if (!CH341WriteData) return false;
        return CH341WriteData(index, buffer, length);
    }
};
#endif // _WIN32

// TwoWire class with platform-specific implementations
class TwoWire {
public:
#ifdef _WIN32
    TwoWire(const char* device = "CH341") : deviceName(device) {}

    ~TwoWire() {
        end();
    }

    void begin() {
        if (!ch341) {
            ch341 = std::make_unique<CH341Wrapper>();
        }
        
        if (!ch341->LoadDLL()) {
            std::cerr << "Failed to load CH341 DLL" << std::endl;
            return;
        }
        
        if (!ch341->OpenDevice(0)) {
            std::cerr << "Failed to open CH341 device 0" << std::endl;
            return;
        }
        
        if (!ch341->SetStream(0, 1)) { // Set I2C mode
            std::cerr << "Failed to set I2C mode" << std::endl;
            return;
        }
        
        isInitialized = true;
        txBuffer.clear();
        while (!rxBuffer.empty()) rxBuffer.pop();
    }

    void end() {
        if (ch341) {
            ch341->CloseDevice(0);
            ch341.reset();
        }
        isInitialized = false;
        txBuffer.clear();
        while (!rxBuffer.empty()) rxBuffer.pop();
    }

    void beginTransmission(uint8_t address) {
        targetAddress = address;
        txBuffer.clear();
    }

    void write(uint8_t data) {
        txBuffer.push_back(data);
    }

    void write(const uint8_t *data, int length) {
        txBuffer.insert(txBuffer.end(), data, data + length);
    }

    int endTransmission(bool stop = true) {
        if (!isInitialized || !ch341) return -1;
        
        if (txBuffer.empty()) return 0;
        
        // For I2C write, first byte is usually the register address
        if (txBuffer.size() == 1) {
            // Just setting register address - store it for later use in requestFrom
            lastRegisterAddress = txBuffer[0];
            return 0;
        } else if (txBuffer.size() >= 2) {
            // Write data to register
            unsigned char regAddr = txBuffer[0];
            std::vector<unsigned char> data(txBuffer.begin() + 1, txBuffer.end());
            
            bool success = ch341->WriteI2C(0, targetAddress, regAddr, data.data(), static_cast<unsigned long>(data.size()));
            return success ? 0 : -1;
        }
        
        return 0;
    }

    uint16_t requestFrom(uint8_t address, uint8_t numBytes, bool stop = true) {
        if (!isInitialized || !ch341) return 0;
        
        // Debug for gyro data reads (register 0x22, 6 bytes)
        bool isGyroRead = (lastRegisterAddress == 0x22 && numBytes == 6);
        
        // Only debug WHO_AM_I register reads
        if (lastRegisterAddress == 0x0F) {
            std::cout << "[DEBUG] Reading WHO_AM_I from addr 0x" << std::hex << (int)address << std::dec << std::endl;
        }
        
        std::vector<unsigned char> buffer(numBytes);
        
        // Normal single read for other registers
        // TODO: Check to see if this last register approach is sound 
        bool success = ch341->ReadI2C(0, address, lastRegisterAddress, buffer.data(), numBytes);
        if (!success) return 0;
        
        if (lastRegisterAddress == 0x0F) {
            std::cout << "[DEBUG] WHO_AM_I result: SUCCESS";
            if (numBytes > 0) {
                std::cout << " = 0x" << std::hex << (int)buffer[0] << std::dec;
            }
            std::cout << std::endl;
        }
        
        // Push received data into RX queue
        for (int i = 0; i < numBytes; i++) {
            rxBuffer.push(buffer[i]);
        }
        
        return numBytes;
    }

    uint8_t read() {
        if (rxBuffer.empty()) {
            return 0xFF; // mimic Arduino: return -1, but cast to uint8_t
        }
        uint8_t b = rxBuffer.front();
        rxBuffer.pop();
        return b;
    }

    int available() const {
        return static_cast<int>(rxBuffer.size());
    }

private:
    std::string deviceName;
    std::unique_ptr<CH341Wrapper> ch341;
    bool isInitialized = false;
    uint8_t targetAddress = 0;
    uint8_t lastRegisterAddress = 0;
    std::vector<uint8_t> txBuffer;
    std::queue<uint8_t> rxBuffer;

#elif defined(__linux__)
    TwoWire(const char* device = "/dev/i2c-16") : devicePath(device) {}

    ~TwoWire() {
        if (fd >= 0) {
            close(fd);
        }
    }

    void begin() {
        if (fd >= 0) close(fd);  // close if previously open
        fd = open(devicePath.c_str(), O_RDWR);
        if (fd < 0) {
            perror("Failed to open I2C device");
        }
        txBuffer.clear();
        while (!rxBuffer.empty()) rxBuffer.pop();
    }

    void end() {
        if (fd >= 0) {
            close(fd);
            fd = -1;
        }
        txBuffer.clear();
        while (!rxBuffer.empty()) rxBuffer.pop();
    }

    void beginTransmission(uint8_t address) {
        targetAddress = address;
        txBuffer.clear();
    }

    void write(uint8_t data) {
        txBuffer.push_back(data);
    }

    void write(const uint8_t *data, int length) {
        txBuffer.insert(txBuffer.end(), data, data + length);
    }

    int endTransmission(bool stop = true) {
        if (fd < 0) return -1;
        if (ioctl(fd, I2C_SLAVE, targetAddress) < 0) {
            perror("Failed to set I2C address");
            return -1;
        }

        ssize_t written = ::write(fd, txBuffer.data(), txBuffer.size());
        if (written != (ssize_t)txBuffer.size()) {
            perror("Failed to write all bytes");
            return -1;
        }
        return 0; // success
    }

    uint16_t requestFrom(uint8_t address, uint8_t numBytes, bool stop = true) {
        if (fd < 0) return 0;
        if (ioctl(fd, I2C_SLAVE, address) < 0) {
            perror("Failed to set I2C address");
            return 0;
        }

        std::vector<uint8_t> buf(numBytes);
        ssize_t readBytes = ::read(fd, buf.data(), numBytes);
        if (readBytes < 0) {
            perror("Failed to read");
            return 0;
        }

        // push into RX queue
        for (ssize_t i = 0; i < readBytes; i++) {
            rxBuffer.push(buf[i]);
        }
        return static_cast<uint16_t>(readBytes);
    }

    uint8_t read() {
        if (rxBuffer.empty()) {
            return 0xFF; // mimic Arduino: return -1, but cast to uint8_t
        }
        uint8_t b = rxBuffer.front();
        rxBuffer.pop();
        return b;
    }

    int available() const {
        return static_cast<int>(rxBuffer.size());
    }

private:
    std::string devicePath;
    int fd = -1;
    uint8_t targetAddress = 0;
    std::vector<uint8_t> txBuffer;
    std::queue<uint8_t> rxBuffer;
#endif // __linux__
};