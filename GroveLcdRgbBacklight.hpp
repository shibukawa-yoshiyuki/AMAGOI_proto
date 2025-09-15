#ifndef GROVE_LCD_RGB_BACKLIGHT_H
#define GROVE_LCD_RGB_BACKLIGHT_H
//
// Filename :	GroveLcdRgbBacklight.hpp
// Abstruct :	Class definition for Grove Lcd
// Author   :	application_division@atit.jp
// Update   :	2025/09/16  New Creation
#include <Wire.h>
#include "rgb_lcd.h"

namespace AMAGOI {
//
// Class    :	GroveLcdRgbBacklight
// Abstruct :	Class definition for Grove Lcd
class GroveLcdRgbBacklight {
    // Definition of variable
private:
  rgb_lcd*    lcd;                  // lcdクラスインスタンス
    // Definition of method
private:
  void writeLine( char*, char* );   // method:LCD画面表示
public:
  GroveLcdRgbBacklight( TwoWire* ); // method:コンストラクタ
  void clearLcd();                  // method:LCD画面クリア
  void writeLine( char** );         // method:LCD画面表示
  void writeLine( char* );          // method:LCD画面表示
};
}
#endif
