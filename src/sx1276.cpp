#include "sx1276.h"

SX1276* lora_radio;

bool init_sx1276(SPIClass& spi)
{
	lora_radio = new SX1276(new Module(5, 16, 17, 4, spi));

	int ret = lora_radio->begin(868.00);
	return ret == RADIOLIB_ERR_NONE;
}

SX1276* get_sx1276_instance()
{
	return lora_radio;
}