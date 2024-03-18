import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import binary_sensor
from esphome.const import DEVICE_CLASS_MOTION
from . import CONF_DFROBOT_SEN0521_ID, DfrobotSen0521Component

DEPENDENCIES = ["dfrobot_sen0521"]

CONFIG_SCHEMA = binary_sensor.binary_sensor_schema(
    device_class=DEVICE_CLASS_MOTION
).extend(
    {
        cv.GenerateID(CONF_DFROBOT_SEN0521_ID): cv.use_id(DfrobotSen0521Component),
    }
)


async def to_code(config):
    parent = await cg.get_variable(config[CONF_DFROBOT_SEN0521_ID])
    binary_sens = await binary_sensor.new_binary_sensor(config)

    cg.add(parent.set_detected_binary_sensor(binary_sens))
