"""
Minimal working example - Simple I2C interface test
"""
from wire import TwoWire
import platform

def main():
    print("Testing Python TwoWire API...")
    print(f"Platform: {platform.system()}")
    
    try:
        # Create TwoWire object (automatically selects platform implementation)
        wire = TwoWire()
        print("TwoWire object created successfully")
        
        # Initialize I2C interface
        print("Calling wire.begin()...")
        if not wire.begin():
            print("Failed to initialize I2C interface")
            return -1
        print("wire.begin() completed")
        
        # Test basic I2C communication
        print("Testing basic I2C communication...")
        
        # Simple test - try to communicate with device at address 0x6A
        test_addr = 0x6A
        wire.begin_transmission(test_addr)
        print("begin_transmission() called")
        
        result = wire.end_transmission()
        print(f"end_transmission() returned: {result}")
        
        # Test WHO_AM_I register read if device responds
        if result == 0:
            print("Testing sensor WHO_AM_I register...")
            wire.begin_transmission(test_addr)
            wire.write(0x0F)  # WHO_AM_I register address
            wire.end_transmission()
            
            bytes_read = wire.request_from(test_addr, 1)
            if bytes_read == 1:
                who_am_i = wire.read()
                print(f"WHO_AM_I register value: 0x{who_am_i:02x}")
                if who_am_i == 0x6B:
                    print("SUCCESS: ISM330DHCX sensor detected!")
                else:
                    print(f"Different device detected (expected 0x6B, got 0x{who_am_i:02x})")
            else:
                print("Failed to read WHO_AM_I register")
        else:
            print("No device responded at address 0x6A")
            
            # Try alternative address
            test_addr = 0x6B
            print("Trying alternative address 0x6B...")
            wire.begin_transmission(test_addr)
            result = wire.end_transmission()
            print(f"Address 0x6B result: {result}")
        
        # Cleanup
        print("Cleaning up...")
        wire.end()
        print("wire.end() completed")
        
    except Exception as e:
        print(f"Exception caught: {e}")
        return -1
    
    print("Python test completed successfully")
    return 0

if __name__ == "__main__":
    exit(main())
