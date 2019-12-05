#pragma once

#include "esphome/core/component.h"
#include "esphome/core/helpers.h"
#include "esphome/core/preferences.h"

namespace esphome {
namespace camera {

enum CameraState {
  CAMERA_STATE_ERROR = 0,
  CAMERA_STATE_UNKNOWN,
  CAMERA_STATE_READY,
  CAMERA_STATE_CAPTURING,
  CAMERA_STATE_STREAMING,
  CAMERA_STATE_SLEEPING
};

class Camera : public Nameable {
 public:
  explicit Camera();
  explicit Camera(const std::string &name);

  void add_on_state_callback(std::function<void()> callback);

  /** Publish a state to the front-end from the back-end.
   *
   * @param state The new state.
   */
  void publish_state(CameraState state);

  /// The current reported state of the camera.
  CameraState state{CAMERA_STATE_UNKNOWN};

  //void request_stream();
  void request_image();

  //
  std::string get_state_as_str();


  size_t read_image(uint8_t *buffer, size_t maxLen, size_t index);
  uint32_t buffer_length{0};
  size_t already_read{0};


 protected:
  uint32_t hash_base() override;

  virtual void capture() = 0;
  virtual size_t read_image_chunk(uint8_t *buffer, size_t maxLen, size_t index) = 0;
  CallbackManager<void()> state_callback_;
};

}  // namespace camera
}  // namespace esphome
