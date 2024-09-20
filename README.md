# MAX31889viaCP2112

This project is intended to provide some assistance as sample code for an interview project.

The interface for the MAX31889 is simple. Call `initialize()`, make any number of `temperature()` measurements, and then call `cleanup()`. Both `initialize()` and `cleanup()` could be hidden away in the constructor and destructor, but I operate under the (possibly mistaken) belief that hot restarts should always be possible.

Under the hood, the MAX31889 uses a CP2112 as an i2c bridge. Consequently, most of the setup and teardown code is actually in the CP2112. This is also where most of the errors could occur. Errors are output to stderr and can cause early program termination. If an error occurs, it may be beneficial to unplug the device from USB and plug it back in. There is little internal state that survives a power cycle on the CP2112, so this is generally a safe way to start in a known state.

This code has been tested with multiple devices. If, however, debugging becomes necessary, there are a number of commented out statements that will print intermediate results to the terminal. Additionally, the code itself should be sufficiently documented to step through and understand the intended behavior.

