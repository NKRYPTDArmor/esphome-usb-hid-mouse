#if defined(USE_ESP32_VARIANT_ESP32P4) || defined(USE_ESP32_VARIANT_ESP32S2) || \
    defined(USE_ESP32_VARIANT_ESP32S3) || defined(USE_ESP32_VARIANT_ESP32S31) || \
    defined(USE_ESP32_VARIANT_ESP32H4)

#include "tinyusb_component.h"

#include "esphome/core/helpers.h"
#include "esphome/core/log.h"
#include "tinyusb_default_config.h"

namespace esphome::tinyusb {

static const char *const TAG = "tinyusb";

// -----------------------------------------------------------------------------
// HID mouse report descriptor
// -----------------------------------------------------------------------------

static const uint8_t HID_REPORT_DESCRIPTOR[] = {
    TUD_HID_REPORT_DESC_MOUSE()
};

// -----------------------------------------------------------------------------
// USB interface numbering
//
// CDC ACM uses two interfaces:
//   0 = CDC control
//   1 = CDC data
//
// HID mouse:
//   2 = HID
// -----------------------------------------------------------------------------

enum {
  ITF_NUM_CDC = 0,
  ITF_NUM_CDC_DATA,
  ITF_NUM_HID,
  ITF_NUM_TOTAL
};

// -----------------------------------------------------------------------------
// Endpoint assignments
// -----------------------------------------------------------------------------

#define EPNUM_CDC_NOTIF 0x81
#define EPNUM_CDC_OUT   0x02
#define EPNUM_CDC_IN    0x82
#define EPNUM_HID_IN    0x83

// Full-speed endpoint sizes.
#define CDC_EP_SIZE 64
#define HID_EP_SIZE 16

#define CONFIG_TOTAL_LEN \
  (TUD_CONFIG_DESC_LEN + TUD_CDC_DESC_LEN + TUD_HID_DESC_LEN)

// -----------------------------------------------------------------------------
// CDC ACM + HID mouse composite configuration descriptor
// -----------------------------------------------------------------------------

static const uint8_t CONFIGURATION_DESCRIPTOR[] = {
    TUD_CONFIG_DESCRIPTOR(
        1,                    // configuration number
        ITF_NUM_TOTAL,        // total interfaces
        0,                    // configuration string index
        CONFIG_TOTAL_LEN,     // total descriptor length
        0x00,                 // attributes
        100                   // 200 mA
    ),

    TUD_CDC_DESCRIPTOR(
        ITF_NUM_CDC,          // CDC control interface
        0,                    // string index
        EPNUM_CDC_NOTIF,      // notification endpoint
        8,                    // notification endpoint size
        EPNUM_CDC_OUT,        // data OUT endpoint
        EPNUM_CDC_IN,         // data IN endpoint
        CDC_EP_SIZE            // data endpoint size
    ),

    TUD_HID_DESCRIPTOR(
        ITF_NUM_HID,                    // HID interface
        0,                              // string index
        HID_ITF_PROTOCOL_MOUSE,         // mouse protocol
        sizeof(HID_REPORT_DESCRIPTOR),  // report descriptor length
        EPNUM_HID_IN,                   // HID IN endpoint
        HID_EP_SIZE,                    // endpoint size
        10                              // polling interval (ms)
    ),
};

// -----------------------------------------------------------------------------
// ESPHome TinyUSB initialization
// -----------------------------------------------------------------------------

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

  ESP_LOGI(TAG, "TinyUSB CDC + HID mouse initialized");
}

void TinyUSB::dump_config() {
  ESP_LOGCONFIG(TAG,
                "TinyUSB:\n"
                "  Product ID: 0x%04X\n"
                "  Vendor ID: 0x%04X\n"
                "  Manufacturer: '%s'\n"
                "  Product: '%s'\n"
                "  Serial: '%s'\n"
                "  USB configuration: CDC ACM + HID mouse",
                this->usb_descriptor_.idProduct,
                this->usb_descriptor_.idVendor,
                this->string_descriptor_[MANUFACTURER],
                this->string_descriptor_[PRODUCT],
                this->string_descriptor_[SERIAL_NUMBER]);
}

}  // namespace esphome::tinyusb

// -----------------------------------------------------------------------------
// TinyUSB HID callbacks
// -----------------------------------------------------------------------------

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
