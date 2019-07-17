#ifndef RoboCar_H
#define RoboCar_H

#include "Pin_Definitions.h"
#include "Particle.h"
#include <Arduino.h>
#include <U8x8lib.h>
#include <Wire.h>
#include "neopixel.h"
#include "config.h"

#define UID "CAR1"

class RoboCar
{
    public:
        RoboCar();
        bool run();
        void new_mesh_msg(const char *event, const char *data);
        void show_congrats(const char* uid);
        ~RoboCar();
    private:
        void display_data();
        void init_pins();
        void init_display();
        void drive(int x1, int x2); //TODO: Implement arcade-stick driving
        void change_rgb_color(uint32_t color);
        void change_rgb_color(String color);
        void move_laser(int deg_hor, int deg_vert);
        void start_match();
        void game_over();
        void enter_free_mode();
        void set_laser(bool val);
        void register_hit();
        bool check_if_hit();
        void cooldownTimeout();
        void disableTimeout();
        U8X8_SSD1306_128X64_NONAME_HW_I2C *display;
        Adafruit_NeoPixel *pixel;
        uint32_t user_color;
        pin_defines pins;
        int hits_received;
        Servo hor_servo;
        Servo vert_servo;
        Timer *cooldown_timer;
        Timer *disable_timer;
        bool motors_enabled, hits_enabled, match_running;
        const char* _name;
        const char* _uid;
};

#endif