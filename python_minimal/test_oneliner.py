"""
Ultra-minimal example - just 10 lines to test I2C interface
"""
import sys
import os
sys.path.append(os.path.dirname(__file__))

from wire import TwoWire

# One-liner test
print("Python Minimal Test")
wire = TwoWire()
success = wire.begin()
print(f"I2C Initialize: {'SUCCESS' if success else 'FAILED'}")
if success:
    wire.begin_transmission(0x6A)
    result = wire.end_transmission()
    print(f"I2C Communication: {'SUCCESS' if result == 0 else 'NO DEVICE'}")
    wire.end()
print("Test complete")
