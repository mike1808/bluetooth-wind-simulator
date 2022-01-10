#define FAN_GPIO 5
#define FAN_PWM_Ch 0
#define FAN_PWM_Res 16
#define FAN_PWM_Freq 25000 // 25 kHz

#ifndef ENABLE_DASH
#define ENABLE_DASH 1
#endif

#define OLED_SLK 22
#define OLED_SDA 21
#define OLED_RESET 16
#define SCREEN_ADDRESS                                                         \
  0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

#define BT_NAME "SimHub Dash"