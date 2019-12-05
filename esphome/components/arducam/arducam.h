#pragma once

#ifdef ARDUINO_ARCH_ESP8266

#include "esphome/core/component.h"
#include "esphome/core/defines.h"
#include "esphome/components/camera/camera.h"
#include "esphome/components/i2c/i2c.h"
#include "esphome/components/spi/spi.h"

#include "arduchip.h"
#ifdef OV2640
#include "ov2640.h"
#endif

namespace esphome {
namespace arducam {

typedef enum SensorModel {
  SENSOR_MODEL_OV2640,
  SENSOR_MODEL_OV5640,
} model_t;

typedef enum SensorFramesize {
  SENSOR_FRAMESIZE_320X240,
  SENSOR_FRAMESIZE_640X480,
  SENSOR_FRAMESIZE_800X600,
} framesize_t;

typedef struct {
  model_t model;
  framesize_t framesize;
} sensor_config_t;

//class ArduCam;

class CameraSensor: public Component, public i2c::I2CDevice {
  public:
    void setup() override;
    void dump_config() override;
    void reset();

  protected:
//    ArduCam *arducam_;
    void write_array(const struct regval_list *vals);
};


class ArduCAM: public Component, public camera::Camera,
               public spi::SPIDevice<spi::BIT_ORDER_MSB_FIRST, spi::CLOCK_POLARITY_LOW,
                                     spi::CLOCK_PHASE_LEADING, spi::DATA_RATE_4MHZ> {
  public:
    void set_parent(CameraSensor *parent) { this->parent_ = parent; }
    ArduCAM(const std::string &name);
    sensor_config_t config_{};

    void setup() override;
    void dump_config() override;
    void flush_fifo();
    void start_capture();
    void set_fifo_burst();
    void clear_fifo_flag();
    uint32_t read_fifo_length();
    uint8_t get_bit_status(uint8_t a_register, uint8_t value);

    void set_model(SensorModel model);
    void set_framesize(SensorFramesize framesize);
    //bool request_stream() override;
    void request_image();
    size_t read_image_chunk(uint8_t *buffer, size_t maxLen, size_t index);


  protected:
    CameraSensor *parent_{};
    void spi_send_byte(uint8_t a_register, uint8_t value);
    void enable_burst_mode();
    void disable_burst_mode();
    bool burst_mode_active_{false};
    uint8_t spi_read_byte(uint8_t a_register);
    //uint32_t hash_base() override;
    uint32_t max_update_interval_{30000};
    uint32_t last_update_{0};
    void capture();
};

}  // namespace arducam
}  // namespace esphome

#endif
