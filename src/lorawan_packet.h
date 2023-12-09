#pragma once

#include <vector>

#include "axioma_reader.h"

extern void add_lorawan_axioma_packet(std::vector<uint8_t>& data, uint32_t serial, axioma_watermeter axioma_data);
extern void add_device_error_packet(std::vector<uint8_t>& data, uint32_t serial);
extern void finish_lorawan_packet(std::vector<uint8_t>& data);