from esphome.components import esp32
import esphome.config_validation as cv

CONFIG_SCHEMA = cv.Schema({})


async def to_code(config):
    # HID-only diagnostic build.
    esp32.add_idf_sdkconfig_option("CONFIG_TINYUSB_CDC_ENABLED", False)
    esp32.add_idf_sdkconfig_option("CONFIG_TINYUSB_CDC_COUNT", 0)

    # Enable exactly one HID interface.
    esp32.add_idf_sdkconfig_option("CONFIG_TINYUSB_HID_COUNT", 1)
