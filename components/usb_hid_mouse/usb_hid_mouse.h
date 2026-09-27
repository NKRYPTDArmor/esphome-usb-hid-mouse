#pragma once

#include "esphome/core/component.h"
#include "esphome/core/log.h"
#include "tusb.h"

namespace esphome {
namespace usb_hid_mouse {

// HID mouse report descriptor
static const uint8_t hid_report_descriptor[] = {
    TUD_HID_REPORT_DESC_MOUSE()
};

// Interface numbering:
// CDC consumes TWO interfaces: control + data.
// HID is the third interface.
enum {
  ITF_NUM_CDC = 0,
  ITF_NUM_CDC_DATA,
  ITF_NUM_HID,
  ITF_NUM_TOTAL
};

#define EPNUM_CDC_NOTIF 0x81
#define EPNUM_CDC_OUT   0x02
#define EPNUM_CDC_IN    0x82
#define EPNUM_HID       0x83

#define CONFIG_TOTAL_LEN \
  (TUD_CONFIG_DESC_LEN + TUD_CDC_DESC_LEN + TUD_HID_DESC_LEN)

// Composite configuration: CDC serial + HID mouse
static const uint8_t configuration_descriptor[] = {
    TUD_CONFIG_DESCRIPTOR(
        1,
        ITF_NUM_TOTAL,
        0,
        CONFIG_TOTAL_LEN,
        0,
        100),

    TUD_CDC_DESCRIPTOR(
        ITF_NUM_CDC,
        4,
        EPNUM_CDC_NOTIF,
        8,
        EPNUM_CDC_OUT,
        EPNUM_CDC_IN,
        CFG_TUD_CDC_EP_BUFSIZE),

    TUD_HID_DESCRIPTOR(
        ITF_NUM_HID,
        0,
        HID_ITF_PROTOCOL_MOUSE,
        sizeof(hid_report_descriptor),
        EPNUM_HID,
        CFG_TUD_HID_EP_BUFSIZE,
        10),
};

class USBHIDMouse : public Component {
 public:
  void setup() override {
    ESP_LOGI("usb_hid_mouse", "USB HID mouse component initialized");
  }

  void loop() override {
    // Still intentionally empty.
    // No mouse movement yet.
  }
};

}  // namespace usb_hid_mouse
}  // namespace esphome


extern "C" {

uint8_t const *tud_descriptor_configuration_cb(uint8_t index) {
  (void) index;
  return esphome::usb_hid_mouse::configuration_descriptor;
}

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
