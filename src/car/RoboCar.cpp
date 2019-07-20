#include "RoboCar.h"

RoboCar::RoboCar(){
    _uid = UID;
    _name = _uid;
    init_pins();
    display = new U8X8_SSD1306_128X64_NONAME_HW_I2C(U8X8_PIN_NONE);
    pixel = new Adafruit_NeoPixel(1, pins.RGB_LED, WS2812B);
    cooldown_timer = new Timer(HIT_COOLDOWN, &RoboCar::cooldownTimeout, *this, true);
    disable_timer = new Timer(HIT_DISABLE, &RoboCar::disableTimeout, *this, true);
    init_display();
    motors_enabled = false;
    hits_enabled = false;
    match_running = false;
    user_color = pixel->Color(0,0,0);
    pixel->begin();
    pixel->setBrightness(LED_BRIGHTNESS);
    change_rgb_color(user_color);
    display->clear();
    display->drawString(0,0,"Name: "); 
    display->drawString(6,0,"          ");
    display->drawString(6,0,_name);
    enter_free_mode();
}

void RoboCar::start_match(){
    match_running = true;
    motors_enabled = true;
    hits_enabled = true;
    hits_received = 0;
}

bool RoboCar::run(){
    Particle.process();
    //Serial.println(match_running);
    if(check_if_hit()){
        Serial.println("HIT");
        register_hit();
        delay(10);
    }
    display_data();
    return 1;
}

void RoboCar::show_congrats(const char* uid){
    display->clear();
    display->print("Congrats "); display->print(uid);
    change_rgb_color(pixel->Color(0,50,240));
    delay(8000);
    match_running = false;
}

void RoboCar::display_data(){
    //List name, hits received
    //display->setCursor(0, 70);
    display->drawString(0,4,"Hits received: "); 
    //display->setCursor(0, 100);
    String hit_str = String(hits_received);
    display->drawString(0,6,hit_str.c_str());
}

void RoboCar::change_rgb_color(uint32_t color){
    pixel->setPixelColor(0, color);
    pixel->show();
}

void RoboCar::change_rgb_color(String color_str){
    uint32_t color = strtoul(color_str, NULL, 16);
    user_color = color;
    pixel->setPixelColor(0, color);
    pixel->show();
    Serial.println("User color set");
}

void RoboCar::set_laser(bool val){
    digitalWrite(pins.LASER, val);
}

void RoboCar::move_laser(int deg_hor, int deg_vert){
    if(deg_hor >= LASER_SERVO_HOR_MIN && deg_hor <= LASER_SERVO_HOR_MAX)
    hor_servo.write(deg_hor);
    if(deg_vert >= LASER_SERVO_VERT_MIN && deg_vert <= LASER_SERVO_VERT_MAX)
    vert_servo.write(deg_vert);
}

bool RoboCar::check_if_hit(){
    //Serial.printlnf("MAG reading: %i", analogRead(pins.MAGNETOMETER));
    //Serial.println(match_running);
    if(analogRead(pins.MAGNETOMETER)>=MAG_THRESHOLD) return 1 * hits_enabled;
    return 0;
}

void RoboCar::enter_free_mode(){
    motors_enabled = true;
    change_rgb_color(user_color);
}

void RoboCar::drive(int x1, int x2){
    if(motors_enabled){
        if(!x2){
            analogWrite(pins.MOTOR_IN1, 0);
            analogWrite(pins.MOTOR_IN2, 0);
        }
        else if (x2 < 0){
            analogWrite(pins.MOTOR_IN1, 0);
            analogWrite(pins.MOTOR_IN2, -x2);
        }
        else{
            analogWrite(pins.MOTOR_IN1, x2);
            analogWrite(pins.MOTOR_IN2, 0);
        }
        if(!x1){
            analogWrite(pins.MOTOR_IN3, 0);
            analogWrite(pins.MOTOR_IN4, 0);
        }
        else if (x1 < 0){
            analogWrite(pins.MOTOR_IN3, 0);
            analogWrite(pins.MOTOR_IN4, -x1);
        }
        else{
            analogWrite(pins.MOTOR_IN3, x1);
            analogWrite(pins.MOTOR_IN4, 0);
        }
        delay(250);
        analogWrite(pins.MOTOR_IN1, 0);
        analogWrite(pins.MOTOR_IN2, 0);
        analogWrite(pins.MOTOR_IN3, 0);
        analogWrite(pins.MOTOR_IN4, 0);
    }
    else{
        analogWrite(pins.MOTOR_IN1, 0);
        analogWrite(pins.MOTOR_IN2, 0);
        analogWrite(pins.MOTOR_IN3, 0);
        analogWrite(pins.MOTOR_IN4, 0);
    }
}

void RoboCar::game_over(){
    display->clear();
    display->print("GAME OVER");
    Mesh.publish("OUT", _uid);
    match_running = true;
    motors_enabled = false;
    hits_enabled = false;
    while(match_running){
        Particle.process();
    }
    display->clear();
    display->drawString(0,0,"Name: "); 
    display->drawString(6,0,"          ");
    display->drawString(6,0,_name);
    enter_free_mode();
}

void RoboCar::register_hit(){
    hits_received++;
    motors_enabled = false;
    hits_enabled = false;
    if(hits_received>=MAX_HITS){
        hits_received = 0;
        change_rgb_color(pixel->Color(0,0,0));
        game_over();
    }
    change_rgb_color(pixel->Color(240,10,0));
    cooldown_timer->reset();
    disable_timer->reset();
}

void RoboCar::disableTimeout(){
    change_rgb_color(pixel->Color(50,50,0));
    motors_enabled = true;
}

void RoboCar::cooldownTimeout(){
    change_rgb_color(user_color);
    hits_enabled = true;
}

void RoboCar::new_mesh_msg(const char *event, const char *data){
    Serial.printlnf("Event: %s | Data: %s", event, data ? data : "NULL");
    int command = String(data).substring(0,2).toInt(); //Take chars 0 and 1 for command int
    Serial.printlnf("Command: %i", command);
    switch(command){
        case 1: //1
        _name = String(data).substring(2).c_str();
        display->drawString(0,0,"Name: "); 
        display->drawString(6,0,"          ");
        display->drawString(6,0,_name);
        Serial.printlnf("Name is now %s.", _name);
        break;
        case 3: //3
        start_match();
        Serial.println("Match is starting");
        break;
        case 4: //4
        match_running = false;
        display->clear();
        display->drawString(0,0,"Name: "); 
        display->drawString(6,0,"          ");
        display->drawString(6,0,_name);
        enter_free_mode();
        break;
        case 2: //2
        drive(String(data).substring(2,6).toInt(), String(data).substring(6,10).toInt());
        Serial.printlnf("Driving with vals %i | %i", String(data).substring(2,6).toInt(), String(data).substring(6,10).toInt());
        break;
        case 5: //5
        set_laser(String(data).substring(2).toInt());
        Serial.printlnf("Setting laser to %i", String(data).substring(2).toInt());
        break;
        case 6: //6
        move_laser(String(data).substring(2,5).toInt(), String(data).substring(5,8).toInt());
        Serial.printlnf("Moving servos to %i | %i", String(data).substring(2,5).toInt(), String(data).substring(5,8).toInt());
        break;
        case 7: //7
        enter_free_mode();
        break;
        case 8: //8
        change_rgb_color(String(data).substring(2));
        Serial.print("Changing color to: "); Serial.println(user_color, HEX);
        break;
    }
}

void RoboCar::init_pins(){
    pinMode(pins.MOTOR_IN1, OUTPUT);
    pinMode(pins.MOTOR_IN2, OUTPUT);
    pinMode(pins.MOTOR_IN3, OUTPUT);
    pinMode(pins.MOTOR_IN4, OUTPUT);
    pinMode(pins.SERVO_PIN1, OUTPUT);
    pinMode(pins.SERVO_PIN2, OUTPUT);
    pinMode(pins.LASER, OUTPUT);
    pinMode(pins.MAGNETOMETER, OUTPUT);
    pinMode(pins.RGB_LED, OUTPUT);
    pinMode(pins.LINE1, INPUT);
    pinMode(pins.LINE2, INPUT);
    pinMode(pins.LINE3, INPUT);
    hor_servo.attach(pins.SERVO_PIN1);
    vert_servo.attach(pins.SERVO_PIN2);
    hor_servo.write(90);
    vert_servo.write(90);

}

void RoboCar::init_display(){
    display->begin();
    display->setFont(u8x8_font_8x13B_1x2_f);
    display->setPowerSave(0);
}

RoboCar::~RoboCar(){
}