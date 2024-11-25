#include <TM1638plus.h> 
#include <TFT_eSPI.h>
#include <SPI.h>
#include <Wire.h>

  // Библиотека для работы с TM1638

 // Определим пины для подключения
#define DATA_PIN  32  // Пин данных
#define CLOCK_PIN 33  // Пин тактового сигнала
#define STROBE_PIN 25 // Пин строба (активирует дисплей)

TM1638plus tm1638(DATA_PIN, CLOCK_PIN, STROBE_PIN);  // Инициализация модуля TM1638

// Структура для хранения интервала работы компрессора
struct CompressorInterval {
  int startHour;
  int startMinute;
  int endHour;
  int endMinute;
};

class CompressorMenu {
private:
  TFT_eSPI tft = TFT_eSPI();

  // Массив интервалов (максимум 5 интервалов)
  CompressorInterval intervals[5];
  int intervalCount = 0;

  // Режимы работы компрессора
  enum CompressorMode {
    CONTINUOUS,
    SCHEDULED,
    MANUAL
  };

  CompressorMode currentMode = SCHEDULED;
  
  // Текущий выбранный элемент для редактирования
  int selectedItem = 0;
  int editField = 0; // 0: startHour, 1: startMinute, 2: endHour, 3: endMinute

public:
  void begin() {
    tft.init();
    tft.setRotation(1);
    tft.fillScreen(TFT_BLACK);
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
  }

  void drawMenu() {
    tft.fillScreen(TFT_BLACK);
    tft.setCursor(10, 10);
    tft.setTextSize(2);
    tft.println("Compressor Settings");

    // Отображение режима работы
    tft.setTextSize(2);
    tft.setCursor(10, 40);
    tft.print("Mode: ");
    switch(currentMode) {
      case CONTINUOUS: tft.println("Continuous"); break;
      case SCHEDULED: tft.println("Scheduled"); break;
      case MANUAL: tft.println("Manual"); break;
    }

    // Отображение интервалов
    for (int i = 0; i < intervalCount; i++) {
      tft.setCursor(10, 60 + i * 20);
      tft.printf("%d: %02d:%02d - %02d:%02d", 
        i + 1, 
        intervals[i].startHour, 
        intervals[i].startMinute, 
        intervals[i].endHour, 
        intervals[i].endMinute
      );

      // Выделение текущего выбранного элемента
      if (selectedItem == i) {
        tft.drawRect(8, 58 + i * 20, tft.width() - 16, 16, TFT_RED);
      }
    }

    // Кнопки управления
    tft.setCursor(10, tft.height() - 30);
    tft.println("BTN1:Select BTN2:Edit BTN3:Add");
  }

  void changeMode() {
    currentMode = static_cast<CompressorMode>((currentMode + 1) % 3);
    drawMenu();
  }

  void selectNextInterval() {
    selectedItem = (selectedItem + 1) % intervalCount;
    drawMenu();
  }

  void addNewInterval() {
    if (intervalCount < 5) {
      intervals[intervalCount] = {0, 0, 0, 0};
      selectedItem = intervalCount;
      intervalCount++;
      drawMenu();
    }
  }

  void editInterval() {
    if (intervalCount == 0) return;

    CompressorInterval& current = intervals[selectedItem];
    
    switch(editField) {
      case 0: current.startHour = (current.startHour + 1) % 24; break;
      case 1: current.startMinute = (current.startMinute + 1) % 60; break;
      case 2: current.endHour = (current.endHour + 1) % 24; break;
      case 3: current.endMinute = (current.endMinute + 1) % 60; break;
    }

    drawMenu();
    editField = (editField + 1) % 4;
  }

  // Возвращает текущую конфигурацию
  CompressorMode getMode() {
    return currentMode;
  }

  CompressorInterval* getIntervals() {
    return intervals;
  }

  int getIntervalCount() {
    return intervalCount;
  }
};

// Глобальный объект меню
CompressorMenu compressorMenu;

void setup() {
  Serial.begin(115200);
  tm1638.begin();
  tm1638.setBrightness(7);  // Устанавливаем яркость (от 0 до 7)
  Serial.println("TM1638 Button Press Example");
  compressorMenu.begin();
  compressorMenu.drawMenu();
  tm1638.displayDigit(4, 8);

}

void loop() {
  // Здесь будет логика кнопок 
  // Примерная реализация:
//   if (digitalRead(BUTTON1) == LOW) {
//     compressorMenu.selectNextInterval();
//     delay(200);
//   }
  
//   if (digitalRead(BUTTON2) == LOW) {
//     compressorMenu.changeMode();
//     delay(200);
//   }
  
//   if (digitalRead(BUTTON3) == LOW) {
//     compressorMenu.addNewInterval();
//     delay(200);
//   }
uint8_t buttons = tm1638.readButtons();  // Чтение состояния кнопок
uint8_t buttonNumber = 0;
// Обрабатываем каждую кнопку, используя switch-case
switch (buttons) {
  case 0x01:  // Кнопка 1 нажата
    Serial.println("Button 1 pressed");
    buttonNumber = 1;
    compressorMenu.selectNextInterval();
    break;
    
  case 0x02:  // Кнопка 2 нажата
    Serial.println("Button 2 pressed");
    buttonNumber = 2;
    ompressorMenu.changeMode();
    break;
    
  case 0x04:  // Кнопка 3 нажата
    Serial.println("Button 3 pressed");
    buttonNumber = 3;
    compressorMenu.addNewInterval();
    break;
    
  case 0x08:  // Кнопка 4 нажата
    Serial.println("Button 4 pressed");
    buttonNumber = 4;
    break;
    
  case 0x10:  // Кнопка 5 нажата
    Serial.println("Button 5 pressed");
    buttonNumber = 5;
    break;
    
  case 0x20:  // Кнопка 6 нажата
    Serial.println("Button 6 pressed");
    buttonNumber = 6;
    break;
    
  case 0x40:  // Кнопка 7 нажата
    Serial.println("Button 7 pressed");
    buttonNumber = 7;
    break;
    
  case 0x80:  // Кнопка 8 нажата
    Serial.println("Button 8 pressed");
    buttonNumber = 8;
    break;

  default:  // Нет нажатой кнопки
  // Можно добавить какой-то код для действия по умолчанию
  break;
}
// Если была нажата кнопка, отображаем номер на 7-сегментном дисплее
if (buttonNumber > 0) {
  tm1638.displayDigit(0, buttonNumber);  // Отображаем число на первой цифре дисплея
} else {
  //tm1638.clearDisplay();  // Если кнопка не нажата, очищаем дисплей
}
  delay(100);  // Небольшая задержка, чтобы не нагружать процессор
}