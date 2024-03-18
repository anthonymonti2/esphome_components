import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import switch
from esphome.const import ENTITY_CATEGORY_CONFIG, CONF_TYPE

from .. import CONF_DFROBOT_SEN0521_ID, DfrobotSen0521Component


DEPENDENCIES = ["dfrobot_sen0521"]

dfrobot_sen0521_ns = cg.esphome_ns.namespace("dfrobot_sen0521")
DfrobotSen0521Switch = dfrobot_sen0521_ns.class_(
    "DfrobotSen0521Switch",
    switch.Switch,
    cg.Component,
    cg.Parented.template(DfrobotSen0521Component),
)

Sen0521PowerSwitch = dfrobot_sen0521_ns.class_(
    "Sen0521PowerSwitch", DfrobotSen0521Switch
)
Sen0521LedSwitch = dfrobot_sen0521_ns.class_("Sen0521LedSwitch", DfrobotSen0521Switch)
Sen0521UartPresenceSwitch = dfrobot_sen0521_ns.class_(
    "Sen0521UartPresenceSwitch", DfrobotSen0521Switch
)
Sen0521StartAfterBootSwitch = dfrobot_sen0521_ns.class_(
    "Sen0521StartAfterBootSwitch", DfrobotSen0521Switch
)

_SWITCH_SCHEMA = (
    switch.switch_schema(
        entity_category=ENTITY_CATEGORY_CONFIG,
    )
    .extend(
        {
            cv.GenerateID(CONF_DFROBOT_SEN0521_ID): cv.use_id(DfrobotSen0521Component),
        }
    )
    .extend(cv.COMPONENT_SCHEMA)
)

CONFIG_SCHEMA = cv.typed_schema(
    {
        "sensor_active": _SWITCH_SCHEMA.extend(
            {cv.GenerateID(): cv.declare_id(Sen0521PowerSwitch)}
        ),
        "turn_on_led": _SWITCH_SCHEMA.extend(
            {cv.GenerateID(): cv.declare_id(Sen0521LedSwitch)}
        ),
        "presence_via_uart": _SWITCH_SCHEMA.extend(
            {cv.GenerateID(): cv.declare_id(Sen0521UartPresenceSwitch)}
        ),
        "start_after_boot": _SWITCH_SCHEMA.extend(
            {cv.GenerateID(): cv.declare_id(Sen0521StartAfterBootSwitch)}
        ),
    }
)


async def to_code(config):
    parent = await cg.get_variable(config[CONF_DFROBOT_SEN0521_ID])
    var = await switch.new_switch(config)
    await cg.register_component(var, config)
    await cg.register_parented(var, parent)
    cg.add(getattr(parent, f"set_{config[CONF_TYPE]}_switch")(var))
