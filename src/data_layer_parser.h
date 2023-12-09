#pragma once

#include <stdint.h>
#include <vector>

enum class data_layer_type {
	UNKNOWN,
	AXIOMA
};

extern data_layer_type get_device_type(uint16_t producer);
extern bool read_data_and_add_to_packet(data_layer_type type,
										uint32_t device_serial,
										std::vector<uint8_t> data_layer,
										std::vector<uint8_t>& lorawan_packet);