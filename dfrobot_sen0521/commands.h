#pragma once

#include <cstdint>
#include <string>

#include "esphome/core/helpers.h"

namespace esphome {
namespace dfrobot_sen0521 {

class DfrobotSen0521Component;

// Use command queue and time stamps to avoid blocking.
// When component has run time, check if minimum time (1s) between
// commands has passed. After that run a command from the queue.
class Command {
 public:
  virtual ~Command() = default;
  virtual uint8_t execute(DfrobotSen0521Component *parent);
  virtual uint8_t on_message(std::string &message) = 0;

 protected:
  DfrobotSen0521Component *parent_{nullptr};
  std::string cmd_;
  bool cmd_sent_{false};
  int8_t retries_left_{2};
  uint32_t cmd_duration_ms_{1000};
  uint32_t timeout_ms_{1500};
};

class ReadStateCommand : public Command {
 public:
  uint8_t execute(DfrobotSen0521Component *parent) override;
  uint8_t on_message(std::string &message) override;

 protected:
  uint32_t timeout_ms_{500};
};

class PowerCommand : public Command {
 public:
  PowerCommand(bool power_on) : power_on_(power_on) {
    if (power_on) {
      cmd_ = "sensorStart";
    } else {
      cmd_ = "sensorStop";
    }
  };
  uint8_t on_message(std::string &message) override;

 protected:
  bool power_on_;
};

class DetRangeCfgCommand : public Command {
 public:
  DetRangeCfgCommand(float min1, float max1);
  uint8_t on_message(std::string &message) override;

 protected:
  float min1_, max1_;
  // TODO: Set min max values in component, so they can be published as sensor.
};

class SetLatencyCommand : public Command {
 public:
  SetLatencyCommand(float delay_after_detection, float delay_after_disappear);
  uint8_t on_message(std::string &message) override;

 protected:
  float delay_after_detection_;
  float delay_after_disappear_;
};

class FactoryResetCommand : public Command {
 public:
  FactoryResetCommand() { cmd_ = "resetCfg"; };
  uint8_t on_message(std::string &message) override;
};

class ResetSystemCommand : public Command {
 public:
  ResetSystemCommand() { cmd_ = "resetSystem"; }
  uint8_t on_message(std::string &message) override;
};

class SaveCfgCommand : public Command {
 public:
  SaveCfgCommand() { cmd_ = "saveCfg"; }
  uint8_t on_message(std::string &message) override;

 protected:
  uint32_t cmd_duration_ms_{3000};
  uint32_t timeout_ms_{3500};
};

class UartOutputCommand : public Command {
 public:
  UartOutputCommand(bool active) : active_(active) {
    if (active) {
      cmd_ = "setUartOutput 1 1";
    } else {
      cmd_ = "setUartOutput 1 0";
    }
  };
  uint8_t on_message(std::string &message) override;

 protected:
  bool active_;
};

class SensitivityCommand : public Command {
 public:
  SensitivityCommand(uint8_t sensitivity) : sensitivity_(sensitivity) {
    if (sensitivity > 9)
      sensitivity_ = sensitivity = 9;
    char tmp_cmd[20] = {0};
    sprintf(tmp_cmd, "setSensitivity %d", sensitivity);
    cmd_ = std::string(tmp_cmd);
  };
  uint8_t on_message(std::string &message) override;

 protected:
  uint8_t sensitivity_;
};

}  // namespace dfrobot_sen0521
}  // namespace esphome
