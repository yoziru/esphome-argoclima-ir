#include "argoclima.h"

#include <cmath>

#include "esphome/components/remote_base/remote_base.h"
#include "esphome/core/log.h"

namespace esphome {
namespace argoclima {

static const char *const TAG = "argoclima.climate";
stdAc::opmode_t esptoac_mode(const climate::ClimateMode mode) {
  switch (mode) {
    case climate::CLIMATE_MODE_OFF:
      return stdAc::opmode_t::kOff;
    case climate::CLIMATE_MODE_AUTO:
    case climate::CLIMATE_MODE_HEAT_COOL:
      return stdAc::opmode_t::kAuto;
    case climate::CLIMATE_MODE_COOL:
      return stdAc::opmode_t::kCool;
    case climate::CLIMATE_MODE_HEAT:
      return stdAc::opmode_t::kHeat;
    case climate::CLIMATE_MODE_DRY:
      return stdAc::opmode_t::kDry;
    case climate::CLIMATE_MODE_FAN_ONLY:
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
    case climate::CLIMATE_FAN_AUTO:
      return stdAc::fanspeed_t::kAuto;
    case climate::CLIMATE_FAN_LOW:
      return stdAc::fanspeed_t::kMin;
    case climate::CLIMATE_FAN_MEDIUM:
      return stdAc::fanspeed_t::kMedium;
    case climate::CLIMATE_FAN_HIGH:
      return stdAc::fanspeed_t::kMax;
    default:
      return stdAc::fanspeed_t::kAuto;
  }
}

stdAc::swingv_t esptoac_swing_mode(const climate::ClimateSwingMode swing_mode) {
  switch (swing_mode) {
    case climate::CLIMATE_SWING_OFF:
      return stdAc::swingv_t::kAuto;
    case climate::CLIMATE_SWING_VERTICAL:
      return stdAc::swingv_t::kHighest;
    default:
      return stdAc::swingv_t::kAuto;
  }
}

IRArgoAC_WREM3 ac(kIrLed);

void ArgoclimaClimate::setup() {
  climate_ir::ClimateIR::setup();

  // Set the initial state of some values
  this->enableLight = true;
  this->enableFilter = true;
  this->enableIFeel = true;
}

void ArgoclimaClimate::send_command(IRArgoAC_WREM3 *ac, const bool on,
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


void ArgoclimaClimate::transmit_state() {
  ESP_LOGD("DEBUG", "Setting mode to: %d", this->mode);
  this->std_mode = esptoac_mode(this->mode);
  if (this->mode == climate::CLIMATE_MODE_OFF) {
    this->isOn = false;
  } else {
    this->isOn = true;
  }

  ESP_LOGD("DEBUG", "Setting preset to: %d", this->preset);
  if (this->preset == climate::CLIMATE_PRESET_ECO) {
    this->enableEco = true;
  } else {
    this->enableEco = false;
  }

  if (this->preset == climate::CLIMATE_PRESET_BOOST) {
    this->enableTurbo = true;
  } else {
    this->enableTurbo = false;
  }

  if (this->preset == climate::CLIMATE_PRESET_SLEEP) {
    this->enableNight = true;
    this->enableLight = false;
  } else {
    this->enableNight = false;
    this->enableLight = true;
  }

  ESP_LOGD("DEBUG", "Setting fan mode to: %d", this->fan_mode);
  this->std_fan_mode = esptoac_fan_mode(this->fan_mode);

  ESP_LOGD("DEBUG", "Setting swing mode to: %d", this->swing_mode);
  this->std_swing_mode = esptoac_swing_mode(this->swing_mode);

  this->send_command(&ac, this->isOn, this->std_mode, this->target_temperature,
    this->current_temperature, this->std_fan_mode, this->std_swing_mode,
    this->enableIFeel, this->enableNight, this->enableEco,
    this->enableTurbo, this->enableFilter, this->enableLight
  );

  ESP_LOGD("DEBUG", "Argo A/C remote is in the following state:");
  ESP_LOGD("DEBUG", "  %s\n", ac.toString().c_str());
}
}  // namespace argoclima
}  // namespace esphome
