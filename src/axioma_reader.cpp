#include <axioma_reader.h>

bool read_axioma_data(std::vector<uint8_t> data, axioma_watermeter& watermeter_data)
{
	if (data.size() < 16) {
		return false;
	}
	
	uint32_t* timestamp = nullptr;
	uint32_t* volume = nullptr;
	for (int i = 0; i < data.size(); i++) {
		auto data_pos = (uint16_t*)(data.data() + i);
		if (*data_pos == 0x6D04) {
			timestamp = (uint32_t*)(data_pos + 1);
		}
		if (*data_pos == 0x1304) {
			volume = (uint32_t*)(data_pos + 1);
		}
	}

	if (!volume || !timestamp) {
		return false;
	}

	watermeter_data.status = data[1];
	watermeter_data.timestamp = *timestamp;
	watermeter_data.meter_data = *volume;

	return true;
}