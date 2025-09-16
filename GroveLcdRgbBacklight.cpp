//
// Filename :   GroveLcdRgbBacklight.cpp
// Abstruct :   Method for GroveLcdRgbBacklight class
// Author   :   application_division@atit.jp
// Update   :   2025/09/16  New Creation
#include <Arduino.h>
#include "GroveLcdRgbBacklight.hpp"

using namespace AMAGOI;
//
// Method   :   GroveLcdRgbBacklight
// Abstruct :   コンストラクタ
// Argument :   n/a
GroveLcdRgbBacklight::GroveLcdRgbBacklight( TwoWire* wire ) {
  // LCDモニタ初期化
  this->lcd = new rgb_lcd();
  this->lcd->begin( (uint8_t)16, (uint8_t)2, (uint8_t)LCD_5x8DOTS, *wire );
}

//
// Method   :   clearLcd
// Abstruct :   LCD画面クリア
// Argument :   n/a
// Return   :   n/a
void GroveLcdRgbBacklight::clearLcd() {
    this->lcd->clear();
}

//
// Method   :   writeLine
// Abstruct :   LCD画面表示
// Argument :   char** text     : 表示文字列
// Return   :   n/a
void GroveLcdRgbBacklight::writeLine( char** text ) {
  char firstLine[16+1]  = "";
  char secondLine[16+1] = "";
  
  strncpy( firstLine,  text[0], 16 );
  strncpy( secondLine, text[1], 16 );
  
  this->writeLine( firstLine, secondLine );
  return;
}

//
// Method   :   writeLine
// Abstruct :   LCD画面表示
// Argument :   char* text      : 表示文字列
// Return   :   n/a
void GroveLcdRgbBacklight::writeLine( char* text ) {
  char firstLine[16+1]  = "";
  char secondLine[16+1] = "";

  strncpy( firstLine, text, 16 );
  if( strlen_P( text ) > 16 ) {
    strncpy( secondLine, text+16, 16 );
  }

  this->writeLine( firstLine, secondLine );
  return;
}

//
// Method   :   writeLine
// Abstruct :   LCD画面表示
// Argument :   char* firstLine : 1行目文字列
//              char* secondLine: 2行目文字列
// Return   :   n/a
void GroveLcdRgbBacklight::writeLine( char* firstLine, char* secondLine ) {
  // 画面表示
  this->clearLcd();
  this->lcd->setCursor( 0, 0 );
  this->lcd->print( firstLine );
  this->lcd->setCursor( 0, 1 );
  this->lcd->print( secondLine );

  return;
}
