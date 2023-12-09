#pragma once

#include <memory>
#include <string>
#include <vector>
#include <optional>

#include "frame_types.hpp"

class lora_decoder {
public:
	static std::optional<std::vector<std::shared_ptr<frame_types::abstract_frame>>> parse_telegram(const std::vector<uint8_t>& telegram);
private:
	
};