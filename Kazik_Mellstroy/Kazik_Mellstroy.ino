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

// Пины для подключения - ПРОБУЕМ D0
const int TOUCH_BUTTON_PIN = 16;  // D0 на Wemos D1 Mini (GPIO16)
const int LED_PIN = 2;           // D4 на Wemos D1 Mini (GPIO2)

// Переменные для обработки сенсорной кнопки
bool lastTouchState = false;
unsigned long lastTouchTime = 0;
const int TOUCH_DEBOUNCE = 500; // Защита от повторного нажатия (мс)

void setup() {
  Serial.begin(115200);
  Serial.println("Запуск системы MELLSTROY.GAME...");
  
  // Инициализация дисплея
  tft.init();
  tft.setRotation(1);
  tft.fillScreen(TFT_WHITE);
  
  // Настройка пинов
  pinMode(LED_PIN, OUTPUT);
  pinMode(TOUCH_BUTTON_PIN, INPUT); // Кнопка как вход
  
  // Сигнал о успешном запуске
  for(int i = 0; i < 3; i++) {
    digitalWrite(LED_PIN, LOW);
    delay(100);
    digitalWrite(LED_PIN, HIGH);
    delay(100);
  }
  
  // Рисуем логотип и барабаны
  drawLogo();
  drawReels();
  drawReel(20, 50, reel1_pos);
  drawReel(90, 50, reel2_pos);
  drawReel(160, 50, reel3_pos);
  
  // Показываем инструкцию
  showInstructions();
  
  Serial.println("=== СИСТЕМА ГОТОВА ===");
  Serial.println("Кнопка подключена к D0 (GPIO16)");
  Serial.println("Коснитесь сенсорной кнопки для запуска барабанов!");
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
  tft.println("Коснитесь кнопки для SPIN!");
}

void clearInstructions() {
  tft.fillRect(50, 140, 140, 20, TFT_WHITE);
}

void drawReel(int x, int y, int position) {
  tft.fillRect(x + 2, y + 2, 56, 76, TFT_WHITE);
  
  for (int i = -1; i <= 1; i++) {
    int symbolIndex = (position + i + 6) % 6;
    int symbolY = y + 40 + i * 25;
    
    tft.setTextColor(symbolColors[symbolIndex]);
    
    if (symbols[symbolIndex] == "7") {
      tft.setTextColor(TFT_RED);
      tft.setTextSize(3);
      tft.setCursor(x + 25, symbolY - 5);
      tft.println("7");
    } else if (symbols[symbolIndex] == "BAR") {
      tft.setTextSize(1);
      tft.setCursor(x + 20, symbolY);
      tft.println("BAR");
    } else {
      tft.setTextSize(1);
      tft.setCursor(x + 15, symbolY);
      tft.println(symbols[symbolIndex]);
    }
  }
  
  tft.drawFastHLine(x + 5, y + 40, 50, TFT_BLACK);
  tft.drawFastHLine(x + 5, y + 41, 50, TFT_BLACK);
}

void startSpinning() {
  if (!spinning && !startSpin) {
    startSpin = true;
    spinning = true;
    spinStart = millis();
    lastAnim = millis();
    
    // Генерируем случайные целевые позиции
    reel1_target = random(0, 6);
    reel2_target = random(0, 6);
    reel3_target = random(0, 6);
    
    clearInstructions();
    
    // Показываем сообщение о вращении
    tft.setTextColor(TFT_ORANGE);
    tft.setTextSize(1);
    tft.setCursor(80, 140);
    tft.println("БАРАБАНЫ КРУТЯТСЯ!");
    
    Serial.println(">>> БАРАБАНЫ ЗАПУЩЕНЫ! <<<");
    
    // Сигнал светодиодом при запуске
    digitalWrite(LED_PIN, LOW);
    delay(50);
    digitalWrite(LED_PIN, HIGH);
  }
}

void spinReels() {
  if (spinning) {
    unsigned long currentTime = millis();
    unsigned long elapsed = currentTime - spinStart;
    
    // Вращаем барабаны с переменной скоростью
    int animSpeed = getAnimationSpeed(elapsed);
    
    if (currentTime - lastAnim >= animSpeed) {
      reel1_pos = (reel1_pos + 1) % 6;
      reel2_pos = (reel2_pos + 1) % 6;
      reel3_pos = (reel3_pos + 1) % 6;
      lastAnim = currentTime;
    }
    
    // Останавливаем через 2.5 секунды
    if (elapsed > 2500) {
      spinning = false;
      startSpin = false;
      reel1_pos = reel1_target;
      reel2_pos = reel2_target;
      reel3_pos = reel3_target;
      
      // Немедленная перерисовка
      drawReel(20, 50, reel1_pos);
      drawReel(90, 50, reel2_pos);
      drawReel(160, 50, reel3_pos);
      
      showResult();
      
      // Сигнал светодиодом
      for(int i = 0; i < 3; i++) {
        digitalWrite(LED_PIN, LOW);
        delay(150);
        digitalWrite(LED_PIN, HIGH);
        delay(150);
      }
    }
  }
}

int getAnimationSpeed(unsigned long elapsed) {
  if (elapsed < 800) return 60;
  else if (elapsed < 1500) return 80;
  else if (elapsed < 2000) return 120;
  else return 200;
}

void showResult() {
  bool isWin = (reel1_pos == reel2_pos && reel2_pos == reel3_pos);
  
  tft.fillRect(50, 140, 140, 40, TFT_WHITE);
  
  if (isWin) {
    showWinAnimation();
    Serial.println("*** ВЫ ВЫИГРАЛИ! ***");
  } else {
    tft.setTextColor(TFT_RED);
    tft.setTextSize(2);
    tft.setCursor(85, 145);
    tft.println("ПРОИГРЫШ");
    Serial.println("--- Попробуйте еще раз! ---");
  }
}

void showWinAnimation() {
  for(int i = 0; i < 6; i++) {
    tft.setTextColor(i % 2 == 0 ? TFT_GREEN : TFT_WHITE);
    tft.setTextSize(2);
    tft.setCursor(70, 145);
    tft.println("ВЫ ВЫИГРАЛИ!");
    
    digitalWrite(LED_PIN, i % 2 == 0 ? LOW : HIGH);
    delay(250);
  }
  digitalWrite(LED_PIN, HIGH);
}

void checkTouchButton() {
  bool currentTouchState = digitalRead(TOUCH_BUTTON_PIN);
  unsigned long currentTime = millis();
  
  // Обнаружение нажатия
  if (currentTouchState && !lastTouchState) {
    if (currentTime - lastTouchTime > TOUCH_DEBOUNCE) {
      Serial.println(">>> КНОПКА НАЖАТА! ЗАПУСК БАРАБАНОВ <<<");
      startSpinning();
      lastTouchTime = currentTime;
    }
  }
  
  lastTouchState = currentTouchState;
}

void loop() {
  // Проверяем сенсорную кнопку
  checkTouchButton();
  
  // Анимация барабанов
  if (spinning) {
    spinReels();
  }
  
  // Перерисовываем барабаны только если вращаются
  if (spinning) {
    drawReel(20, 50, reel1_pos);
    drawReel(90, 50, reel2_pos);
    drawReel(160, 50, reel3_pos);
  }
  
  // Мигание светодиода во время вращения
  static unsigned long lastLedBlink = 0;
  
  if (spinning) {
    if (millis() - lastLedBlink > 80) {
      digitalWrite(LED_PIN, !digitalRead(LED_PIN));
      lastLedBlink = millis();
    }
  } else {
    digitalWrite(LED_PIN, HIGH);
  }
  
  delay(10);
}