#include <iostream>

#include "lora_decoder.h"

int main(int argc, char** argv)
{
	if (argc != 2) {
		std::cout << "Bad params" << std::endl;
		return -1;
	}

	std::string telegram(argv[1]);
	std::vector<uint8_t> hex_data;
	for (size_t i = 0; i < telegram.size(); i += 2) {
		auto hex_value = std::stoi(telegram.substr(i, 2), 0, 16);
		hex_data.push_back(hex_value);
	}

	const auto& readings = lora_decoder::parse_telegram(hex_data);
	if (!readings) {
		std::cout << "Incorrect data" << std::endl;
		return -1;
	}

	const auto& data = readings.value();
	for (size_t i = 0; i < data.size(); ++i) {
		std::cout << "== Data from packet #" << i + 1 << " ==" << std::endl;
		data[i]->print_data();
		std::cout << "=========================" << std::endl;
	}

	return 0;
}