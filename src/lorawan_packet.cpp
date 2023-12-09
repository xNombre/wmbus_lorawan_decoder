#include "lorawan_packet.h"

void add_by_byte(std::vector<uint8_t>& array, uint8_t* data, size_t len)
{
	for (size_t i = 0; i < len; i++)
	{
		array.push_back(data[i]);
	}
}

void add_lorawan_axioma_packet(std::vector<uint8_t>& data, uint32_t serial, axioma_watermeter axioma_data)
{
	data.push_back(0x11); // watermeter header
	data.push_back(0x01); // axioma type

	add_by_byte(data, (uint8_t*)&serial, sizeof(serial));
	data.push_back(axioma_data.status);
	add_by_byte(data, (uint8_t*)&axioma_data.timestamp, sizeof(axioma_watermeter::timestamp));
	add_by_byte(data, (uint8_t*)&axioma_data.meter_data, sizeof(axioma_watermeter::meter_data));
}

void add_device_error_packet(std::vector<uint8_t>& data, uint32_t serial)
{
	data.push_back(0x10); // error header
	data.push_back(0x01); // axioma type

	add_by_byte(data, (uint8_t*)&serial, sizeof(serial));
}

void finish_lorawan_packet(std::vector<uint8_t>& data)
{
	data.push_back(0xFF);
}