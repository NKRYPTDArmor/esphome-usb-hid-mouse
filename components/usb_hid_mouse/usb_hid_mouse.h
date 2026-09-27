#pragma once

#include "esphome/core/component.h"
#include "tusb.h"

namespace esphome {
namespace usb_hid_mouse {

static const uint8_t hid_report_descriptor[] = {
    TUD_HID_REPORT_DESC_MOUSE()
};

class USBHIDMouse : public Component {
 public:
  void setup() override {
    ESP_LOGI("usb_hid_mouse", "USB HID mouse component initialized");
  }

  void loop() override {
    // Intentionally empty.
    // No mouse movement is sent yet.
  }
};

}  // namespace usb_hid_mouse
}  // namespace esphome
