#include "camera.h"
#include "esphome/core/log.h"

namespace esphome {
namespace camera {

static const char *TAG = "camera";

Camera::Camera(const std::string &name) : Nameable(name) {}
Camera::Camera() : Camera("") {}

void Camera::add_on_state_callback(std::function<void()> callback) {
  this->state_callback_.add(std::move(callback));
}
void Camera::publish_state(CameraState state) {
  this->state = state;
  ESP_LOGD(TAG, "'%s': Sending new state '%s'", this->name_.c_str(), this->get_state_as_str().c_str());
  this->state_callback_.call();
}
std::string Camera::get_state_as_str() {
  std::string state;
  switch (this->state) {
    case CAMERA_STATE_ERROR:
      state = "Error";
      break;
    case CAMERA_STATE_READY:
      state = "Ready";
      break;
    case CAMERA_STATE_CAPTURING:
      state = "Capturing";
      break;
    case CAMERA_STATE_STREAMING:
      state = "Streaming";
      break;
    case CAMERA_STATE_SLEEPING:
      state = "Sleeping";
      break;
    default:
      state = "Unknown";
  }
  return state;
}
void Camera::request_image() {
  this->publish_state(CAMERA_STATE_CAPTURING);
  ESP_LOGD(TAG, " Request Image...");
  this->capture();
  this->publish_state(CAMERA_STATE_READY);
}

size_t Camera::read_image(uint8_t *buffer, size_t maxLen, size_t index) {
  //ESP_LOGD(TAG, " maxLen %d", maxLen);
  //ESP_LOGD(TAG, " index %d", index);
  return this->read_image_chunk(buffer, maxLen, index);
}

uint32_t Camera::hash_base() { return 418001110UL; }

}  // namespace camera
}  // namespace esphome
