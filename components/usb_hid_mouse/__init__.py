import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.const import CONF_ID

usb_hid_mouse_ns = cg.esphome_ns.namespace("usb_hid_mouse")
USBHIDMouse = usb_hid_mouse_ns.class_("USBHIDMouse", cg.Component)

CONFIG_SCHEMA = cv.Schema(
    {
        cv.GenerateID(): cv.declare_id(USBHIDMouse),
    }
).extend(cv.COMPONENT_SCHEMA)


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
