#pragma once

#include <iostream>
#include <ctime>

namespace frame_data
{
enum class device_type {
	UNKNOWN,
	AXIOMA_WATERMETER
};
using serial_number = std::string;
using reading = std::string;
enum class device_status {
	OK,
	TEMPORARY_ERROR
};
using time = tm;
}

namespace frame_types
{

inline std::string get_type_name(frame_data::device_type type)
{
	if (type == frame_data::device_type::AXIOMA_WATERMETER) {
		return "Axioma-compatible Watermeter";
	}
	if (type == frame_data::device_type::UNKNOWN) {
		return "Unknown Watermeter";
	}
	return "";
}

inline std::string get_status(frame_data::device_status status)
{
	if (status == frame_data::device_status::OK) {
		return "OK";
	}
	if (status == frame_data::device_status::TEMPORARY_ERROR) {
		return "temporary error";
	}
	return "";
}

struct abstract_frame {
	virtual void print_data() = 0;
};

struct device_error : public abstract_frame
{
	frame_data::device_type type;
	frame_data::serial_number serial;

	virtual void print_data() override
	{
		std::cout << "Device error" << std::endl;
		std::cout << "Device type " << get_type_name(type) << std::endl;
		std::cout << "Device serial " << serial << std::endl;
	}
};

struct watermeter_reading : public abstract_frame
{
	frame_data::device_type type;
	frame_data::serial_number serial;
	frame_data::device_status status;
	frame_data::time timestamp;
	frame_data::reading water_counter;

	virtual void print_data() override
	{
		std::cout << "Watermeter reading" << std::endl;
		std::cout << "Device type " << get_type_name(type) << std::endl;
		std::cout << "Device serial " << serial << std::endl;
		std::cout << "Device status " << get_status(status) << std::endl;

		char buf[80];
		strftime(buf, sizeof(buf), "%d.%m.%Y %R", &timestamp);
		std::cout << "Device timestamp " << buf << std::endl;

		std::cout << "Device reading " << water_counter << "m3" << std::endl;
	}
};
} // namespace frame_types