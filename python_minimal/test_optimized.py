"""
Optimized real-time streaming for ISM330DHCX sensor
Focus: Maximum performance with clean, simple code
"""
from wire import TwoWire
import time
import sys

def main():
    print("ISM330DHCX High-Performance Streaming")
    print("=====================================")
    
    # Initialize
    wire = TwoWire()
    wire.begin()
    
    sensor_addr = 0x6A
    
    # Quick sensor check
    wire.begin_transmission(sensor_addr)
    wire.write(0x0F)  # WHO_AM_I
    wire.end_transmission()
    wire.request_from(sensor_addr, 1)
    if wire.read() != 0x6B:
        print("Sensor not found")
        return
    
    # Configure sensor - minimal setup
    wire.begin_transmission(sensor_addr)
    wire.write(0x10); wire.write(0x60)  # CTRL1_XL: 416 Hz, ±2g
    wire.end_transmission()
    
    wire.begin_transmission(sensor_addr)
    wire.write(0x11); wire.write(0x60)  # CTRL2_G: 416 Hz, ±250 dps
    wire.end_transmission()
    
    wire.begin_transmission(sensor_addr)
    wire.write(0x12); wire.write(0x44)  # CTRL3_C: auto-increment + BDU
    wire.end_transmission()
    
    time.sleep(0.1)  # Sensor startup
    
    print("READY - Move the sensor!")
    print("Press Ctrl+C to stop")
    print()
    
    # Pre-allocate for speed
    accel = [0, 0, 0]
    gyro = [0, 0, 0]
    
    # Scaling factors
    accel_scale = 0.000061  # g per LSB for ±2g
    gyro_scale = 0.00875    # dps per LSB for ±250 dps
    
    sample_count = 0
    start_time = time.time()
    
    try:
        while True:
            # Read accelerometer (6 individual reads - fastest working method)
            for i, reg in enumerate([0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D]):
                wire.begin_transmission(sensor_addr)
                wire.write(reg)
                wire.end_transmission()
                wire.request_from(sensor_addr, 1)
                
                if i % 2 == 0:  # Low byte
                    low = wire.read()
                else:  # High byte
                    high = wire.read()
                    # Combine and convert to signed
                    raw = (high << 8) | low
                    if raw > 32767: raw -= 65536
                    accel[i // 2] = raw * accel_scale
            
            # Read gyroscope
            for i, reg in enumerate([0x22, 0x23, 0x24, 0x25, 0x26, 0x27]):
                wire.begin_transmission(sensor_addr)
                wire.write(reg)
                wire.end_transmission()
                wire.request_from(sensor_addr, 1)
                
                if i % 2 == 0:  # Low byte
                    low = wire.read()
                else:  # High byte
                    high = wire.read()
                    # Combine and convert to signed
                    raw = (high << 8) | low
                    if raw > 32767: raw -= 65536
                    gyro[i // 2] = raw * gyro_scale
            
            # Display (optimized format)
            sample_count += 1
            elapsed = time.time() - start_time
            rate = sample_count / elapsed
            
            print(f"\rA:{accel[0]:+6.2f},{accel[1]:+6.2f},{accel[2]:+6.2f}g "
                  f"G:{gyro[0]:+7.1f},{gyro[1]:+7.1f},{gyro[2]:+7.1f}°/s "
                  f"{rate:5.0f}Hz {sample_count:5d}", end="", flush=True)
            
            # No artificial delay - run as fast as possible
            
    except KeyboardInterrupt:
        elapsed = time.time() - start_time
        print(f"\n\nStopped. Final rate: {sample_count/elapsed:.0f} Hz")
    
    wire.end()

if __name__ == "__main__":
    main()
