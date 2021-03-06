#include <stepper_28bjy48.h>

// Date and time functions using a DS1307 RTC connected via I2C and Wire lib

#include <Wire.h>
#include "RTClib.h"

RTC_DS1307 rtc;
Stepper_28BJY48 motor(6,7,8,9, 1);

void setup () {
  Serial.begin(57600);
#ifdef AVR
  Wire.begin();
#else
  Wire1.begin(); // Shield I2C pins connect to alt I2C bus on Arduino Due
#endif
  rtc.begin();
  
  rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  
  if (! rtc.isrunning()) {
    Serial.println("RTC is NOT running!");
    // following line sets the RTC to the date & time this sketch was compiled
    //rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    // This line sets the RTC with an explicit date & time, for example to set
    // January 21, 2014 at 3am you would call:
    // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
  }
}

void loop () {
    DateTime now = rtc.now();
    
    Serial.print(now.year(), DEC);
    Serial.print('/');
    Serial.print(now.month(), DEC);
    Serial.print('/');
    Serial.print(now.day(), DEC);
    Serial.print(' ');
    Serial.print(now.hour(), DEC);
    Serial.print(':');
    Serial.print(now.minute(), DEC);
    Serial.print(':');
    Serial.print(now.second(), DEC);
    Serial.println();
    
    Stepper_28BJY48::Position goal;
    //goal.turns = now.hour() > 12 ? now.hour() - 12 : now.hour();
    //goal.angle = now.minute() * 8.533 + now.second() * 0.142;
    
    goal.turns = now.second() / 5;
    goal.angle = (now.second() % 5) * 512;
    
    motor.moveTo(goal);
    
    Serial.println();
    delay(3000);
}

