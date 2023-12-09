#include "cc1101.h"

#include <RadioLib.h>

uint8_t sync_word[] = { 0b01010100, 0b00111101 };

CC1101* wmbus_radio;

bool init_cc1101(SPIClass& spi)
{
	wmbus_radio = new CC1101(new Module(22, 26, RADIOLIB_NC, 27, spi));

	int ret = wmbus_radio->begin(868.95, 100, 80, 270);
	return ret == RADIOLIB_ERR_NONE;
}

CC1101* get_cc1101_instance()
{
	return wmbus_radio;
}

volatile bool receivedFlag = false;

ICACHE_RAM_ATTR void setFlag(void)
{
	receivedFlag = true;
}

bool setup_wmbus_receive_configuration()
{
	int ret = 0;

	ret |= wmbus_radio->setPreambleLength(32, 32 - 4);
	ret |= wmbus_radio->setCrcFiltering(false);
	ret |= wmbus_radio->setSyncWord(sync_word, 2);
	wmbus_radio->setPacketReceivedAction(setFlag);

	return ret == RADIOLIB_ERR_NONE;
}

std::vector<uint8_t> receive_packet(unsigned timeout_timestamp)
{
	wmbus_radio->startReceive();

	Serial.println("Waiting for CC1101 interrupt...");
	while (!receivedFlag) {
		if (millis() > timeout_timestamp) {
			return std::vector<uint8_t>();
		}
	}

	auto size = wmbus_radio->getPacketLength();
	std::vector<uint8_t> data(size + 1);
	data[0] = 0x36;
	auto state = wmbus_radio->readData(data.data() + 1, size);

	Serial.println("Received WMBus packet!");

	// Trim postamble 0x55 = 0b01010101
	while (data.back() == 0x55) {
		data.pop_back();
	}

	if (state == RADIOLIB_ERR_NONE && size > 2) {
		return data;
	}
	else {
		return std::vector<uint8_t>();
	}
}