#pragma once

#include <stdint.h>

#include <vector>

struct axioma_watermeter {
	uint8_t status;
	uint32_t timestamp;
	uint32_t meter_data;
};

extern bool read_axioma_data(std::vector<uint8_t> data, axioma_watermeter& watermeter_data);