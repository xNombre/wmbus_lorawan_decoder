#pragma once

#include <vector>

#include <RadioLib.h>

extern void init_node(SX1276* radio);
extern bool lorawan_activation();
extern bool send_lorawan_message(std::vector<uint8_t>& data);