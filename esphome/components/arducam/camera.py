import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import camera, i2c, spi
from esphome.const import CONF_ID, CONF_NAME, CONF_MODEL, ESP_PLATFORM_ESP8266

ESP_PLATFORMS = [ESP_PLATFORM_ESP8266]
DEPENDENCIES = ['i2c', 'spi']

arducam_ns = cg.esphome_ns.namespace('arducam')
ArduCAM = arducam_ns.class_('ArduCAM',
                            cg.Component,
                            camera.Camera,
                            spi.SPIDevice)
CameraSensor = arducam_ns.class_('CameraSensor',
                                 cg.Component,
                                 i2c.I2CDevice)

SensorModel = arducam_ns.enum('SensorModel')
SENSOR_MODELS = {
    'OV2640': SensorModel.SENSOR_MODEL_OV2640,
    'OV5640': SensorModel.SENSOR_MODEL_OV5640,
}

SensorFramesize = arducam_ns.enum('SensorFramesize')
SENSOR_FRAMESIZES = {
    '320X240': SensorFramesize.SENSOR_FRAMESIZE_320X240,
    '640X480': SensorFramesize.SENSOR_FRAMESIZE_640X480,
    '800X600': SensorFramesize.SENSOR_FRAMESIZE_800X600,
}

CONF_RESOLUTION = 'resolution'
CONF_CAMERASENSOR = 'camerasensor'

CONFIG_SCHEMA = cv.Schema({
    cv.GenerateID(): cv.declare_id(ArduCAM),
    cv.GenerateID(CONF_CAMERASENSOR): cv.declare_id(CameraSensor),
    cv.Required(CONF_NAME): cv.string,
    cv.Required(CONF_MODEL): cv.enum(SENSOR_MODELS, upper=True),
    cv.Optional(CONF_RESOLUTION, default='800X600'): cv.enum(SENSOR_FRAMESIZES, upper=True),
}).extend(spi.SPI_DEVICE_SCHEMA).extend(i2c.i2c_device_schema(None)).extend(cv.COMPONENT_SCHEMA)

print('config_schema:', CONFIG_SCHEMA)


def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID], config[CONF_NAME])
    yield cg.register_component(var, config)
    yield spi.register_spi_device(var, config)
    yield camera.register_camera(var, config)
    cg.add(var.set_model(config[CONF_MODEL]))
    cg.add_define(config[CONF_MODEL])
    cg.add(var.set_framesize(config[CONF_RESOLUTION]))

    camerasensor = cg.new_Pvariable(config[CONF_CAMERASENSOR])
    yield cg.register_component(camerasensor, config)
    yield i2c.register_i2c_device(camerasensor, config)
