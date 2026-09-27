from esphome.components import esp32
import esphome.config_validation as cv

CONFIG_SCHEMA = cv.Schema({})


async def to_code(config):
    # Enable one TinyUSB CDC-ACM interface.
    esp32.add_idf_sdkconfig_option("CONFIG_TINYUSB_CDC_ENABLED", True)
    esp32.add_idf_sdkconfig_option("CONFIG_TINYUSB_CDC_COUNT", 1)
    esp32.add_idf_sdkconfig_option("CONFIG_TINYUSB_CDC_RX_BUFSIZE", 256)
    esp32.add_idf_sdkconfig_option("CONFIG_TINYUSB_CDC_TX_BUFSIZE", 256)

    # Enable one TinyUSB HID interface for the mouse.
    esp32.add_idf_sdkconfig_option("CONFIG_TINYUSB_HID_ENABLED", True)
    esp32.add_idf_sdkconfig_option("CONFIG_TINYUSB_HID_COUNT", 1)
