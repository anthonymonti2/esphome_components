#pragma once

#include "esphome/components/switch/switch.h"
#include "esphome/core/component.h"

#include "../dfrobot_sen0521.h"

namespace esphome {
namespace dfrobot_sen0521 {

class DfrobotSen0521Switch : public switch_::Switch, public Component, public Parented<DfrobotSen0521Component> {};

class Sen0521PowerSwitch : public DfrobotSen0521Switch {
 public:
  void write_state(bool state) override;
};

class Sen0521LedSwitch : public DfrobotSen0521Switch {
 public:
  void write_state(bool state) override;
};

class Sen0521UartPresenceSwitch : public DfrobotSen0521Switch {
 public:
  void write_state(bool state) override;
};

class Sen0521StartAfterBootSwitch : public DfrobotSen0521Switch {
 public:
  void write_state(bool state) override;
};

}  // namespace dfrobot_sen0521
}  // namespace esphome
