#include <TFT_eSPI.h>

TFT_eSPI tft = TFT_eSPI();

// Переменные для анимации барабанов
int reel1_pos = 0;
int reel2_pos = 0; 
int reel3_pos = 0;
int reel1_target = 2;
int reel2_target = 2;
int reel3_target = 2;
bool spinning = false;
bool startSpin = false;
unsigned long lastAnim = 0;
unsigned long spinStart = 0;

// Символы для барабанов
String symbols[6] = {"CER", "BAR", "7", "STR", "DIA", "LMN"};
uint16_t symbolColors[6] = {TFT_RED, TFT_WHITE, TFT_RED, TFT_YELLOW, TFT_CYAN, TFT_YELLOW};

// Пины для подключения
const int TOUCH_BUTTON_PIN = 16;  // D0 на Wemos D1 Mini (GPIO16)
const int LED_PIN = 2;           // D4 на Wemos D1 Mini (GPIO2)

// Переменные для обработки сенсорной кнопки
bool lastTouchState = false;
unsigned long lastTouchTime = 0;
const int TOUCH_DEBOUNCE = 500;

void setup() {
  Serial.begin(115200);
  Serial.println("MELLSTROY.GAME Starting...");
  
  tft.init();
  tft.setRotation(1);
  tft.fillScreen(TFT_WHITE);
  
  pinMode(LED_PIN, OUTPUT);
  pinMode(TOUCH_BUTTON_PIN, INPUT);
  
  // Сигнал запуска
  for(int i = 0; i < 3; i++) {
    digitalWrite(LED_PIN, LOW); delay(100);
    digitalWrite(LED_PIN, HIGH); delay(100);
  }
  
  drawLogo();
  drawReels();
  drawReel(20, 50, reel1_pos);
  drawReel(90, 50, reel2_pos);
  drawReel(160, 50, reel3_pos);
  showInstructions();
  
  Serial.println("System READY - Touch button to SPIN!");
}

void drawLogo() {
  tft.setTextColor(TFT_BLACK);
  tft.setTextSize(2);
  tft.setCursor(35, 8);
  tft.println("MELLSTROY.GAME");
  tft.drawFastHLine(10, 32, 220, TFT_BLACK);
}

void drawReels() {
  tft.drawRect(20, 50, 60, 80, TFT_BLACK);
  tft.drawRect(90, 50, 60, 80, TFT_BLACK);
  tft.drawRect(160, 50, 60, 80, TFT_BLACK);
  
  tft.setTextColor(TFT_BLACK);
  tft.setTextSize(2);
  tft.setCursor(95, 35);
  tft.println("KAZIK");
}

void showInstructions() {
  tft.setTextColor(TFT_BLUE);
  tft.setTextSize(1);
  tft.setCursor(60, 140);
  tft.println("Touch button for SPIN!");
}

// ... (остальные функции остаются такими же как в рабочем коде)