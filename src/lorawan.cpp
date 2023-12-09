#include "lorawan.h"

uint64_t joinEUI = 0x220F5A2C79B0E1DD;
uint64_t devEUI = 0x52835080fd37d30b;
uint8_t nwkKey[] = { 0x07, 0xC3, 0x90, 0x5B, 0x96, 0x7D, 0xA8, 0x9F, 0x41, 0x12, 0x58, 0xAA, 0x56, 0x67, 0x61, 0x0A };
uint8_t appKey[] = { 0xC4, 0xE5, 0x8E, 0x7E, 0xF4, 0x68, 0x50, 0xBA, 0x63, 0x56, 0x74, 0x9D, 0x37, 0x3F, 0x50, 0x33 };

LoRaWANNode* node;

void init_node(SX1276* radio)
{
	node = new LoRaWANNode(radio, &EU868);

	// Report 100% battery level
	node->setDeviceStatus(254);
}

bool lorawan_activation()
{
	int16_t ret = node->restore();

	if (ret == RADIOLIB_ERR_NONE) {
		Serial.println("Loaded LoRaWAN information from storage");
		return true;
	}

	if (ret != RADIOLIB_ERR_NETWORK_NOT_JOINED) {
		Serial.println("Unknown LoRaWAN activation status ");
		Serial.println(ret);
		return false;
	}

	Serial.println("Start LoRaWAN OTAA...");
	ret = node->beginOTAA(joinEUI, devEUI, nwkKey, appKey);

	if (ret != RADIOLIB_ERR_NONE) {
		Serial.println("OTAA failed");
		Serial.println(ret);
		return false;
	}

	Serial.println("Done! Saving data...");
	node->saveSession();

	if (ret != RADIOLIB_ERR_NONE) {
		Serial.println("failed");
		return false;
	}

	return true;
}

bool send_lorawan_message(std::vector<uint8_t>& data)
{
	uint8_t downlink[255];
	size_t received;

	Serial.println("Transmitting LoRaWAN message...");
	auto ret = node->sendReceive(data.data(), data.size(), 10, downlink, &received);
	Serial.println("Done!");

	return ret == RADIOLIB_ERR_NONE;
}