#include "arducam.h"
#include "esphome/core/log.h"
//#include "esphome/components/i2c/i2c.h"

#ifdef ARDUINO_ARCH_ESP8266

namespace esphome {
namespace arducam {

static const char *TAG = "arducam";

ArduCAM::ArduCAM(const std::string &name) {}

void ArduCAM::setup() {
  uint8_t value;
  ESP_LOGCONFIG(TAG, "Setting up ArduChip...");
  this->spi_setup();

  //Check if the ArduCAM SPI bus is OK
  this->spi_send_byte(ARDUCHIP_TEST_REGISTER, 0x55);
  value = this->spi_read_byte(ARDUCHIP_TEST_REGISTER);
  if (value != 0x55) {
    ESP_LOGE(TAG, "SPI interface Error!");
    return;
  }
  ESP_LOGCONFIG(TAG, "Setup done for ArduChip");
}
void ArduCAM::spi_send_byte(uint8_t a_register, uint8_t value) {
  if (!this->burst_mode_active_) { this->enable(); }
  //delayMicroseconds(10);
  this->write_byte(a_register | ARDUCHIP_WRITE_OPERATION);
  this->write_byte(value);
  ESP_LOGVV(TAG, "send byte: reg 0x%02X val 0x%02X", a_register, value);
  if (!this->burst_mode_active_) { this->disable(); }
}
uint8_t ArduCAM::spi_read_byte(uint8_t a_register) {
  if (!this->burst_mode_active_) { this->enable(); }
  //delayMicroseconds(10);
  this->write_byte(a_register & ARDUCHIP_READ_OPERATION);
  uint8_t value = this->read_byte();
  ESP_LOGVV(TAG, "read byte: reg 0x%02X val 0x%02X", a_register, value);
  if (!this->burst_mode_active_) { this->disable(); }
  return value;
}
void ArduCAM::enable_burst_mode() {
  this->enable();
  this->burst_mode_active_ = true;
  this->set_fifo_burst();
  ESP_LOGD(TAG, "Burst mode enabled");
}
void ArduCAM::disable_burst_mode() {
  this->disable();
  this->burst_mode_active_ = false;
  ESP_LOGD(TAG, "Burst mode disabled");
}
void ArduCAM::dump_config() {
  ESP_LOGCONFIG(TAG, "ArduCAM:");
  ESP_LOGCONFIG(TAG, "  Name: %s", this->name_.c_str());
  uint8_t value = this->spi_read_byte(0x40);
  ESP_LOGCONFIG(TAG, "  version: 0x%02X", value);
  ESP_LOGCONFIG(TAG, "  version: %d", value);
}
void ArduCAM::flush_fifo() {
	this->spi_send_byte(ARDUCHIP_FIFO_CONTROL, FIFO_CLEAR_MASK);
}
void ArduCAM::start_capture() {
	this->spi_send_byte(ARDUCHIP_FIFO_CONTROL, FIFO_START_MASK);
}
void ArduCAM::set_fifo_burst() {
  this->spi_read_byte(BURST_FIFO_READ);
}
void ArduCAM::clear_fifo_flag( ) {
	this->spi_send_byte(ARDUCHIP_FIFO_CONTROL, FIFO_CLEAR_MASK);
}
uint32_t ArduCAM::read_fifo_length() {
	uint32_t len1, len2, len3, length = 0;
	len1 = this->spi_read_byte(FIFO_SIZE1);
  len2 = this->spi_read_byte(FIFO_SIZE2);
  len3 = this->spi_read_byte(FIFO_SIZE3) & 0x7f;
  length = ((len3 << 16) | (len2 << 8) | len1) & 0x07fffff;
	return length;
}
uint8_t ArduCAM::get_bit_status(uint8_t addr, uint8_t bit) {
  uint8_t temp;
  temp = this->spi_read_byte(addr);
  temp = temp & bit;
  return temp;
}
void ArduCAM::set_model(SensorModel model) {
  this->config_.model = model;
}
void ArduCAM::set_framesize(SensorFramesize framesize) {
  if (framesize == this->config_.framesize) {
    return;
  }
  this->config_.framesize = framesize;
}
void ArduCAM::capture() {
  this->clear_fifo_flag();
  this->start_capture();
  int total_time = millis();
  while (!this->get_bit_status(ARDUCHIP_TRIG, CAPTURE_DONE_MASK));
  total_time = millis() - total_time;
  ESP_LOGD(TAG, "Capture done in %d ms", total_time);
  this->buffer_length = this->read_fifo_length();
  ESP_LOGD(TAG, "FIFO length: %d", this->buffer_length);
  //return true;
}
size_t ArduCAM::read_image_chunk(uint8_t *buffer, size_t maxLen, size_t index) {
  if (!this->burst_mode_active_) { this->enable_burst_mode(); }
  size_t i = 0;
  uint8_t value = 0;
  uint8_t last_value = 0;
  //uint8_t picbuffer[maxLen];
  while (i < 64) {
    last_value = value;
    value = this->spi_read_byte(0x00);
    //ESP_LOGD(TAG, "%d  0x%02X", i, value);
    //ESP_LOGD(TAG, "%d", &buffer);
    //picbuffer[i++] = value;
    buffer[i++] = value;
    if ( (value == 0xD9) && (last_value == 0xFF) ) {
      // FF D9    EOI    End of Image
      break;
    }
  }
  this->disable_burst_mode();
  //memcpy(buffer, picbuffer, i);
  //ESP_LOGD(TAG, "memcpy finished");
  return i;
}

/*
void ArduCAM::update() {
  this->capture();
}

void ArduCAM::loop() {
  const uint32_t now = millis();
  if (now - this->last_update_ <= this->max_update_interval_)
    return;
  this->capture();
  this->last_update_ = millis();
}
*/






void CameraSensor::setup() {
  uint8_t vid = 0;
  uint8_t pid = 0;
  uint8_t midh = 0;
  uint8_t midl = 0;

  //auto model = this->parent_->config_.model;
  //if (model == ARDUCAM_MODEL_OV2640) {
  ESP_LOGCONFIG(TAG, "Setting up Sensor...");
  this->write_byte(BANK_SEL, BANK_SEL_SENSOR);
  this->read_byte(SENSOR_CHIPID_HIGH, &vid);
  this->read_byte(SENSOR_CHIPID_LOW, &pid);
  ESP_LOGVV(TAG, "vid: 0x%02X pid: 0x%02X", vid, pid);
  if ((vid != 0x26 ) && (( pid != 0x41 ) || ( pid != 0x42 ))) {
    ESP_LOGE(TAG, "Setup failed for Sensor!");
    return;
  }
  this->reset();
//  } else {
//    ESP_LOGE(TAG, "Unknown ArduCam Device");
//    return;
// }
}
void CameraSensor::dump_config() {
  ESP_LOGCONFIG(TAG, "Camera sensor:");
  LOG_I2C_DEVICE(this);
}

void CameraSensor::reset() {
  this->write_array(SENSOR_RESET_SEQ);
  ESP_LOGD(TAG, "Reset done");
  delay(5);
  ESP_LOGD(TAG, "Wait done");
  this->write_array(SENSOR_INIT_DEFAULTS_SEQ);
  ESP_LOGD(TAG, "Init defaults done");
  this->write_array(SENSOR_SVGA_SEQ);
  ESP_LOGD(TAG, "Init svga done");
  this->write_array(SENSOR_JPEG_SEQ);
  ESP_LOGD(TAG, "Init jpeg done");
}

void CameraSensor::write_array(const struct regval_list *vals) {
  //while ((vals->reg != 0) || (vals->val != 0)) {
  ESP_LOGD(TAG, "Write array start");
  while (vals->reg) {
    this->write_byte(vals->reg, vals->val);
    vals++;
  }
  ESP_LOGD(TAG, "Write array done");
}




}  // namespace arducam
}  // namespace esphome

#endif
