#pragma once

#include <iostream>
#include <vector>
#include <queue>
#include <memory>
#include "ch341_wrapper.h"

class TwoWire {
public:
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
            // Just setting register address - this is handled in requestFrom
            lastRegisterAddress = txBuffer[0];
            return 0;
        } else if (txBuffer.size() >= 2) {
            // Write data to register
            unsigned char regAddr = txBuffer[0];
            std::vector<unsigned char> data(txBuffer.begin() + 1, txBuffer.end());
            
            bool success = ch341->WriteI2C(0, targetAddress, regAddr, data.data(), data.size());
            return success ? 0 : -1;
        }
        
        return 0;
    }

    uint16_t requestFrom(uint8_t address, uint8_t numBytes, bool stop = true) {
        if (!isInitialized || !ch341) return 0;
        
        std::vector<unsigned char> buffer(numBytes);
        bool success = ch341->ReadI2C(0, address, lastRegisterAddress, buffer.data(), numBytes);
        
        if (!success) return 0;
        
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
        return rxBuffer.size();
    }

private:
    std::string deviceName;
    std::unique_ptr<CH341Wrapper> ch341;
    bool isInitialized = false;
    uint8_t targetAddress = 0;
    uint8_t lastRegisterAddress = 0;
    std::vector<uint8_t> txBuffer;
    std::queue<uint8_t> rxBuffer;
};
