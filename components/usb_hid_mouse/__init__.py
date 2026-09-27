from esphome.components import esp32
import esphome.config_validation as cv

CONFIG_SCHEMA = cv.Schema({})


async def to_code(config):
    esp32.add_idf_sdkconfig_option("CONFIG_TINYUSB_HID_COUNT", 1)
