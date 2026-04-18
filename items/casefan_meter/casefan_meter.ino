#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

/* IO settings */
#define PULSE_IN 6
#define I2C_SDA  0
#define I2C_SCL  1

/* OLED settings */
#define SCREEN_WIDTH    128
#define SCREEN_HEIGHT    32
#define OLED_RESET       -1
#define SCREEN_ADDRESS 0x3C

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

void setup() {
  pinMode(PULSE_IN, INPUT_PULLUP);

  Serial.begin(115200);
  for(int i = 0; i < 100 & !Serial; i++) delay(10);

  Wire.setSDA(I2C_SDA);
  Wire.setSCL(I2C_SCL);

  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }

  // Clear the buffer
  display.clearDisplay();
  display.display();
}

void loop() {
  uint16_t rpm;
  uint16_t ms_dn, ms_up;

  while(true) {
    while(digitalRead(PULSE_IN) ==  LOW);

    ms_up = millis();
    rpm = (uint16_t)(15000 / (ms_up - ms_dn));

    update_display(rpm);

    //----------------------------------------------------

    while(digitalRead(PULSE_IN) == HIGH);

    ms_dn = millis();
    rpm = (uint16_t)(15000 / (ms_dn - ms_up));

    update_display(rpm);
  }
}

void update_display(uint16_t rpm) {
  char buffer[10];
  sprintf(buffer, "%4d rpm", average(rpm));

  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE, SSD1306_BLACK);
  display.setCursor(15, 5);
  display.print(buffer);
  display.display();
}

// 移動平均を計算する関数
uint16_t average(uint16_t input) {
  static uint16_t array[4] = {0};
  int size = sizeof(array) / sizeof(array[0]);
  char buffer[32];

  for(int i = size - 1; 0 < i ; i--) {
    array[i] = array[i - 1];
  }

  array[0] = input;

  for(int i = 0; i < size; i++) {
    sprintf(buffer, "%4d ", array[i]);
    Serial.print(buffer);
  }

  uint32_t sum = 0;

  for(int i = 0; i < size; i++) {
    sum += array[i];
  }

  uint16_t res = sum / size;

  sprintf(buffer, "| res = %4d\n", res);
  Serial.print(buffer);

  return res;
}
