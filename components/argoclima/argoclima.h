#pragma once

#include "IRremoteESP8266.h"
#include "IRsend.h"
#include "ir_Argo.h"
#include "esphome/components/climate_ir/climate_ir.h"

namespace esphome {
namespace argoclima {


const uint16_t kIrLed = 4;
const uint8_t ARGOCLIMA_TEMP_MIN = 10;  // Celsius
const uint8_t ARGOCLIMA_TEMP_MAX = 30;  // Celsius

class ArgoclimaClimate : public climate_ir::ClimateIR {
 public:
  ArgoclimaClimate()
      : climate_ir::ClimateIR(
          ARGOCLIMA_TEMP_MIN, ARGOCLIMA_TEMP_MAX, 1.0f, true, true,
          {
            climate::CLIMATE_FAN_AUTO,
            climate::CLIMATE_FAN_LOW,
            climate::CLIMATE_FAN_MEDIUM,
            climate::CLIMATE_FAN_HIGH
          }, {
            climate::CLIMATE_SWING_OFF,
            climate::CLIMATE_SWING_VERTICAL
          }, {
            climate::CLIMATE_PRESET_NONE,
            climate::CLIMATE_PRESET_ECO,
            climate::CLIMATE_PRESET_BOOST,
            climate::CLIMATE_PRESET_SLEEP,
          }
        ) {}

  void setup() override;
  
  /// Send an Argo A/C WREM-3 AC **control** message with the supplied settings.
  /// @param[in, out] ac A Ptr to an IRArgoAC_WREM3 object to use.
  /// @param[in] on The power setting.
  /// @param[in] mode The operation mode setting.
  /// @param[in] degrees The set temperature setting in degrees Celsius.
  /// @param[in] sensorTemp The room (iFeel) temperature sensor reading in degrees
  ///                       Celsius.
  /// @warning The @c sensorTemp param is assumed to be in 0..255 range (uint8_t)
  ///          The overflow is *not* checked, though.
  /// @note The value is rounded to nearest integer, rounding halfway cases
  ///       away from zero. E.g. 1.5 [C] becomes 2 [C].
  /// @param[in] fan The speed setting for the fan.
  /// @param[in] swingv The vertical swing setting.
  /// @param[in] iFeel Whether to enable iFeel (remote temp) mode on the A/C unit.
  /// @param[in] night Enable night mode (raises temp by +1*C after 1h).
  /// @param[in] econo Enable eco mode (limits power consumed).
  /// @param[in] turbo Run the device in turbo/powerful mode.
  /// @param[in] filter Enable filter mode
  /// @param[in] light Enable device display/LEDs
  // void set_sensor(sensor::Sensor *sensor) { this->sensor = sensor; }
  stdAc::opmode_t std_mode;
  stdAc::fanspeed_t std_fan_mode;
  stdAc::swingv_t std_swing_mode;
  bool isOn;
  bool enableLight;
  bool enableEco;
  bool enableTurbo;
  bool enableFilter;
  bool enableNight;
  bool enableIFeel;

 protected:
  void send_command(IRArgoAC_WREM3 *ac, const bool on,
    const stdAc::opmode_t mode, const float degrees, const float sensorTemp,
    const stdAc::fanspeed_t fan, const stdAc::swingv_t swingv, const bool iFeel,
    const bool night, const bool econo, const bool turbo, const bool filter,
    const bool light);
  void transmit_state() override;
};

}  // namespace argoclima
}  // namespace esphome
