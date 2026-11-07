import esphome.codegen as cg
from esphome.components import climate_ir

AUTO_LOAD = ["climate_ir"]

argo_ulisse_ns = cg.esphome_ns.namespace("argo_ulisse")
ArgoUlisseClimate = argo_ulisse_ns.class_("ArgoUlisseClimate", climate_ir.ClimateIR)

CONFIG_SCHEMA = climate_ir.climate_ir_with_receiver_schema(ArgoUlisseClimate)


async def to_code(config):
    await climate_ir.new_climate_ir(config)
