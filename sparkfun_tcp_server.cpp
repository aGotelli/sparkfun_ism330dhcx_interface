#include <filesystem>
#include <iostream>
#include <fstream>
#include <string>
#include <thread>
#include <sstream>
#include <chrono>
#include <memory>

#ifdef _WIN32
    #include <winsock2.h>
    #pragma comment(lib, "ws2_32.lib")
#else
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <unistd.h>
    #define SOCKET int
    #define closesocket close
#endif
#include "gyro.h"

class SparkFunTCPServer {
private:
    int m_frequency { 300 };
    int m_port { 8080 };
    std::unique_ptr<GyroAPI> m_gyro_api;
    std::string m_folder_path;
    double m_duration;
    
public:
    SparkFunTCPServer(int t_frequency) 
    : m_frequency(t_frequency)
    {
#ifdef _WIN32
        WSADATA wsa;
        WSAStartup(MAKEWORD(2,2), &wsa);
#endif

        setup_gyro();
        // unsigned int max_tentatives = 10;
        // for(int it=0; it<max_tentatives; ++it)
        //     if(setup_gyro())
        //         break;
        //     else
        //         std::cout << "No devices detected. Retrying..." << std::endl;
        


        m_gyro_api->setRecord(true, m_frequency);
    }


    
    bool setup_gyro() 
    {
        // Setup will be called when client sends setup command   
#ifdef __linux__
        std::cout << "Using Linux I2C interface" << std::endl;
        GyroAPI gyro_api = GyroAPI(); // Default Linux path: /dev/i2c-16
#elif defined(_WIN32)
        std::cout << "Using Windows CH341 USB-to-I2C interface" << std::endl;
        m_gyro_api = std::make_unique<GyroAPI>(); // Default Windows device: CH341
#endif

        // Try both possible addresses
        m_gyro_api->add_device(ISM330DHCX_ADDRESS_LOW); // Soldered address (0x6A) End effector right now
        m_gyro_api->add_device(ISM330DHCX_ADDRESS_HIGH); // Default (unsoldered) address (0x6B) Middle one right now 

        // Check if any devices were successfully detected
        if (!m_gyro_api->statusCheck()) 
            return false;
        
        return true;

    }
    
    void start() {
        SOCKET server = socket(AF_INET, SOCK_STREAM, 0);
        sockaddr_in addr = {AF_INET, htons(m_port), INADDR_ANY};
        bind(server, (sockaddr*)&addr, sizeof(addr));
        listen(server, 1);
        std::cout << "Server on port " << m_port << std::endl;
        
        while(true) {
            SOCKET client = accept(server, nullptr, nullptr);
            std::cout << "Client connected" << std::endl;
            
            // Set timeout for client socket
#ifdef _WIN32
            DWORD timeout = 1000; // 1 second
            setsockopt(client, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeout, sizeof(timeout));
#else
            struct timeval tv;
            tv.tv_sec = 1;
            tv.tv_usec = 0;
            setsockopt(client, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
#endif
            
            while(true) {
                char buf[1024];
                int bytes = recv(client, buf, 1024, 0);
                if(bytes <= 0) break;
                
                buf[bytes] = '\0';
                std::string cmd(buf);
                
                if(cmd.find("setup") != std::string::npos) {
                    // Parse folder and duration from JSON
                    size_t folder_pos = cmd.find("\"folder\"");
                    size_t duration_pos = cmd.find("\"duration\"");
                    
                    if(folder_pos != std::string::npos) {
                        size_t start = cmd.find(":", folder_pos) + 2;
                        size_t end = cmd.find("\"", start);
                        m_folder_path = cmd.substr(start, end - start);
                    }
                    
                    if(duration_pos != std::string::npos) {
                        size_t start = cmd.find(":", duration_pos) + 1;
                        size_t end = cmd.find_first_of(",}", start);
                        m_duration = std::stod(cmd.substr(start, end - start));
                    }
                    
                    send(client, "{\"status\": \"ready\"}", 19, 0);
                } 
                if(cmd.find("start") != std::string::npos) {
                    send(client, "{\"status\": \"Recording started\"}", 32, 0);
                    // Start actual recording
                    if(m_gyro_api && !m_folder_path.empty()) {
                        m_gyro_api->startUpdateLoop(const_cast<char*>(m_folder_path.c_str()));
                    }
                }

                std::this_thread::sleep_for(std::chrono::milliseconds(100));
            }
            closesocket(client);
            std::cout << "Client disconnected" << std::endl;

            std::this_thread::sleep_for(std::chrono::milliseconds(500));
        }
    }
};

int main(int argc, char **argv)
{
    if (argc < 2) {
        std::cout << "Usage: " << argv[0] << " <frequency_hz>" << std::endl;
        return 1;
    }

    int frequency = std::stoi(argv[1]); // Desired frequency in Hz

    SparkFunTCPServer tcp_server(frequency);
    tcp_server.start();
    
    return 0;
}

