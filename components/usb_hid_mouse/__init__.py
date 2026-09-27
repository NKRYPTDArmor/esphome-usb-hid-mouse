from esphome.components import esp32
import esphome.config_validation as cv

CONFIG_SCHEMA = cv.Schema({})


async def to_code(config):
    # Compile one CDC-ACM interface into TinyUSB.
    esp32.add_idf_sdkconfig_option("CONFIG_TINYUSB_CDC_ENABLED", True)
    esp32.add_idf_sdkconfig_option("CONFIG_TINYUSB_CDC_COUNT", 1)
    esp32.add_idf_sdkconfig_option("CONFIG_TINYUSB_CDC_RX_BUFSIZE", 256)
    esp32.add_idf_sdkconfig_option("CONFIG_TINYUSB_CDC_TX_BUFSIZE", 256)

    # A HID count greater than zero enables TinyUSB HID.
    esp32.add_idf_sdkconfig_option("CONFIG_TINYUSB_HID_COUNT", 1)
