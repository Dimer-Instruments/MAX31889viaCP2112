#pragma once
class MockMax31889
{
private: 
	static const size_t max = 10;
	const double simulated_run[max] = { 23.4, 23.5, 23.6, 23.7, 23.9, 24, 24.4, 24.6, 25, 25.5 };
	size_t index = 0;
public:
	int initialize();
	double temperature();
	int cleanup();
};

