from esphome import pins
import esphome.codegen as cg
import esphome.config_validation as cv

import esphome.components.neopixelbus as npb
import esphome.components.neopixelbus._methods as npb_methods
import esphome.components.neopixelbus.light as npb_light
import esphome.components.neopixelbus.const as npb_const

from esphome.const import (
    CONF_CLOCK_PIN,
    CONF_NUM_CHIPS,
    CONF_DATA_PIN,
    CONF_DATA_RATE,
    CONF_ID,
    CONF_TYPE,
    CONF_METHOD,
    CONF_PIN,
    CONF_VARIANT,
    CONF_INVERT
)


AUTO_LOAD = ["neopixelbus", "output"]
CODEOWNERS = ["@mabels"]
MULTI_CONF = True
CONF_BUS = "bus"

CONF_CHANNEL_OFFSET = "channel_offset"
CONF_CHIP_OFFSET = "chip_offset"
CONF_CHIP_CHANNELS = "chip_channels"

bus_ns = cg.esphome_ns.namespace("neopixel_bus")

NeoPixelBus = bus_ns.class_("NeoPixel_Bus", cg.Component)

NeoPixelBusId = cv.declare_id(NeoPixelBus)

CONFIG_SCHEMA = cv.All(
    cv.COMPONENT_SCHEMA.extend(
        {
            cv.GenerateID(CONF_ID): NeoPixelBusId,
            cv.Required(CONF_VARIANT): cv.one_of(*npb_const.CHIP_TYPES, lower=True),
            cv.Optional(CONF_DATA_PIN): pins.internal_gpio_output_pin_number,
            cv.Optional(CONF_CLOCK_PIN): pins.internal_gpio_output_pin_number,
            cv.Optional(CONF_PIN): pins.internal_gpio_output_pin_number,
            cv.Optional(CONF_INVERT, default="no"): cv.boolean,
            cv.Optional(CONF_METHOD): npb_light._validate_method,
            cv.Optional(CONF_DATA_RATE): cv.frequency,
            cv.Required(CONF_NUM_CHIPS): cv.positive_not_null_int,
            cv.Optional(CONF_CHIP_CHANNELS, default=3): cv.positive_not_null_int,
        }
    ),
    npb_light._choose_default_method,
)

async def to_code(config):
    method = config[CONF_METHOD]
    method_template = npb_methods.METHODS[method[CONF_TYPE]].to_code(
        method, config[CONF_VARIANT], config[CONF_INVERT]
    )
    out_type = NeoPixelBus.template(method_template)
    rhs = out_type.new(f"{config[CONF_ID]}", config[CONF_CHIP_CHANNELS], config[CONF_NUM_CHIPS])
    var = cg.Pvariable(config[CONF_ID], rhs, out_type)
    if CONF_PIN in config:
        cg.add(var.config_pin(config[CONF_PIN]))
    else:
        cg.add(var.config_pin(config[CONF_CLOCK_PIN], config[CONF_DATA_PIN]))
    await cg.register_component(var, config)

    # https://github.com/Makuna/NeoPixelBus/blob/master/library.json
    # Version Listed Here: https://registry.platformio.org/libraries/makuna/NeoPixelBus/versions
    cg.add_library("makuna/NeoPixelBus", "2.7.7")