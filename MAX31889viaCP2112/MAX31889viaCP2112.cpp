// MAX31889viaCP2112.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "Max31889.h"
// #include "MockMax31889.h"
#include "CP2112.h"
#include <synchapi.h>
import std;

// forward declarations
int verify_cp2112();
int record_temperatures();
int max_write_throughput();
std::string iso_time();
__int64 get_now_millis();

/// <summary>
/// main entry point, call the appropruate program for your needs.
/// </summary>
/// <returns>0 for success</returns>
int main() {
	auto retval = record_temperatures();
//	auto retval = max_write_throughput();
	std::cout << "Press Enter to exit." << std::endl;
	std::string s;
	std::getline(std::cin, s);
	return retval;
}

/// <summary>
/// Sample program that verifies initialization of the CP2112
/// </summary>
/// <remarks>
/// This program is intended to be called directly from main.
/// </remarks>
/// <returns>0 for success</returns>
int verify_cp2112() {
	CP2112 i2c{};
	std::cout << "Initializing CP2112 device." << std::endl;
	auto retval = i2c.initialize();
	if (retval != 0) {
		std::cerr << "Failed to initialize i2c bridge, error code: " << retval << std::endl;
	}
	std::cout << "Calling cleanup." << std::endl;
	retval = i2c.cleanup();

	if (retval != 0) {
		std::cerr << "Cleanup failed, error code: " << retval << std::endl;
	} else {
		std::cout << "Cleanup success." << std::endl;
		return retval;
	}
}


/// <summary>
/// Sample program that uses the CP2112EK as a USB to i2c bridge to talk to a MAX31889 thermometer.
/// </summary>
/// <remarks>
/// This (the whole project) will not compile without the <c>/std:c++latest</c> flag and the "Build ISO C++23 Standard Library Modules" in visual studio 2022.
/// This function is intended to be called from main directly
/// Output is saved to csv with a timestamped name. CSV contents are ISO timestamp, elapsed_millis, temperature
/// </remarks>
/// <returns>0 for success</returns>
int record_temperatures() {
	const auto now = std::chrono::system_clock::now();
	std::string filename = std::format("output-{:%Y-%m-%d_%H_%M_%OS}.csv", now);
	std::ofstream csvfile;
	csvfile.open(filename);
	if (!csvfile.is_open()) {
		std::cerr << "Unable to open file " << filename << ", aborting." << std::endl;
		return 1;
	}

	const auto default_precision{ std::cout.precision() };
	Max31889 thermometer{};
	thermometer.initialize();
	auto start_time = get_now_millis();
	for (int i = 0; i < 240; i++) {
		auto temperature = thermometer.temperature();
		std::cout << "Recording temperature: " << std::setprecision(4) << temperature << " degrees Celcius." << std::endl;
		auto elapsed_time = get_now_millis() - start_time;
		csvfile << iso_time() << ", " << elapsed_time << ", " << temperature << std::endl;
		csvfile.flush();
		Sleep(15000);
	}
	std::cout << std::setprecision(default_precision);
	csvfile.flush();
	csvfile.close();
	thermometer.cleanup();
	return 0;
}

/// <summary>
/// This is a short main() alternative to measure write throughput.
/// </summary>
/// <remarks>
/// Ignore this, it is only for stress-testing the I2C bus
/// </remarks>
/// <returns></returns>
int max_write_throughput() {
	Max31889 thermometer{};
	thermometer.initialize();
	int bytes = 100000;

	std::cout << "Max Write Throughput:" << std::endl;

	for (int i = 0; i < 5; ++i) {
		const auto t1 = std::chrono::high_resolution_clock::now();
		thermometer.max_write(bytes);
		const auto t2 = std::chrono::high_resolution_clock::now();

		// floating-point duration: no duration_cast needed
		const std::chrono::duration<double, std::milli> fp_ms = t2 - t1;

		std::cout << bytes << " bytes in " << fp_ms.count() << "ms" << std::endl;
	}

	return 0;
}

/// <summary>
/// This is a short main() alternative to measure read/write throughput.
/// </summary>
/// <remarks>
/// Ignore this, it is only for stress-testing the I2C bus
/// </remarks>
/// <returns></returns>
int max_read_write_throughput() {
	Max31889 thermometer{};
	thermometer.initialize();
	int bytes = 100000;

	std::cout << "Max Read/Write Throughput:" << std::endl;

	for (int i = 0; i < 5; ++i) {
		const auto t1 = std::chrono::high_resolution_clock::now();
		thermometer.max_read_write(bytes);
		const auto t2 = std::chrono::high_resolution_clock::now();

		// floating-point duration: no duration_cast needed
		const std::chrono::duration<double, std::milli> fp_ms = t2 - t1;

		std::cout << bytes << " bytes in " << fp_ms.count() << "ms" << std::endl;
	}

	return 0;
}

/// <summary>
/// now() as an ISO 8601 string
/// </summary>
/// <returns>ISO 8601 time as string</returns>
std::string iso_time() {
	// returns an ISO 8601 timestamp string
	// shamelessly stolen from https://stackoverflow.com/a/69812826
	const auto now = std::chrono::system_clock::now();
	return std::format("{:%FT%TZ}", now);
}

__int64 get_now_millis() {
	const auto now = std::chrono::system_clock::now();
	return std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count();
}