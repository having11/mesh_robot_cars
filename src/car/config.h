#define HIT_COOLDOWN 7000 //How many ms until robot can register new hit again
#define HIT_DISABLE 4000 //How many ms the robot will be unable to move once hit
#define MAG_THRESHOLD 2500 //0-4095 magnetometer value
#define MAX_HITS 10 //Number of hits before car becomes disabled
#define LED_BRIGHTNESS 70 //Number between 0-100 for RGB LED brightness
#define LASER_SERVO_HOR_MIN 0
#define LASER_SERVO_HOR_MAX 180
#define LASER_SERVO_VERT_MIN 60
#define LASER_SERVO_VERT_MAX 120

enum Commands
{
    NAME = 1,
    DRIVE,
    START,
    END,
    LASER_TOGGLE,
    SERVO,
    FREE_MODE,
    COLOR
};