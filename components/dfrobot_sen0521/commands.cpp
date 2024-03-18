#include "commands.h"

#include <cmath>

#include "esphome/core/log.h"

#include "dfrobot_sen0521.h"

namespace esphome {
namespace dfrobot_sen0521 {

static const char *const TAG = "dfrobot_sen0521.commands";

uint8_t Command::execute(DfrobotSen0521Component *parent) {
  this->parent_ = parent;
  if (this->cmd_sent_) {
    if (this->parent_->read_message_()) {
      std::string message(this->parent_->read_buffer_);
      if (message.rfind("is not recognized as a CLI command") != std::string::npos) {
        ESP_LOGD(TAG, "Command not recognized properly by sensor");
        if (this->retries_left_ > 0) {
          this->retries_left_ -= 1;
          this->cmd_sent_ = false;
          ESP_LOGD(TAG, "Retrying...");
          return 0;
        } else {
          this->parent_->find_prompt_();
          return 1;  // Command done
        }
      }
      uint8_t rc = on_message(message);
      if (rc == 2) {
        if (this->retries_left_ > 0) {
          this->retries_left_ -= 1;
          this->cmd_sent_ = false;
          ESP_LOGD(TAG, "Retrying...");
          return 0;
        } else {
          this->parent_->find_prompt_();
          return 1;  // Command done
        }
      } else if (rc == 0) {
        return 0;
      } else {
        this->parent_->find_prompt_();
        return 1;
      }
    }
    if (millis() - this->parent_->ts_last_cmd_sent_ > this->timeout_ms_) {
      ESP_LOGD(TAG, "Command timeout");
      if (this->retries_left_ > 0) {
        this->retries_left_ -= 1;
        this->cmd_sent_ = false;
        ESP_LOGD(TAG, "Retrying...");
      } else {
        return 1;  // Command done
      }
    }
  } else if (this->parent_->send_cmd_(this->cmd_.c_str(), this->cmd_duration_ms_)) {
    this->cmd_sent_ = true;
  }
  return 0;  // Command not done yet
}

uint8_t ReadStateCommand::execute(DfrobotSen0521Component *parent) {
  this->parent_ = parent;
  if (this->parent_->read_message_()) {
    std::string message(this->parent_->read_buffer_);
    if (message.rfind("$JYBSS,0, , , *") != std::string::npos) {
      this->parent_->set_detected_(false);
      this->parent_->set_active(true);
      return 1;  // Command done
    } else if (message.rfind("$JYBSS,1, , , *") != std::string::npos) {
      this->parent_->set_detected_(true);
      this->parent_->set_active(true);
      return 1;  // Command done
    }
  }
  if (millis() - this->parent_->ts_last_cmd_sent_ > this->timeout_ms_) {
    return 1;  // Command done, timeout
  }
  return 0;  // Command not done yet.
}

uint8_t ReadStateCommand::on_message(std::string &message) { return 1; }

uint8_t PowerCommand::on_message(std::string &message) {
  if (message == "sensor stopped already") {
    this->parent_->set_active(false);
    ESP_LOGI(TAG, "Stopped sensor (already stopped)");
    return 1;  // Command done
  } else if (message == "sensor started already") {
    this->parent_->set_active(true);
    ESP_LOGI(TAG, "Started sensor (already started)");
    return 1;  // Command done
  } else if (message == "new parameter isn't save, can't startSensor") {
    this->parent_->set_active(false);
    ESP_LOGE(TAG, "Can't start sensor! (Use SaveCfgCommand to save config first)");
    return 1;  // Command done
  } else if (message == "Done") {
    this->parent_->set_active(this->power_on_);
    if (this->power_on_) {
      ESP_LOGI(TAG, "Started sensor");
    } else {
      ESP_LOGI(TAG, "Stopped sensor");
    }
    return 1;  // Command done
  }
  return 0;  // Command not done yet.
}

DetRangeCfgCommand::DetRangeCfgCommand(float min1, float max1) {
  // TODO: Print warning when values are rounded
  if (min1 < 0 || max1 < 0) {
    this->min1_ = min1 = 0;
    this->max1_ = max1 = 0;

    ESP_LOGW(TAG, "DetRangeCfgCommand invalid input parameters. Using range config 0 0.");

    this->cmd_ = "detRangeCfg -1 0 0";
  } else {
    this->min1_ = min1 = round(min1 / 0.15) * 0.15;
    this->max1_ = max1 = round(max1 / 0.15) * 0.15;

    this->cmd_ = str_sprintf("detRangeCfg -1 "
                             "%.0f %.0f",
                             min1 / 0.15, max1 / 0.15);
  }

  this->min1_ = min1;
  this->max1_ = max1;
};

uint8_t DetRangeCfgCommand::on_message(std::string &message) {
  if (message == "sensor is not stopped") {
    ESP_LOGE(TAG, "Cannot configure range config. Sensor is not stopped!");
    return 1;  // Command done
  } else if (message == "Done") {
    ESP_LOGI(TAG, "Updated detection area config:");
    ESP_LOGI(TAG, "Detection area 1 from %.02fm to %.02fm.", this->min1_, this->max1_);
    ESP_LOGD(TAG, "Used command: %s", this->cmd_.c_str());
    return 1;  // Command done
  }
  return 0;  // Command not done yet.
}

SetLatencyCommand::SetLatencyCommand(float delay_after_detection, float delay_after_disappear) {
  delay_after_detection = std::round(delay_after_detection / 0.025f) * 0.025f;
  delay_after_disappear = std::round(delay_after_disappear / 0.025f) * 0.025f;
  this->delay_after_detection_ = clamp(delay_after_detection, 0.0f, 1638.375f);
  this->delay_after_disappear_ = clamp(delay_after_disappear, 0.0f, 1638.375f);
  this->cmd_ = str_sprintf("setLatency %.03f %.03f", this->delay_after_detection_, this->delay_after_disappear_);
};

uint8_t SetLatencyCommand::on_message(std::string &message) {
  if (message == "sensor is not stopped") {
    ESP_LOGE(TAG, "Cannot configure output latency. Sensor is not stopped!");
    return 1;  // Command done
  } else if (message == "Done") {
    ESP_LOGI(TAG, "Updated output latency config:");
    ESP_LOGI(TAG, "Signal that someone was detected is delayed by %.03f s.", this->delay_after_detection_);
    ESP_LOGI(TAG, "Signal that nobody is detected anymore is delayed by %.03f s.", this->delay_after_disappear_);
    ESP_LOGD(TAG, "Used command: %s", this->cmd_.c_str());
    return 1;  // Command done
  }
  return 0;  // Command not done yet
}

uint8_t FactoryResetCommand::on_message(std::string &message) {
  if (message == "sensor is not stopped") {
    ESP_LOGE(TAG, "Cannot factory reset. Sensor is not stopped!");
    return 1;  // Command done
  } else if (message == "Done") {
    ESP_LOGI(TAG, "Sensor factory reset done.");
    return 1;  // Command done
  }
  return 0;  // Command not done yet
}

uint8_t ResetSystemCommand::on_message(std::string &message) {
  if (message == "leapMMW:/>") {
    ESP_LOGI(TAG, "Restarted sensor.");
    return 1;  // Command done
  }
  return 0;  // Command not done yet
}

uint8_t SaveCfgCommand::on_message(std::string &message) {
  if (message == "no parameter has changed") {
    ESP_LOGI(TAG, "Not saving config (no parameter changed).");
    return 1;  // Command done
  } else if (message == "Done") {
    ESP_LOGI(TAG, "Saved config. Saving a lot may damage the sensor.");
    return 1;  // Command done
  }
  return 0;  // Command not done yet
}

uint8_t UartOutputCommand::on_message(std::string &message) {
  if (message == "sensor is not stopped") {
    ESP_LOGE(TAG, "Cannot set uart output mode. Sensor is not stopped!");
    return 1;  // Command done
  } else if (message == "Done") {
    ESP_LOGI(TAG, "Set uart mode done.");
    if (this->active_) {
      this->parent_->set_uart_presence_active(true);
      ESP_LOGI(TAG, "Presence information is sent via UART and GPIO.");
    } else {
      this->parent_->set_uart_presence_active(false);
      ESP_LOGI(TAG, "Presence information is only sent via GPIO.");
    }
    ESP_LOGD(TAG, "Used command: %s", this->cmd_.c_str());
    return 1;  // Command done
  }
  return 0;  // Command not done yet
}

uint8_t SensitivityCommand::on_message(std::string &message) {
  if (message == "sensor is not stopped") {
    ESP_LOGE(TAG, "Cannot set sensitivity. Sensor is not stopped!");
    return 1;  // Command done
  } else if (message == "Done") {
    ESP_LOGI(TAG, "Set sensitivity done. Set to value %d.", this->sensitivity_);
    ESP_LOGD(TAG, "Used command: %s", this->cmd_.c_str());
    return 1;  // Command done
  }
  return 0;  // Command not done yet
}

}  // namespace dfrobot_sen0521
}  // namespace esphome
