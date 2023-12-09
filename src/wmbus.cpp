#include "wmbus.h"

#include "3outof6.h"
#include <Arduino.h>

bool try_extracting_telegram_from_radio_packet(std::vector<uint8_t>& packet, std::vector<uint8_t>& telegram)
{
	uint8_t first_word[2];

	auto ret = decode3outof6(packet.data(), first_word, 0);
	if (ret != 0) {
		return false;
	}

	size_t packet_len = first_word[0];
	packet_len += 1; // L field
	packet_len += 2; // CRC after wmbus info
	packet_len += 2 * ((first_word[0] - 9) / 16); // CRC after each 16 bytes

	size_t packet_half = packet_len / 2 + packet_len % 2;

	for (size_t i = 0; i < packet_half; i++)
	{
		uint8_t word[2];
		bool last_byte = i == packet_half - 1;

		auto ret = decode3outof6(packet.data() + 3*i, word, 0);
		if (ret != 0) {
			return false;
		}

		telegram.push_back(word[0]);
		if (!last_byte || packet_len % 2 == 0) {
			telegram.push_back(word[1]);
		}
	}

	return true;
}

uint16_t read_uint16(uint8_t* data)
{
	return (*data) << 8 | *(data + 1);
}

uint32_t read_uint32(uint8_t* data)
{
	uint32_t result = *(data + 3) << 24;
	result |= *(data + 2) << 16;
	result |= *(data + 1) << 8;
	result |= *(data);
	return result;
}

bool parse_wmbus_telegram(std::vector<uint8_t>& telegram, wmbus_frame& frame)
{
	if (telegram.size() < 13) {
		return false;
	}

	if (telegram[1] != 0x44) {
		return false;
	}

	frame.device_manufacturer = read_uint16(telegram.data() + 2);
	frame.device_serial = read_uint32(telegram.data() + 4);

	auto it = telegram.begin() + 13;
	unsigned crc_counter = 1; // Start counting from CI field
	while (it < telegram.end()) {
		if (crc_counter == 16) { // skip CRC pool
			it += 2;
			crc_counter = 0;
			continue;
		}

		frame.data_layer.push_back(*it++);
		crc_counter++;
	}

	return true;
}
