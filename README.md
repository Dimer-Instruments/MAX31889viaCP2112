# MAX31889viaCP2112

This project is intended to provide some assistance as sample code for an interview project.

The interface for the MAX31889 is simple. Call `initialize()`, make any number of `temperature()` measurements, and then call `cleanup()`. Both `initialize()` and `cleanup()` could be hidden away in the constructor and destructor, but I operate under the (possibly mistaken) belief that hot restarts should always be possible.

In the implementation, the MAX31889 uses a CP2112 as an i2c bridge. Consequently, most of the setup and teardown code is actually in the CP2112. This is also where most of the errors could occur. Errors are output to stderr and can cause early program termination. If an error occurs, it may be beneficial to unplug the device from USB and plug it back in. There is little internal state that survives a power cycle on the CP2112, so this is generally a safe way to start in a known state.

This code has been tested with multiple devices. If, however, debugging becomes necessary, there are a number of commented out statements that will print intermediate results to the terminal. Additionally, the code itself should be sufficiently documented to step through and understand the intended behavior.

## MAX31889 Information

There are no device drivers, headers, or libraries to talk to the MAX31889. As such, porting the code from one i2c bridge to another should be fairly straightforward. Write `0b11000001` (`0xC1`) to the register `0x14` to request a temperature measurement. The data sheet for the MAX31889 doesn't do the best job making this clear, but in the register description it does mention that the 2 high bits are reserved and must be `HIGH` in all writes. Once the request is made, wait (read, check, wait, and loop) until the status register `0x00` has the least significant bit `HIGH` (bitwise and with `0x01`). That bit indicates that a temperature is ready to be read from the FIFO queue. Two bytes must be read in burst mode from the data register (AKA the FIFO queue) at `0x08`. The two bytes can be translated into a temperature with units Celcius by treating them as a 16 bit integer and multiplying by 0.005. In the code, this is observed as `((temperature_buffer[0] << 8) + temperature_buffer[1]) * 0.005`.

## CP2112

There are no device drivers for the CP2112 (it appears as an HID device in Windows Device Manager), but there are required headers, static libraries (for linking) and dynamic libraries (for runtime). These should be included in this repository or any zip snapshot we send. Most of the work happens during `initialize()`. 

A brief description of what happens:

1. A simple API call is made to verify that things are linked and dynamic libraries are loading properly. This step is not strictly necessary.
2. A call is made to determine the number of attached devices. This call is necessarily filtered by `vendor_id` and `product_id`. Without the filtering, iterating over the number of devices would cause failure.
3. For each device, a call is made to describe the device. A described device cannot already be open, or an error will occur. Because Windows has active HID devices, filtering by `vendor_id` and `product_id` in the previous step (and this step) is necessary.
4. Assuming the target device is found, the device should be opened. This function call also takes the `vendor_id` and `product_id` filters (to ensure consistent indexing). This step gives us a valid device handle for use until we close the device.
5. The open device is queried using the device handle. This is a sanity check and is not strictly necessary.
6. The device is configured. The defaults would also likely work for our demo, but this step ensures reproducibility.

After the device is initialized, it may be used to read and write from attached i2c slave devices. These calls use both the device handle and address information for the desired i2c slave device.

Upon completion, the device is closed using the device handle. This step invalidates the device handle.
