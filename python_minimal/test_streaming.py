"""
Real-time streaming test for ISM330DHCX sensor
"""
from wire import TwoWire
import platform
import time
import sys

def configure_sensor(wire, sensor_addr):
    """Configure the ISM330DHCX sensor for operation"""
    print("Configuring sensor...")
    
    # Reset the sensor first: CTRL3_C (0x12)
    wire.begin_transmission(sensor_addr)
    wire.write(0x12)  # CTRL3_C register
    wire.write(0x01)  # SW_RESET bit
    wire.end_transmission()
    time.sleep(0.01)  # Wait for reset
    
    # Configure accelerometer: CTRL1_XL (0x10)
    # Set ODR = 104 Hz, FS = ±2g
    wire.begin_transmission(sensor_addr)
    wire.write(0x10)  # CTRL1_XL register
    wire.write(0x40)  # 0100 0000 = 104 Hz ODR, ±2g full scale
    wire.end_transmission()
    
    # Configure gyroscope: CTRL2_G (0x11)
    # Set ODR = 104 Hz, FS = ±250 dps
    wire.begin_transmission(sensor_addr)
    wire.write(0x11)  # CTRL2_G register
    wire.write(0x40)  # 0100 0000 = 104 Hz ODR, ±250 dps full scale
    wire.end_transmission()
    
    # Configure control register: CTRL3_C (0x12)
    # Enable register auto-increment and block data update
    wire.begin_transmission(sensor_addr)
    wire.write(0x12)  # CTRL3_C register
    wire.write(0x44)  # 0100 0100 = IF_INC + BDU bits set
    wire.end_transmission()
    
    print("Sensor configured successfully")
    return True

def read_sensor_data(wire, sensor_addr):
    """Read accelerometer and gyroscope data"""
    accel_data = [0, 0, 0]
    gyro_data = [0, 0, 0]
    
    # Read accelerometer data - try reading each register individually first
    accel_regs = [0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D]  # X_L, X_H, Y_L, Y_H, Z_L, Z_H
    accel_bytes = []
    
    for reg in accel_regs:
        wire.begin_transmission(sensor_addr)
        wire.write(reg)
        wire.end_transmission()
        
        if wire.request_from(sensor_addr, 1) == 1:
            accel_bytes.append(wire.read())
        else:
            print(f"Failed to read accelerometer register 0x{reg:02x}")
            accel_bytes.append(0)
    
    # Combine bytes for accelerometer (little endian)
    for i in range(3):
        low_byte = accel_bytes[i * 2]
        high_byte = accel_bytes[i * 2 + 1]
        
        # Combine bytes (little endian)
        raw_value = (high_byte << 8) | low_byte
        
        # Convert to signed 16-bit
        if raw_value > 32767:
            raw_value -= 65536
        
        accel_data[i] = raw_value
    
    # Read gyroscope data - reading each register individually
    gyro_regs = [0x22, 0x23, 0x24, 0x25, 0x26, 0x27]  # X_L, X_H, Y_L, Y_H, Z_L, Z_H
    gyro_bytes = []
    
    for reg in gyro_regs:
        wire.begin_transmission(sensor_addr)
        wire.write(reg)
        wire.end_transmission()
        
        if wire.request_from(sensor_addr, 1) == 1:
            gyro_bytes.append(wire.read())
        else:
            print(f"Failed to read gyroscope register 0x{reg:02x}")
            gyro_bytes.append(0)
    
    # Combine bytes for gyroscope (little endian)
    for i in range(3):
        low_byte = gyro_bytes[i * 2]
        high_byte = gyro_bytes[i * 2 + 1]
        
        # Combine bytes (little endian)
        raw_value = (high_byte << 8) | low_byte
        
        # Convert to signed 16-bit
        if raw_value > 32767:
            raw_value -= 65536
        
        gyro_data[i] = raw_value
    
    return accel_data, gyro_data

def main():
    print("ISM330DHCX Real-time Streaming Test")
    print(f"Platform: {platform.system()}")
    print("=" * 50)
    
    try:
        wire = TwoWire()
        
        if not wire.begin():
            print("Failed to initialize I2C interface")
            return -1
        
        # Find sensor
        sensor_addr = None
        for addr in [0x6A, 0x6B]:
            wire.begin_transmission(addr)
            wire.write(0x0F)  # WHO_AM_I register
            wire.end_transmission()
            
            if wire.request_from(addr, 1) == 1:
                who_am_i = wire.read()
                if who_am_i == 0x6B:  # ISM330DHCX identifier
                    sensor_addr = addr
                    print(f"ISM330DHCX found at address 0x{addr:02x}")
                    break
        
        if sensor_addr is None:
            print("No ISM330DHCX sensor found")
            wire.end()
            return -1
        
        # Configure sensor
        if not configure_sensor(wire, sensor_addr):
            print("Failed to configure sensor")
            wire.end()
            return -1
        
        print("Waiting for sensor to stabilize...")
        time.sleep(0.5)  # Wait 500ms for sensor to start
        
        print("\n" + "=" * 80)
        print("SENSOR READY - You can now move the sensor!")
        print("Press Ctrl+C to stop streaming")
        print("=" * 80)
        print()
        
        # Streaming loop
        sample_count = 0
        start_time = time.time()
        
        try:
            while True:
                accel_data, gyro_data = read_sensor_data(wire, sensor_addr)
                
                # Convert to physical units
                # Accelerometer: ±2g range, so LSB = 0.061 mg
                accel_scale = 0.000061  # g per LSB
                accel_g = [x * accel_scale for x in accel_data]
                
                # Gyroscope: ±250 dps range, so LSB = 8.75 mdps
                gyro_scale = 0.00875  # dps per LSB
                gyro_dps = [x * gyro_scale for x in gyro_data]
                
                # Calculate elapsed time and sample rate
                elapsed = time.time() - start_time
                sample_count += 1
                sample_rate = sample_count / elapsed if elapsed > 0 else 0
                
                # Format and display data with carriage return for overwriting
                output = (
                    f"Accel: X={accel_g[0]:+7.3f}g Y={accel_g[1]:+7.3f}g Z={accel_g[2]:+7.3f}g | "
                    f"Gyro: X={gyro_dps[0]:+8.2f}°/s Y={gyro_dps[1]:+8.2f}°/s Z={gyro_dps[2]:+8.2f}°/s | "
                    f"Rate: {sample_rate:5.1f}Hz | Samples: {sample_count:6d}"
                )
                
                # Use carriage return to overwrite the line
                print(f"\r{output}", end="", flush=True)
                
                # Small delay to control update rate
                time.sleep(0.05)  # 20Hz display update
                
        except KeyboardInterrupt:
            print("\n\nStreaming stopped by user")
            print(f"Total samples: {sample_count}")
            print(f"Average sample rate: {sample_count / elapsed:.1f} Hz")
        
        wire.end()
        
    except Exception as e:
        print(f"\nException caught: {e}")
        return -1
    
    print("Streaming test completed")
    return 0

if __name__ == "__main__":
    exit(main())
