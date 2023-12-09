#include "lora_decoder.h"

#include <iomanip>
#include <cstring>

#include "frame_types.h"

frame_data::device_type get_device_type(const uint8_t* code)
{
	std::cout << std::hex << *code << std::endl;
	if (*code == 0x01) {
		return frame_data::device_type::AXIOMA_WATERMETER;
	}

	return frame_data::device_type::UNKNOWN;
}

frame_data::serial_number get_device_serial(const uint32_t* code)
{
	std::stringstream ss;
	ss << std::setfill('0') << std::setw(sizeof(uint32_t) * 2) << std::hex << *code;
	return ss.str();
}

frame_data::serial_number get_device_counter(const uint32_t* code)
{
	double conv = *code;
	conv *= 0.001;

	std::stringstream ss;
	ss.precision(3);
	ss << conv;
	return ss.str();
}

frame_data::device_status get_device_status(const uint8_t* code)
{
	frame_data::device_status status;
	if (*code == 0x00) {
		status = frame_data::device_status::OK;
	}
	if (*code == 0x10) {
		status = frame_data::device_status::TEMPORARY_ERROR;
	}
	return status;
}

frame_data::time get_device_timestamp(const uint32_t* code)
{
	tm time;
	memset(&time, 0, sizeof(time));

	uint16_t date = (*code & 0xffff0000) >> 16;
	time.tm_year = ((((date & 0xFF00) >> 8) & 0xF0) >> 1) + (((date & 0xFF) & 0xE0) >> 5) + 100;
	time.tm_mon = (((date & 0xFF00) >> 8) & 0xF) - 1;
	time.tm_mday = (date & 0xFF) & 0x1F;

	uint16_t clock = *code & 0xffff;
	time.tm_hour = ((clock & 0xFF00) >> 8) & 0x1F;
	time.tm_min = (clock & 0xFF) & 0x3F;

	return time;
}

std::optional<std::vector<std::shared_ptr<frame_types::abstract_frame>>> lora_decoder::parse_telegram(const std::vector<uint8_t>& telegram)
{
	std::vector<std::shared_ptr<frame_types::abstract_frame>> data;

	for (int i = 0; i < telegram.size();)
	{
		auto frame_type = telegram[i];
		auto frame_start = i + 1;

		if (frame_type == 0xFF) {
			break;
		}

		std::shared_ptr<frame_types::abstract_frame> reading;

		if (frame_type == 0x10) {
			frame_types::device_error frame;

			frame.type = get_device_type(telegram.data() + frame_start);
			frame.serial = get_device_serial((uint32_t*)(telegram.data() + frame_start + 1));

			reading = std::make_shared<frame_types::device_error>(frame);
			i += 5;
		}

		if (frame_type == 0x11) {
			frame_types::watermeter_reading frame;

			frame.type = get_device_type(telegram.data() + frame_start);
			frame.serial = get_device_serial((uint32_t*)(telegram.data() + frame_start + 1));
			frame.status = get_device_status(telegram.data() + frame_start + 5);
			frame.timestamp = get_device_timestamp((uint32_t*)(telegram.data() + frame_start + 6));
			frame.water_counter = get_device_counter((uint32_t*)(telegram.data() + frame_start + 10));

			i += 14;
			reading = std::make_shared<frame_types::watermeter_reading>(frame);
		}

		data.push_back(reading);
		i++;
	}

	return data;
}