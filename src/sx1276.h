#pragma once

#include <SPI.h>
#include <RadioLib.h>

extern bool init_sx1276(SPIClass& spi);
extern SX1276* get_sx1276_instance();