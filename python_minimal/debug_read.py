"""
Debug script to test multi-byte I2C reads
"""
from wire import TwoWire
import platform

def debug_i2c_read():
    print("Debug: Testing multi-byte I2C reads")
    print(f"Platform: {platform.system()}")
    
    wire = TwoWire()
    
    if not wire.begin():
        print("Failed to initialize I2C interface")
        return
    
    sensor_addr = 0x6A
    
    # Test 1: Read WHO_AM_I (single byte)
    print("\nTest 1: Single byte read (WHO_AM_I)")
    wire.begin_transmission(sensor_addr)
    wire.write(0x0F)  # WHO_AM_I register
    wire.end_transmission()
    
    if wire.request_from(sensor_addr, 1) == 1:
        who_am_i = wire.read()
        print(f"WHO_AM_I: 0x{who_am_i:02x}")
    else:
        print("Failed to read WHO_AM_I")
        wire.end()
        return
    
    # Configure sensor first
    print("\nConfiguring sensor...")
    
    # Configure accelerometer: CTRL1_XL (0x10)
    wire.begin_transmission(sensor_addr)
    wire.write(0x10)  # CTRL1_XL register
    wire.write(0x40)  # 104 Hz ODR, ±2g
    wire.end_transmission()
    
    # Configure gyroscope: CTRL2_G (0x11)
    wire.begin_transmission(sensor_addr)
    wire.write(0x11)  # CTRL2_G register
    wire.write(0x40)  # 104 Hz ODR, ±250 dps
    wire.end_transmission()
    
    # Enable register auto-increment: CTRL3_C (0x12)
    wire.begin_transmission(sensor_addr)
    wire.write(0x12)  # CTRL3_C register
    wire.write(0x44)  # IF_INC + BDU bits set
    wire.end_transmission()
    
    import time
    time.sleep(0.1)  # Wait for sensor to start
    
    # Test 2: Read accelerometer data byte by byte
    print("\nTest 2: Reading accelerometer data byte by byte")
    accel_bytes = []
    for i in range(6):
        reg_addr = 0x28 + i  # OUTX_L_A to OUTZ_H_A
        
        wire.begin_transmission(sensor_addr)
        wire.write(reg_addr)
        wire.end_transmission()
        
        if wire.request_from(sensor_addr, 1) == 1:
            byte_val = wire.read()
            accel_bytes.append(byte_val)
            print(f"  Reg 0x{reg_addr:02x}: 0x{byte_val:02x} ({byte_val})")
        else:
            print(f"  Failed to read reg 0x{reg_addr:02x}")
            break
    
    if len(accel_bytes) == 6:
        # Combine bytes
        accel_x = (accel_bytes[1] << 8) | accel_bytes[0]
        accel_y = (accel_bytes[3] << 8) | accel_bytes[2]
        accel_z = (accel_bytes[5] << 8) | accel_bytes[4]
        
        # Convert to signed
        if accel_x > 32767: accel_x -= 65536
        if accel_y > 32767: accel_y -= 65536
        if accel_z > 32767: accel_z -= 65536
        
        print(f"  Combined: X={accel_x}, Y={accel_y}, Z={accel_z}")
    
    # Test 3: Read 6 bytes at once
    print("\nTest 3: Reading 6 bytes at once")
    wire.begin_transmission(sensor_addr)
    wire.write(0x28)  # OUTX_L_A register
    wire.end_transmission()
    
    bytes_read = wire.request_from(sensor_addr, 6)
    print(f"Bytes requested: 6, Bytes received: {bytes_read}")
    print(f"Available bytes in buffer: {wire.available()}")
    
    if bytes_read == 6:
        multi_bytes = []
        for i in range(6):
            if wire.available() > 0:
                byte_val = wire.read()
                multi_bytes.append(byte_val)
                print(f"  Byte {i}: 0x{byte_val:02x} ({byte_val})")
            else:
                print(f"  Byte {i}: No data available")
                break
        
        if len(multi_bytes) == 6:
            # Combine bytes
            accel_x = (multi_bytes[1] << 8) | multi_bytes[0]
            accel_y = (multi_bytes[3] << 8) | multi_bytes[2]
            accel_z = (multi_bytes[5] << 8) | multi_bytes[4]
            
            # Convert to signed
            if accel_x > 32767: accel_x -= 65536
            if accel_y > 32767: accel_y -= 65536
            if accel_z > 32767: accel_z -= 65536
            
            print(f"  Combined: X={accel_x}, Y={accel_y}, Z={accel_z}")
    
    wire.end()
    print("\nDebug test completed")

if __name__ == "__main__":
    debug_i2c_read()
