#include <thread>
#include <chrono>
#include <cstdlib>
#include <libudev.h>
#include <boost/bind/bind.hpp>

#include "gyro.h"

void GyroAPI::startUpdateLoop(char *folder_name)
{
  m_run_thread = true;
  for (unsigned int i = 0; i < m_devices.size(); i++)
  {
    std::filesystem::path log_file_path = std::filesystem::path(folder_name) / std::filesystem::path("sensor" + std::to_string(i) + ".csv");
    std::ofstream *file_stream = new std::ofstream();
    file_stream->open(log_file_path);
    m_file_streams.emplace_back(file_stream);
    *m_file_streams.back() << "time (us),x (mdps),y (mdps),z(mdps)\n";

    m_last_times.push_back(0);
  }
  m_thread = std::thread(boost::bind(&GyroAPI::gyro_thread, this));
}

void GyroAPI::setRecord(bool value, int frequency)
{
  m_record = value;
  m_frequency = frequency;
}

void GyroAPI::add_device(uint8_t address)
{
  SparkFun_ISM330DHCX *new_device = new SparkFun_ISM330DHCX();
  new_device->begin(m_wire, address);
  new_device->setDeviceConfig();
  new_device->setBlockDataUpdate();

  // Set the output data rate and precision of the gyroscope
  new_device->setGyroDataRate(ISM_GY_ODR_6667Hz);
  new_device->setGyroFullScale(ISM_250dps);

  // Turn on the gyroscope's filter and apply settings.
  new_device->setGyroFilterLP1();
  new_device->setGyroLP1Bandwidth(ISM_MEDIUM);
  m_devices.push_back(new_device);

  uint8_t who_am_i = new_device->getUniqueId();
  assert(new_device->getUniqueId() == 0x6b && "Who am I register returned incorrect value. Expected 0x6b.");
  std::cout << "Added device with address 0x" << std::hex << (int)address << std::endl;
  std::cout << "This device will log to sensor" << m_devices.size() - 1 << ".csv" << std::endl;
}

void GyroAPI::flush()
{
  for (auto &stream : m_file_streams)
  {
    stream->flush();
  }
}

void GyroAPI::join()
{
  if (m_thread.joinable())
    m_thread.join();
}

bool GyroAPI::statusCheck()
{
  if (m_devices.size() == 0)
    return false;
  // Check connection status of all devices
  for (auto &device : m_devices)
    if (!device->isConnected())
      return false;
  return true;
}
void GyroAPI::stopUpdateLoop()
{
  m_run_thread = false;
  join();
  flush();
  for (auto &stream : m_file_streams)
  {
    stream->close();
  }
}

void GyroAPI::gyro_thread()
{
  while (m_run_thread)
  {
    for (int index = 0; index < m_devices.size(); index++)
    {
      if (!m_run_thread)
        break;

      // Get current time
      int64_t now_time = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now().time_since_epoch()).count();

      // Save results to file
      if ((1000000000.0 / (now_time - m_last_times[index]) <= m_frequency) && m_record)
      {
        double current_rate = 1000000000.0 / (now_time - m_last_times[index]);
        std::cout << "\rCurrent rate: " << current_rate << " Hz     " << std::flush;
        m_last_times[index] = now_time;

        bool success;

        if (m_devices[index]->checkGyroStatus())
        {
          sfe_ism_data_t gyroData;
          m_devices[index]->getGyro(&gyroData);
          now_time = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now().time_since_epoch()).count();

          *m_file_streams[index] << now_time
                                 << "," << gyroData.xData
                                 << "," << gyroData.yData
                                 << "," << gyroData.zData << "," << std::endl;
        }
        else
        {
          std::cout << "Gyro data not ready. Data will not be logged.\n";
        }
      }
    }
  }
  std::cout << "Gyro thread stopped." << std::endl;
  return;
}
