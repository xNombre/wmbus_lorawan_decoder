#include "data_layer_parser.h"

#include <Arduino.h>

#include "lorawan_packet.h"
#include "axioma_reader.h"

data_layer_type get_device_type(uint16_t producer)
{
	std::string producer_name;

	char l1 = (producer & 0x7c) >> 2;
	char l2 = ((producer & 0x3) << 3) | ((producer & 0xE000) << 13);
	char l3 = (producer & 0x1f00) >> 8;

	producer_name.push_back(64 + l1);
	producer_name.push_back(64 + l2);
	producer_name.push_back(64 + l3);

	Serial.print("Manufacturer ID: ");
	Serial.println(producer_name.c_str());

	if (producer_name == "AXI") {
		return data_layer_type::AXIOMA;
	}

	return data_layer_type::UNKNOWN;
}

bool read_data_and_add_to_packet(data_layer_type type,
								 uint32_t device_serial,
								 std::vector<uint8_t> data_layer,
								 std::vector<uint8_t>& lorawan_packet)
{
	if (type == data_layer_type::AXIOMA) {
		Serial.println("Reading Axioma data...");

		axioma_watermeter axioma_data;
		auto ret = read_axioma_data(data_layer, axioma_data);
		if (!ret) {
			return ret;
		}

		add_lorawan_axioma_packet(lorawan_packet, device_serial, axioma_data);
		return true;
	}

	return false;
}