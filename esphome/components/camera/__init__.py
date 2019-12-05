import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.const import CONF_ID, CONF_INTERNAL, CONF_NAME
from esphome.core import CORE, coroutine, coroutine_with_priority

IS_PLATFORM_COMPONENT = True

camera_ns = cg.esphome_ns.namespace('camera')
Camera = camera_ns.class_('Camera', cg.Nameable)

CONFIG_SCHEMA = cv.Schema({
    cv.Required(CONF_ID): cv.declare_id(Camera)
})


@coroutine
def setup_camera_core_(var, config):
    cg.add(var.set_name(config[CONF_NAME]))
    if CONF_INTERNAL in config:
        cg.add(var.set_internal(config[CONF_INTERNAL]))


@coroutine
def register_camera(var, config):
    if not CORE.has_id(config[CONF_ID]):
        var = cg.Pvariable(config[CONF_ID], var)
    cg.add(cg.App.register_camera(var))
    yield setup_camera_core_(var, config)


@coroutine
def new_camera(config):
    var = cg.new_Pvariable(config[CONF_ID])
    yield register_camera(var, config)
    yield var


@coroutine_with_priority(100.0)
def to_code(config):
    cg.add_define('USE_CAMERA')
    cg.add_global(camera_ns.using)
