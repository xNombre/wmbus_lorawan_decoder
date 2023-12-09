
#include <Arduino.h>
#include <SPI.h>

#include "cc1101.h"
#include "sx1276.h"
#include "lorawan.h"
#include "wmbus.h"
#include "lorawan_packet.h"
#include "data_layer_parser.h"

const uint32_t known_device_serial = 0x64100805;
const unsigned end_time = 5 * 60 * 1000; // 5 minutes

SPIClass spi(VSPI);
bool should_run = true;
unsigned tries_count = 0;
unsigned end_timestamp;

void deadend()
{
	while (true);
}

void check_ret(bool ret)
{
	if (!ret) {
		Serial.println("Error occured, goto sleep");
		deadend();
	}
	else {
		Serial.println("OK!");
	}
}

void setup()
{
	Serial.begin(115200);
	Serial.println("Boot started!");

	Serial.println("Init SPI");
	spi.begin();

	Serial.println("Init CC1101...");
	bool ret = init_cc1101(spi);
	check_ret(ret);

	Serial.println("Init SX1276...");
	ret = init_sx1276(spi);
	check_ret(ret);

	Serial.println("Init LoRaWAN node");
	init_node(get_sx1276_instance());

	Serial.println("Begin LoRaWAN activation...");
	ret = lorawan_activation();
	check_ret(ret);

	Serial.println("Prepare RX for WMBus...");
	ret = setup_wmbus_receive_configuration();
	check_ret(ret);

	end_timestamp = millis() + end_time;
}

void PrintHex8(uint8_t data)
{
	char tmp[16];
	sprintf(tmp, "%.2X", data);
	Serial.print(tmp);
}

void loop()
{
	if (!should_run || millis() > end_timestamp || tries_count > 10) {
		return;
	}

	auto wmbus_packet = receive_packet(end_timestamp);
	if (wmbus_packet.empty()) {
		Serial.println("No packet received!");
		return;
	}

	for (auto b : wmbus_packet) {
		PrintHex8(b);
	}
	Serial.println();

	Serial.println("Try to decode WMBus packet...");
	std::vector<uint8_t> telegram;
	auto ret = try_extracting_telegram_from_radio_packet(wmbus_packet, telegram);
	check_ret(ret);

	Serial.print("Telegram: ");
	for (auto b : telegram) {
		PrintHex8(b);
	}
	Serial.println("");

	Serial.println("Parsing WMBus frame...");
	wmbus_frame frame;
	ret = parse_wmbus_telegram(telegram, frame);
	check_ret(ret);

	auto dev_type = get_device_type(frame.device_manufacturer);
	if (dev_type == data_layer_type::UNKNOWN) {
		Serial.println("Unknown device");
		check_ret(false);
	}

	if (frame.device_serial != known_device_serial) {
		Serial.println("Unknown device serial");
		check_ret(false);
	}

	std::vector<uint8_t> lorawan_payload;
	ret = read_data_and_add_to_packet(dev_type, frame.device_serial, frame.data_layer, lorawan_payload);
	check_ret(ret);

	finish_lorawan_packet(lorawan_payload);

	Serial.print("LoRaWAN payload: ");
	for (auto b : lorawan_payload) {
		PrintHex8(b);
	}
	Serial.println("");

	send_lorawan_message(lorawan_payload);

	should_run = false;
}