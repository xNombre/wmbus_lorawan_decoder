#pragma once

#include <stdint.h>

#include <vector>

struct wmbus_frame {
	uint16_t device_manufacturer;
	uint32_t device_serial;
	std::vector<uint8_t> data_layer;
};

extern bool try_extracting_telegram_from_radio_packet(std::vector<uint8_t>& packet, std::vector<uint8_t>& telegram);
extern bool parse_wmbus_telegram(std::vector<uint8_t>& telegram, wmbus_frame& frame);