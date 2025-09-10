#include "gyro.h"

#include <fstream>
int main(int argc, char **argv)
{
	std::filesystem::path log_folder_path = std::filesystem::path(argv[1]);
	if (!std::filesystem::exists(log_folder_path))
		std::filesystem::create_directory(log_folder_path);
	else
		std::cout << "[WARNING] Log folder already exists. Data will be appended to existing files.\n";

  std::cout << "Initializing gyro...\n";
  GyroAPI gyro_api = GyroAPI();
  gyro_api.add_device(ISM330DHCX_ADDRESS_LOW); // Soldered address
  gyro_api.add_device(ISM330DHCX_ADDRESS_HIGH); // Default (unsoldered) address

  int frequency = std::stoi(argv[2]); // Desired frequency in Hz
  gyro_api.setRecord(true, frequency);
  gyro_api.startUpdateLoop(argv[1]);
  std::cout << "Started recording. Press Enter to stop.\n";

  std::cin.get();
  gyro_api.stopUpdateLoop(); 
	return 0; 
}
