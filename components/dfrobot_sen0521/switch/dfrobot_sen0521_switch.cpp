#include "dfrobot_sen0521_switch.h"

namespace esphome {
namespace dfrobot_sen0521 {

void Sen0521PowerSwitch::write_state(bool state) { this->parent_->enqueue(make_unique<PowerCommand>(state)); }

void Sen0521LedSwitch::write_state(bool state) {
  bool was_active = false;
  if (this->parent_->is_active()) {
    was_active = true;
    this->parent_->enqueue(make_unique<PowerCommand>(false));
  }
  this->parent_->enqueue(make_unique<SaveCfgCommand>());
  if (was_active) {
    this->parent_->enqueue(make_unique<PowerCommand>(true));
  }
}

void Sen0521UartPresenceSwitch::write_state(bool state) {
  bool was_active = false;
  if (this->parent_->is_active()) {
    was_active = true;
    this->parent_->enqueue(make_unique<PowerCommand>(false));
  }
  this->parent_->enqueue(make_unique<UartOutputCommand>(state));
  this->parent_->enqueue(make_unique<SaveCfgCommand>());
  if (was_active) {
    this->parent_->enqueue(make_unique<PowerCommand>(true));
  }
}

void Sen0521StartAfterBootSwitch::write_state(bool state) {
  bool was_active = false;
  if (this->parent_->is_active()) {
    was_active = true;
    this->parent_->enqueue(make_unique<PowerCommand>(false));
  }
  this->parent_->enqueue(make_unique<SensorCfgStartCommand>(state));
  this->parent_->enqueue(make_unique<SaveCfgCommand>());
  if (was_active) {
    this->parent_->enqueue(make_unique<PowerCommand>(true));
  }
}

}  // namespace dfrobot_sen0521
}  // namespace esphome
