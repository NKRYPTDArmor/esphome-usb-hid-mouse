#pragma once

#include "esphome/core/component.h"
#include "esphome/core/log.h"
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
    // No mouse movement yet.
  }
};

}  // namespace usb_hid_mouse
}  // namespace esphome

extern "C" {

uint8_t const *tud_hid_descriptor_report_cb(uint8_t instance) {
  (void) instance;
  return esphome::usb_hid_mouse::hid_report_descriptor;
}

uint16_t tud_hid_get_report_cb(
    uint8_t instance,
    uint8_t report_id,
    hid_report_type_t report_type,
    uint8_t *buffer,
    uint16_t reqlen) {
  (void) instance;
  (void) report_id;
  (void) report_type;
  (void) buffer;
  (void) reqlen;
  return 0;
}

void tud_hid_set_report_cb(
    uint8_t instance,
    uint8_t report_id,
    hid_report_type_t report_type,
    uint8_t const *buffer,
    uint16_t bufsize) {
  (void) instance;
  (void) report_id;
  (void) report_type;
  (void) buffer;
  (void) bufsize;
}

}
