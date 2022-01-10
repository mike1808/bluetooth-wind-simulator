#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Arduino.h>
#include <BluetoothSerial.h>
#include <Wire.h>

#include "config.h"

#if defined(ENABLE_DASH)
#include "dash.h"
#endif

#define BT_SERIAL_BUFSIZE 50
#define FPS 30

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
uint16_t fanDutyCycle = 0;
int drawEvery_mS;
BluetoothSerial SerialBT;

char btSerialBuffer[BT_SERIAL_BUFSIZE];

#if defined(ENABLE_DASH)
void drawBackgroud() {
  display.drawXBitmap(0, 0, dash_bits, dash_width, dash_height, SSD1306_WHITE);
}

void clearSection(int16_t x, int16_t y, int16_t width, int16_t height) {
  display.fillRect(x, y, width, height, SSD1306_BLACK);
}

void drawGear(const char *gear) {
  Serial.printf("Setting gear to %s\r\n", gear);

  clearSection(37, 30, 30, 30);
  display.setTextSize(4);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(37, 30);
  display.println(gear);
}

void drawRPM(int percent) {
  Serial.printf("Setting rpm to %d%%\r\n", percent);
  drawRevs(display, percent);
}

void drawSpeed(int speed) {
  Serial.printf("Setting speed to %d\r\n", speed);

  clearSection(80, 22, 40, 20);
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(80, 22);
  display.println(speed);
}

void drawLap(int lap) {
  Serial.printf("Setting lap to %d\r\n", lap);

  clearSection(90, 50, 28, 20);
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(90, 50);
  display.println(lap);
}

void resetDisplay() {
  display.clearDisplay();
  display.drawXBitmap(0, 0, dash_bits, dash_width, dash_height, SSD1306_WHITE);
  drawGear("0");
  drawSpeed(0);
  drawLap(0);
}
#endif

void controlFan(int power) {
  Serial.printf("Setting fan power to %d%%\r\n", power);
  fanDutyCycle = float(power) / 100 * ((1 << 16) - 1);
}

void processCommand() {
  char cmd = btSerialBuffer[0];

  switch (cmd) {
#if defined(ENABLE_DASH)
  case 'S': {
    int arg0;
    sscanf(&btSerialBuffer[1], "%d", &arg0);
    drawSpeed(arg0);
    break;
  }
  case 'R': {
    int arg0;
    sscanf(&btSerialBuffer[1], "%d", &arg0);
    drawRPM(arg0);
    break;
  }
  case 'G': {
    char str[BT_SERIAL_BUFSIZE];
    for (int i = 1, j = 0; i < BT_SERIAL_BUFSIZE; i++, j++) {
      if (btSerialBuffer[i] == '}') {
        break;
      }
      str[j] = btSerialBuffer[i];
    }
    drawGear(str);
    break;
  }
  case 'L': {
    int arg0;
    sscanf(&btSerialBuffer[1], "%d", &arg0);
    drawLap(arg0);
    break;
  }
  case 'C': {
    resetDisplay();
    break;
  }
#endif
  case 'W': {
    int arg0;
    sscanf(&btSerialBuffer[1], "%d", &arg0);
    controlFan(arg0);
    break;
  }
  default:
    Serial.printf("Unknown command: %c\r\n", cmd);
  }

#if defined(ENABLE_DASH)
  display.display();
#endif
}

void setup() {
  Serial.begin(115200);
  SerialBT.begin(BT_NAME);

#if defined(ENABLE_DASH)
  Wire.setPins(OLED_SDA, OLED_SLK);
  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;)
      ; // Don't proceed, loop forever
  }

  find_rev_limits();

  drawEvery_mS = float(1000) / FPS;

  Serial.printf("FPS = %d\r\n", drawEvery_mS);

  resetDisplay();
  display.display();
#endif

  ledcAttachPin(FAN_GPIO, FAN_PWM_Ch);
  ledcSetup(FAN_PWM_Ch, FAN_PWM_Freq, FAN_PWM_Res);
}

void loop() {
  if (SerialBT.available()) {
    SerialBT.readBytesUntil('{', btSerialBuffer, BT_SERIAL_BUFSIZE);
    int read = SerialBT.readBytesUntil('}', btSerialBuffer, BT_SERIAL_BUFSIZE);
    btSerialBuffer[min(read, BT_SERIAL_BUFSIZE - 1)] = 0x0;
    if (read > 0) {
      Serial.printf("got command: %s\r\n", btSerialBuffer);
      processCommand();
    }
  }

  ledcWrite(FAN_PWM_Ch, fanDutyCycle);
}
