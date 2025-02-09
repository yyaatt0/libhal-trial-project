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
#include <libhal-sensor/imu/mpu6050.hpp>
#include <libhal-util/bit_bang_i2c.hpp>
#include <libhal-micromod/micromod.hpp>

#include <libhal-util/serial.hpp>
#include <libhal/output_pin.hpp>
#include <libhal/pwm.hpp>
#include <resource_list.hpp>

#include <libhal-exceptions/control.hpp>
#include <libhal/error.hpp>

// void initialize_platform(resource_list& p_list)
// {
//   using namespace hal::literals;
//   p_list.reset = +[]() { hal::micromod::v1::reset(); },

//   hal::micromod::v1::initialize_platform();

//   p_list.status_led = &hal::micromod::v1::led();
//   p_list.clock = &hal::micromod::v1::uptime_clock();
//   p_list.console = &hal::micromod::v1::console(hal::buffer<128>);
//   // (*((unsigned int *) 0x4001'0C00)) = 0x66666666;
// }

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

  p_list.reset = +[]() { hal::cortex_m::reset(); };
  // Set the MCU to the maximum clock speed
  hal::stm32f1::maximum_speed_using_internal_oscillator();

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

  static auto& sda = hal::micromod::v1::output_g0();
  static auto& scl = hal::micromod::v1::output_g1();

  // stm32f1 output_pin may have issues with bit bang taking micromod output pin
  // static hal::stm32f1::output_pin sda('A', 0);   // G0
  // static hal::stm32f1::output_pin scl('A', 15);  // G1

  hal::print<1024>(uart1, "sda and scl created\n");

  static hal::bit_bang_i2c i2c({.sda = &sda, .scl = & scl}, counter);
  p_list.i2c = &i2c;
  hal::print<1024>(uart1, "bit bang created\n");

  hal::byte mpu_address = 0x68;
  // hal::print<1024>(uart1, "created byte\n");

  static hal::sensor::mpu6050 mpu(i2c, mpu_address);
  p_list.mpu = &mpu;
  hal::print<1024>(uart1, "mpu created\n"); 

  // static pwm_trial pwm_x;
  // pwm_x.frequency(set_frequency);
  // pwm_x.duty_cycle(set_duty_cycle);
  // p_list.pwm_x = &pwm_x;

  // static pwm_trial pwm_y;
  // pwm_y.frequency(set_frequency);
  // pwm_y.duty_cycle(set_duty_cycle);
  // p_list.pwm_y = &pwm_y;
}
