#include "esphome.h"
#include "IRremoteESP8266.h"
#include "IRsend.h"
#include "ir_Argo.h"

const uint16_t kIrLed = 4;

IRArgoAC_WREM3 ac(kIrLed);

class ArgoAC : public Component, public Climate {
  public:
    sensor::Sensor *sensor_{nullptr};

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
    void set_sensor(sensor::Sensor *sensor) { this->sensor_ = sensor; }

    void send_command(IRArgoAC_WREM3 *ac, const bool on,
        const stdAc::opmode_t mode, const float degrees, const float sensorTemp,
        const stdAc::fanspeed_t fan, const stdAc::swingv_t swingv, const bool iFeel,
        const bool night, const bool econo, const bool turbo, const bool filter,
        const bool light) {
      ac->begin();
      ac->setMessageType(argoIrMessageType_t::AC_CONTROL);
      ac->setPower(on);
      ac->setMode(ac->convertMode(mode));
      ac->setTemp(degrees);
      if (sensorTemp != kNoTempValue) {
        ac->setSensorTemp(static_cast<uint8_t>(roundf(sensorTemp)));
      }
      ac->setiFeel(iFeel);
      ac->setFan(ac->convertFan(fan));
      ac->setFlap(ac->convertSwingV(swingv));
      ac->setNight(night);
      ac->setEco(econo);
      ac->setMax(turbo);
      ac->setFilter(filter);
      ac->setLight(light);
      // No Clean setting available.
      // No Beep setting available - always beeps in this mode :)
      ac->send();
    }

    stdAc::opmode_t esptoac_mode(const ClimateMode mode) {
      switch (mode) {
        case CLIMATE_MODE_OFF:
          return stdAc::opmode_t::kOff;
        case CLIMATE_MODE_AUTO:
          return stdAc::opmode_t::kAuto;
        case CLIMATE_MODE_COOL:
          return stdAc::opmode_t::kCool;
        case CLIMATE_MODE_HEAT:
          return stdAc::opmode_t::kHeat;
        case CLIMATE_MODE_DRY:
          return stdAc::opmode_t::kDry;
        case CLIMATE_MODE_FAN_ONLY:
          return stdAc::opmode_t::kFan;
        default:
          return stdAc::opmode_t::kAuto;
      }
    }

    stdAc::fanspeed_t esptoac_fan_mode(const esphome::optional<esphome::climate::ClimateFanMode> fan_mode) {
      if (!fan_mode.has_value()) {
        return stdAc::fanspeed_t::kAuto;
      }

      switch (fan_mode.value()) {
        case CLIMATE_FAN_AUTO:
          return stdAc::fanspeed_t::kAuto;
        case CLIMATE_FAN_LOW:
          return stdAc::fanspeed_t::kLow;
        case CLIMATE_FAN_MEDIUM:
          return stdAc::fanspeed_t::kMedium;
        case CLIMATE_FAN_HIGH:
          return stdAc::fanspeed_t::kHigh;
        default:
          return stdAc::fanspeed_t::kAuto;
      }
    }

    stdAc::swingv_t esptoac_swing_mode(const ClimateSwingMode swing_mode) {
      switch (swing_mode) {
        case CLIMATE_SWING_OFF:
          return stdAc::swingv_t::kAuto;
        case CLIMATE_SWING_VERTICAL:
          return stdAc::swingv_t::kHighest;
        default:
          return stdAc::swingv_t::kAuto;
      }
    }


    void setup() override
    {
      if (this->sensor_) {
        this->sensor_->add_on_state_callback([this](float state) {
          this->current_temperature = state;
          this->publish_state();
        });
        this->current_temperature = this->sensor_->state;
      } else {
        this->current_temperature = NAN;
      }

      auto restore = this->restore_state_();
      if (restore.has_value()) {
        restore->apply(this);
      } else {
        this->mode = climate::CLIMATE_MODE_AUTO;
        this->std_mode = esptoac_mode(this->mode);
        this->target_temperature = roundf(constrain(this->current_temperature, 16, 30));
        this->fan_mode = climate::CLIMATE_FAN_AUTO;
        this->std_fan_mode = esptoac_fan_mode(this->fan_mode);
        this->swing_mode = climate::CLIMATE_SWING_OFF;
        this->std_swing_mode = esptoac_swing_mode(this->swing_mode);
        this->isOn = false;
        this->enableLight = true;
        this->enableEco = false;
        this->enableTurbo = false;
        this->enableFilter = true;
        this->enableNight = false;
        this->enableIFeel = false;
      }

      if (isnan(this->target_temperature)) {
        this->target_temperature = 25;
      }

      this->send_command(&ac, this->isOn, this->std_mode, this->target_temperature,
        this->current_temperature, this->std_fan_mode, this->std_swing_mode,
        this->enableIFeel, this->enableNight, this->enableEco,
        this->enableTurbo, this->enableFilter, this->enableLight
      );

      ESP_LOGD("DEBUG", "[setup] Argo A/C remote is in the following state:");
      ESP_LOGD("DEBUG", "  %s\n", ac.toString().c_str());
    }

    climate::ClimateTraits traits() {
      auto traits = climate::ClimateTraits();
      traits.set_supports_current_temperature(this->sensor_ != nullptr);
      traits.set_supports_two_point_target_temperature(false);
      traits.set_supported_presets({
        CLIMATE_PRESET_NONE,
        CLIMATE_PRESET_ECO,
        CLIMATE_PRESET_BOOST,
        CLIMATE_PRESET_SLEEP,
      });
      traits.set_supported_modes({
        CLIMATE_MODE_OFF,
        CLIMATE_MODE_AUTO,
        CLIMATE_MODE_COOL,
        CLIMATE_MODE_HEAT,
        CLIMATE_MODE_DRY,
        CLIMATE_MODE_FAN_ONLY
      });
      traits.set_supported_fan_modes({
        CLIMATE_FAN_AUTO,
        CLIMATE_FAN_LOW,
        CLIMATE_FAN_MEDIUM,
        CLIMATE_FAN_HIGH
      });
      traits.set_supported_swing_modes({
        CLIMATE_SWING_OFF,
        CLIMATE_SWING_VERTICAL
      });
      // traits.set_supports_two_point_target_temperature(true);
      traits.set_visual_max_temperature(30);
      traits.set_visual_min_temperature(16);
      traits.set_visual_temperature_step(1);

      return traits;
    }

  void control(const ClimateCall &call) override {
    if (call.get_mode().has_value()) {
      ClimateMode mode = *call.get_mode();
      ESP_LOGD("DEBUG", "Setting mode to: %d", mode);
      this->mode = mode;
      this->std_mode = esptoac_mode(this->mode);
      if (mode == CLIMATE_MODE_OFF) {
        this->isOn = false;
      } else {
        this->isOn = true;
      }
    }

    if (call.get_preset().has_value()) {
      ClimatePreset preset = *call.get_preset();
      ESP_LOGD("DEBUG", "Setting preset to: %d", preset);
      this->preset = preset;
      if (preset == CLIMATE_PRESET_ECO) {
        this->enableEco = true;
      } else {
        this->enableEco = false;
      }

      if (preset == CLIMATE_PRESET_BOOST) {
        this->enableTurbo = true;
      } else {
        this->enableTurbo = false;
      }

      if (preset == CLIMATE_PRESET_SLEEP) {
        this->enableNight = true;
        this->enableLight = false;
      } else {
        this->enableNight = false;
        this->enableLight = true;
      }
    }

    if (call.get_target_temperature().has_value()) {
      float temp = *call.get_target_temperature();
      ESP_LOGD("DEBUG", "Setting target temperature to: %f", temp);
      ac.setTemp(temp);
      this->target_temperature = temp;
    }

    if (call.get_fan_mode().has_value()) {
      ClimateFanMode fan_mode = *call.get_fan_mode();
      ESP_LOGD("DEBUG", "Setting fan mode to: %d", fan_mode);
      this->fan_mode = fan_mode;
      this->std_fan_mode = esptoac_fan_mode(this->fan_mode);
    }

    if (call.get_swing_mode().has_value()) {
      ClimateSwingMode swing_mode = *call.get_swing_mode();
      ESP_LOGD("DEBUG", "Setting swing mode to: %d", swing_mode);
      this->swing_mode = swing_mode;
      this->std_swing_mode = esptoac_swing_mode(this->swing_mode);
    }

    this->send_command(&ac, this->isOn, this->std_mode, this->target_temperature,
      this->current_temperature, this->std_fan_mode, this->std_swing_mode,
      this->enableIFeel, this->enableNight, this->enableEco,
      this->enableTurbo, this->enableFilter, this->enableLight
    );

    this->publish_state();

    ESP_LOGD("DEBUG", "Argo A/C remote is in the following state:");
    ESP_LOGD("DEBUG", "  %s\n", ac.toString().c_str());
  }
};
