#pragma once

namespace esphome {
namespace arducam {

#define SEQ_ENDMARKER { 0, 0 }

struct regval_list {
	uint8_t reg;
	uint8_t val;
};

}  // namespace arducam
}  // namespace esphome
