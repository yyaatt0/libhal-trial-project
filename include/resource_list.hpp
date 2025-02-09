// Copyright 2024 Khalil Estell
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#pragma once


#include <libhal-util/bit_bang_i2c.hpp>
#include <libhal-sensor/imu/mpu6050.hpp>
#include <libhal-actuator/rc_servo.hpp>

#include <libhal/pwm.hpp>

#include <optional>
#include <libhal/functional.hpp>
#include <libhal/output_pin.hpp>
#include <libhal/serial.hpp>
#include <libhal/steady_clock.hpp>

struct resource_list
{
  /// Initialized 1st
  hal::callback<void()> reset = +[] {
    while (true) {
      continue;
    }
  };
  // Both status_led and clock are required in order to generate the terminate
  // blink pattern.

  std::optional<hal::steady_clock*> clock;
  std::optional<hal::output_pin*> status_led;
  // Initialize 3rd to support logging error messages
  std::optional<hal::serial*> console;

  std::optional<hal::sensor::mpu6050*> mpu;
  std::optional<hal::bit_bang_i2c*> i2c;
  std::optional<hal::pwm*> pwm_x;
  std::optional<hal::pwm*> pwm_y;
};

/**
 * @brief Initializes the target platform and the resource list
 *
 * @param p_list - the list of resources to initialize for the application to
 * run properly. The initialize platform library should initialize each resource
 * in the resoure list.
 */
void initialize_platform(resource_list& p_list);
