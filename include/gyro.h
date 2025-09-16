#pragma once

#include <filesystem>
#include <iostream>
#include <fstream>
#include <thread>
#include "Wire.h"
#include "SparkFun_ISM330DHCX.h"

class GyroAPI
{
public:
    GyroAPI(const char *i2c_path = "/dev/i2c-16") : m_wire(i2c_path)
    {
        m_wire.begin();
    }
    ~GyroAPI()
    {
        m_wire.end();
    }

    void add_device(uint8_t address);

    void startUpdateLoop(char *folder_name);
    void stopUpdateLoop();
    void setRecord(bool value, int frequency);
    bool statusCheck();
    void flush();
    void join();

private:
    void gyro_thread();

    uint64_t m_now_time, m_last_time;

    TwoWire m_wire;

    bool m_record = false, m_run_thread = false;
    int m_frequency;

    std::thread m_thread;
    std::vector<int64_t> m_last_times;
    std::vector<SparkFun_ISM330DHCX *> m_devices;
    std::vector<std::ofstream *> m_file_streams;
};