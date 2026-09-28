#if defined(USE_ESP32_VARIANT_ESP32P4) || defined(USE_ESP32_VARIANT_ESP32S2) || \
    defined(USE_ESP32_VARIANT_ESP32S3) || defined(USE_ESP32_VARIANT_ESP32S31) || \
    defined(USE_ESP32_VARIANT_ESP32H4)

#include "tinyusb_component.h"

#include "esphome/core/helpers.h"
#include "esphome/core/log.h"
#include "tinyusb_default_config.h"

namespace esphome::tinyusb {

static const char *const TAG = "tinyusb";

static const uint8_t HID_REPORT_DESCRIPTOR[] = {
    TUD_HID_REPORT_DESC_MOUSE(HID_REPORT_ID(1)),
    TUD_HID_REPORT_DESC_KEYBOARD(HID_REPORT_ID(2))
};

enum {
  ITF_NUM_HID = 0,
  ITF_NUM_TOTAL
};

#define EPNUM_HID_IN 0x81

#define CONFIG_TOTAL_LEN \
  (TUD_CONFIG_DESC_LEN + TUD_HID_DESC_LEN)

static const uint8_t CONFIGURATION_DESCRIPTOR[] = {
    TUD_CONFIG_DESCRIPTOR(
        1,
        ITF_NUM_TOTAL,
        0,
        CONFIG_TOTAL_LEN,
        0x00,
        100),

    TUD_HID_DESCRIPTOR(
        ITF_NUM_HID,
        0,
        HID_ITF_PROTOCOL_MOUSE,
        sizeof(HID_REPORT_DESCRIPTOR),
        EPNUM_HID_IN,
        CFG_TUD_HID_EP_BUFSIZE,
        10),
};

void TinyUSB::setup() {
  if (this->string_descriptor_[SERIAL_NUMBER] == nullptr) {
    static char mac_addr_buf[MAC_ADDRESS_BUFFER_SIZE];
    get_mac_address_into_buffer(mac_addr_buf);
    this->string_descriptor_[SERIAL_NUMBER] = mac_addr_buf;
  }

  this->tusb_cfg_ = TINYUSB_DEFAULT_CONFIG();

  this->tusb_cfg_.port = TINYUSB_PORT_FULL_SPEED_0;
  this->tusb_cfg_.phy.skip_setup = false;

  this->tusb_cfg_.descriptor = {
      .device = &this->usb_descriptor_,
      .string = this->string_descriptor_,
      .string_count = SIZE,
      .full_speed_config = CONFIGURATION_DESCRIPTOR,
  };

  esp_err_t result = tinyusb_driver_install(&this->tusb_cfg_);

  if (result != ESP_OK) {
    ESP_LOGE(TAG, "tinyusb_driver_install failed: %s", esp_err_to_name(result));
    this->mark_failed();
    return;
  }

  ESP_LOGI(TAG, "TinyUSB HID-only mouse initialized");
}

bool TinyUSB::move_mouse(int8_t x, int8_t y) {
  if (!tud_mounted()) {
    ESP_LOGW(TAG, "Mouse move ignored: USB is not mounted");
    return false;
  }

  if (!tud_hid_ready()) {
    ESP_LOGW(TAG, "Mouse move ignored: HID interface is not ready");
    return false;
  }

  bool sent = tud_hid_mouse_report(
      1,   // report ID
      0,   // buttons
      x,   // relative X
      y,   // relative Y
      0,   // vertical wheel
      0    // horizontal pan
  );

  if (sent) {
    ESP_LOGD(TAG, "Mouse movement sent: x=%d y=%d", x, y);
  } else {
    ESP_LOGW(TAG, "TinyUSB rejected mouse report");
  }

  return sent;
}
bool TinyUSB::send_key(uint8_t modifier, uint8_t keycode) {
  if (!tud_mounted()) {
    ESP_LOGW(TAG, "Key ignored: USB is not mounted");
    return false;
  }

  if (!tud_hid_ready()) {
    ESP_LOGW(TAG, "Key ignored: HID interface is not ready");
    return false;
  }

  uint8_t keycodes[6] = {keycode, 0, 0, 0, 0, 0};

  // Press the key.
  bool sent = tud_hid_keyboard_report(
      2,          // report ID - keyboard
      modifier,
      keycodes
  );

  if (!sent) {
    ESP_LOGW(TAG, "TinyUSB rejected keyboard report");
    return false;
  }

  // Give the host time to process the key press.
  delay(10);

  // Release all keys.
  uint8_t empty_keys[6] = {0, 0, 0, 0, 0, 0};
  tud_hid_keyboard_report(
      2,          // report ID - keyboard
      0,
      empty_keys
  );

  return true;
}
void TinyUSB::dump_config() {
  ESP_LOGCONFIG(TAG,
                "TinyUSB:\n"
                "  Product ID: 0x%04X\n"
                "  Vendor ID: 0x%04X\n"
                "  Manufacturer: '%s'\n"
                "  Product: '%s'\n"
                "  Serial: '%s'\n"
                "  USB classes: HID mouse only",
                this->usb_descriptor_.idProduct,
                this->usb_descriptor_.idVendor,
                this->string_descriptor_[MANUFACTURER],
                this->string_descriptor_[PRODUCT],
                this->string_descriptor_[SERIAL_NUMBER]);
}

}  // namespace esphome::tinyusb


extern "C" {

uint8_t const *tud_hid_descriptor_report_cb(uint8_t instance) {
  (void) instance;
  return esphome::tinyusb::HID_REPORT_DESCRIPTOR;
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

#endif
