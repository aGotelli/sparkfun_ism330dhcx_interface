"""
Advanced test with actual sensor data reading
"""
from wire import TwoWire
import platform
import time

def main():
    print("Testing ISM330DHCX sensor with Python TwoWire API...")
    print(f"Platform: {platform.system()}")
    
    try:
        wire = TwoWire()
        
        if not wire.begin():
            print("Failed to initialize I2C interface")
            return -1
        
        # Try both possible sensor addresses
        sensor_addr = None
        for addr in [0x6A, 0x6B]:
            print(f"\nTrying sensor address 0x{addr:02x}...")
            
            # Read WHO_AM_I register
            wire.begin_transmission(addr)
            wire.write(0x0F)  # WHO_AM_I register
            wire.end_transmission()
            
            if wire.request_from(addr, 1) == 1:
                who_am_i = wire.read()
                print(f"WHO_AM_I: 0x{who_am_i:02x}")
                
                if who_am_i == 0x6B:  # ISM330DHCX identifier
                    sensor_addr = addr
                    print(f"SUCCESS: ISM330DHCX found at address 0x{addr:02x}")
                    break
        
        if sensor_addr is None:
            print("No ISM330DHCX sensor found")
            wire.end()
            return -1
        
        # Configure the sensor for operation
        print("\nConfiguring sensor...")
        
        # Configure accelerometer: CTRL1_XL (0x10)
        # Set ODR = 104 Hz, FS = ±2g, enable all axes
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
        
        # Enable accelerometer and gyroscope: CTRL3_C (0x12)
        wire.begin_transmission(sensor_addr)
        wire.write(0x12)  # CTRL3_C register
        wire.write(0x04)  # 0000 0100 = IF_INC bit set (auto-increment registers)
        wire.end_transmission()
        
        print("Sensor configured successfully")
        print("Waiting for sensor to stabilize...")
        time.sleep(0.1)  # Wait 100ms for sensor to start
        
        # Read accelerometer data
        print("\nReading accelerometer data...")
        
        # Read 6 bytes starting from OUTX_L_A (0x28)
        wire.begin_transmission(sensor_addr)
        wire.write(0x28)  # OUTX_L_A register
        wire.end_transmission()
        
        if wire.request_from(sensor_addr, 6) == 6:
            # Read X, Y, Z acceleration (16-bit each, little endian)
            accel_x_low = wire.read()
            accel_x_high = wire.read()
            accel_y_low = wire.read()
            accel_y_high = wire.read()
            accel_z_low = wire.read()
            accel_z_high = wire.read()
            
            # Combine bytes (little endian)
            accel_x = (accel_x_high << 8) | accel_x_low
            accel_y = (accel_y_high << 8) | accel_y_low
            accel_z = (accel_z_high << 8) | accel_z_low
            
            # Convert to signed 16-bit
            if accel_x > 32767: accel_x -= 65536
            if accel_y > 32767: accel_y -= 65536
            if accel_z > 32767: accel_z -= 65536
            
            print("Accelerometer data (raw):")
            print(f"  X: {accel_x}")
            print(f"  Y: {accel_y}")
            print(f"  Z: {accel_z}")
            
            # Convert to g-force (approximate, depends on full-scale setting)
            # Default is ±2g, so LSB = 0.061 mg
            scale = 0.000061  # g per LSB for ±2g range
            print("Accelerometer data (g-force):")
            print(f"  X: {accel_x * scale:.3f} g")
            print(f"  Y: {accel_y * scale:.3f} g")
            print(f"  Z: {accel_z * scale:.3f} g")
            
        else:
            print("Failed to read accelerometer data")
        
        # Read gyroscope data
        print("\nReading gyroscope data...")
        
        # Read 6 bytes starting from OUTX_L_G (0x22)
        wire.begin_transmission(sensor_addr)
        wire.write(0x22)  # OUTX_L_G register
        wire.end_transmission()
        
        if wire.request_from(sensor_addr, 6) == 6:
            # Read X, Y, Z gyroscope (16-bit each, little endian)
            gyro_x_low = wire.read()
            gyro_x_high = wire.read()
            gyro_y_low = wire.read()
            gyro_y_high = wire.read()
            gyro_z_low = wire.read()
            gyro_z_high = wire.read()
            
            # Combine bytes (little endian)
            gyro_x = (gyro_x_high << 8) | gyro_x_low
            gyro_y = (gyro_y_high << 8) | gyro_y_low
            gyro_z = (gyro_z_high << 8) | gyro_z_low
            
            # Convert to signed 16-bit
            if gyro_x > 32767: gyro_x -= 65536
            if gyro_y > 32767: gyro_y -= 65536
            if gyro_z > 32767: gyro_z -= 65536
            
            print("Gyroscope data (raw):")
            print(f"  X: {gyro_x}")
            print(f"  Y: {gyro_y}")
            print(f"  Z: {gyro_z}")
            
            # Convert to degrees per second (approximate, depends on full-scale setting)
            # Default is ±250 dps, so LSB = 8.75 mdps
            scale = 0.00875  # dps per LSB for ±250 dps range
            print("Gyroscope data (degrees/sec):")
            print(f"  X: {gyro_x * scale:.3f} dps")
            print(f"  Y: {gyro_y * scale:.3f} dps")
            print(f"  Z: {gyro_z * scale:.3f} dps")
            
        else:
            print("Failed to read gyroscope data")
        
        wire.end()
        
    except Exception as e:
        print(f"Exception caught: {e}")
        return -1
    
    print("\nAdvanced sensor test completed")
    return 0

if __name__ == "__main__":
    exit(main())
