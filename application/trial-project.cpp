#include <libhal-actuator/rc_servo.hpp>
#include <libhal-util/serial.hpp>
#include <libhal-util/steady_clock.hpp>
#include <libhal/units.hpp>
#include <resource_list.hpp>

using namespace hal::literals;
using namespace std::chrono_literals;

void application(resource_list p_resources)
{
  

  // auto& i2c = *p_resources.i2c.value();
  // hal::print<1024>(console, "this is so sad i2c\n");

  auto& console = *p_resources.console.value();
  auto& clock = *p_resources.clock.value();

  auto& mpu = *p_resources.mpu.value();
  hal::print<1024>(console, "this is so sad mpu\n");

  auto& pwm_x = *p_resources.pwm_x.value();
  auto& pwm_y = *p_resources.pwm_y.value();
  hal::print<1024>(console, "this is so sad pwms\n");

  hal::actuator::rc_servo::settings rc_servo_settings{
    .frequency = 50,
    .min_angle = -90,
    .max_angle = 90,
    .min_microseconds = 1000,
    .max_microseconds = 2000,
  };

  hal::actuator::rc_servo servo_x(pwm_x, rc_servo_settings);
  hal::actuator::rc_servo servo_y(pwm_y, rc_servo_settings);
  hal::print<1024>(console, "this so sad servos\n");

  hal::degrees x_pos = 0.0;
  hal::degrees y_pos = 0.0;

  servo_x.position(x_pos);
  servo_y.position(y_pos);

  float const threshold = 0.1;

  while (true) {
    // auto mpu_data = mpu.read();

    // hal::print<1024>(
    //   console, "x: %fg, y: %fg, z: %fg\n", mpu_data.x, mpu_data.y,
    //   mpu_data.z);

    // // Will be changed because I don't know how the data looks like from the MPU
    // hal::degrees change_x = mpu_data.x / threshold;
    // hal::degrees change_y = mpu_data.y / threshold;

    // if (mpu_data.x > threshold || mpu_data.x < (threshold * -1)) {
    //   if (x_pos + change_x > rc_servo_settings.max_angle) {
    //     x_pos = 1.0;
    //   } else if (x_pos + change_x < rc_servo_settings.min_angle) {
    //     x_pos = -1.0;
    //   } else {
    //     x_pos += change_x;
    //   }

    //   servo_x.position(x_pos);
    // }

    // if (mpu_data.y > threshold || mpu_data.y < (threshold * -1)) {
    //   if (y_pos + change_y > rc_servo_settings.max_angle) {
    //     y_pos = 1.0;
    //   } else if (y_pos + change_y < rc_servo_settings.min_angle) {
    //     y_pos = -1.0;
    //   } else {
    //     y_pos += change_y;
    //   }

    //   servo_y.position(y_pos);
    // }

    // hal::delay(clock, 100ms);

//  servo_x.position(hal::degrees(-135));
//     hal::delay(clock, 1000ms);

    servo_x.position(hal::degrees(-90));
    hal::delay(clock, 1000ms);

    servo_x.position(hal::degrees(-45));
    hal::delay(clock, 1000ms);

    servo_x.position(hal::degrees(0));
    hal::delay(clock, 1000ms);

    servo_x.position(hal::degrees(45));
    hal::delay(clock, 1000ms);

    servo_x.position(hal::degrees(90));
    hal::delay(clock, 1000ms);

    // servo_x.position(hal::degrees(135));
    // hal::delay(clock, 1000ms);

    // servo_x.position(hal::degrees(180));
    // hal::delay(clock, 1000ms);

    // servo_x.position(hal::degrees(225));
    // hal::delay(clock, 1000ms);

    // servo_x.position(hal::degrees(270));
    // hal::delay(clock, 1000ms);

    // servo_x.position(hal::degrees(315));
    // hal::delay(clock, 1000ms);

    // servo_x.position(hal::degrees(360));
    // hal::delay(clock, 1000ms);
  }
}
