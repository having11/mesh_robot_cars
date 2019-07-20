#include <Arduino.h>

struct pin_defines {
    static const uint8_t MOTOR_IN1 = 7, MOTOR_IN2 = 6, MOTOR_IN3 = 4, MOTOR_IN4 = 5;
    static const uint8_t SERVO_PIN1 = 3, SERVO_PIN2 = 2;
    static const uint8_t LASER = A4, MAGNETOMETER = A5, RGB_LED = A0;
    static const uint8_t LINE1 = A1, LINE2 = A2, LINE3 = A3;
};