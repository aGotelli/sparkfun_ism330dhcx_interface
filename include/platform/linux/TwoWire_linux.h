#pragma once

#include <iostream>
#include <vector>
#include <queue>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>

class TwoWire {
public:
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
        return rxBuffer.size();
    }

private:
    std::string devicePath;
    int fd = -1;
    uint8_t targetAddress = 0;
    std::vector<uint8_t> txBuffer;
    std::queue<uint8_t> rxBuffer;
};
