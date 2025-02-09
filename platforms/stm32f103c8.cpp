// // Copyright 2024 Khalil Estell
// //
// // Licensed under the Apache License, Version 2.0 (the "License");
// // you may not use this file except in compliance with the License.
// // You may obtain a copy of the License at
// //
// //      http://www.apache.org/licenses/LICENSE-2.0
// //
// // Unless required by applicable law or agreed to in writing, software
// // distributed under the License is distributed on an "AS IS" BASIS,
// // WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// // See the License for the specific language governing permissions and
// // limitations under the License.

#include <libhal-arm-mcu/dwt_counter.hpp>
#include <libhal-arm-mcu/stm32f1/clock.hpp>
#include <libhal-arm-mcu/stm32f1/constants.hpp>
#include <libhal-arm-mcu/stm32f1/output_pin.hpp>
#include <libhal-arm-mcu/stm32f1/uart.hpp>
#include <libhal-arm-mcu/system_control.hpp>

#include <libhal-actuator/rc_servo.hpp>
#include <libhal-util/bit_bang_i2c.hpp>
#include <libhal-expander/pca9685.hpp>

#include <libhal-arm-mcu/stm32f1/input_pin.hpp>
#include <libhal-arm-mcu/stm32f1/pin.hpp>
#include <libhal-util/serial.hpp>
#include <libhal-util/steady_clock.hpp>
#include <libhal/output_pin.hpp>
#include <libhal/pwm.hpp>
#include <resource_list.hpp>

#include <libhal-exceptions/control.hpp>
#include <libhal/error.hpp>

// constexpr auto set_frequency = hal::hertz(1'000);
// constexpr auto set_duty_cycle = float(0.5);

// class pwm_trial : public hal::pwm {
//   private:
//     void driver_frequency(hal::hertz p_frequency) override {
//       m_frequency = p_frequency;
//     }
//     void driver_duty_cycle(float p_duty_cycle) override {
//       m_duty_cycle = p_duty_cycle;
//     }
//   public:
//     hal::hertz m_frequency{};
//     float m_duty_cycle{};
//     ~pwm_trial() override = default;
// };

void initialize_platform(resource_list& p_list)
{
  using namespace hal::literals;
  using namespace std::chrono_literals;

  p_list.reset = +[]() { hal::cortex_m::reset(); };
  // Set the MCU to the maximum clock speed
  hal::stm32f1::maximum_speed_using_internal_oscillator();
  
  hal::stm32f1::release_jtag_pins(); // If you want to use G0 do this command

  static hal::cortex_m::dwt_counter counter(
    hal::stm32f1::frequency(hal::stm32f1::peripheral::cpu));
  p_list.clock = &counter;

  static hal::stm32f1::output_pin led('C', 13);
  p_list.status_led = &led;

  static hal::stm32f1::uart uart1(hal::port<1>,
                                  hal::buffer<128>,
                                  hal::serial::settings{
                                    .baud_rate = 115200,
                                  });
  p_list.console = &uart1;
  hal::print<1024>(uart1, "console created\n");

  // stm32f1 output_pin may have issues with bit bang taking micromod output pin
  static hal::stm32f1::output_pin sda('A', 0);   // G0
  static hal::stm32f1::output_pin scl('A', 15);  // G1

  hal::print<1024>(uart1, "sda and scl created\n");

  //   static hal::bit_bang_i2c i2c(pins, counter);
  static hal::bit_bang_i2c i2c({ .sda = &sda, .scl = &scl }, counter);
  p_list.i2c = &i2c;
  hal::print<1024>(uart1, "bit bang created\n");

  hal::byte mpu_address = 0x68;
  hal::print<1024>(uart1, "created byte\n");

  try {
    static hal::sensor::mpu6050 mpu(i2c, mpu_address);
    p_list.mpu = &mpu;
    hal::print<1024>(uart1, "mpu created\n");
  } catch (hal::no_such_device) {
    hal::print<1024>(uart1, "mpu FAILED\n\n");
  }

  static hal::expander::pca9685 pca(i2c, 0b100'0000);

  static auto pwm_x = pca.get_pwm_channel<0>();
  static auto pwm_y = pca.get_pwm_channel<1>();

  p_list.pwm_x = &pwm_x;
  p_list.pwm_y = &pwm_y;

  hal::print<1024>(uart1, "pwm created\n");
}
