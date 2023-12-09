#pragma once

#include <vector>

#include <RadioLib.h>

extern bool init_cc1101(SPIClass& spi);
extern bool setup_wmbus_receive_configuration();
extern std::vector<uint8_t> receive_packet(unsigned timeout_timestamp);
