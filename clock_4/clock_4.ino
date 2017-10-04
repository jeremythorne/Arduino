#include <stepper_28bjy48.h>

// Date and time functions using a DS1307 RTC connected via I2C and Wire lib

#include <Wire.h>
#include "RTClib.h"

RTC_DS1307 rtc;
typedef Stepper_28BJY48 Motor;

Motor motor(6,7,8,9, 1);

int first_second = -1;
int count = 0;

void time_to_turns(int hour, int minute, int second, Motor::Position & turns)
{
      //one minute cycle
      //goal.turns = second / 5;
      //goal.angle = (second % 5) * 512/5;
      
      //one hour cycle - 6 turns
      //goal.turns = now.minute() / 10;
      //goal.angle = (now.minute() % 10) * 512/10 + now.second() * 512/(600);
      
      //12 hour cycle - 8 turns
      hour = hour >= 12 ? hour - 12 : hour;
      turns.turns = hour * 2 / 3;
      hour -= turns.turns * 3 / 2;
      turns.angle = ((int)(hour * 341.3 + minute * 5.689)) % 512;
}

void print_time(int hour, int minute, int second)
{
  Serial.print(hour, DEC);
  Serial.print(":");
  Serial.print(minute, DEC);
  Serial.print(":");
  Serial.print(second, DEC);
}

void a_test(int hour, int minute, int second)
{
  Motor::Position goal;
  time_to_turns(hour, minute, second, goal);
  print_time(hour, minute, second);
  Serial.print("->");
  goal.print();
}

void advance_to_goal(Motor::Position goal)
{
  Serial.print("goal: ");
  goal.print();
  motor.setGoal(goal);
  while(1)
  {
    motor.advance();
    if(motor.atGoal()) return;
    delay(1);
  }
}

void advance_to_time(int hour, int minute, int second)
{
  Motor::Position goal;
  time_to_turns(hour, minute, second, goal);
  print_time(hour, minute, second);
  Serial.print("->");
  advance_to_goal(goal);
}

void test_time_to_turns()
{
  Serial.println("testing time to turns function:");  
  a_test(0, 0, 0);
  a_test(0, 5, 5);
  a_test(0, 10, 0);
  a_test(0, 50, 0);
  a_test(1, 0, 0);
  a_test(2, 0, 0);
  a_test(8, 0, 0);
  a_test(8, 59, 0);
  a_test(9, 0, 0);
  a_test(10, 0, 0);
  a_test(11, 0, 0);
  a_test(11, 59, 59);
  a_test(12, 0, 0);
}

void test_clock()
{
  Serial.println("testing clock");
  delay(10*1000);
  for(int hour = 0; hour < 12; hour++)
  {
    for(int minute = 0; minute < 60; minute++)
    {
      advance_to_time(hour, minute, 0);
    }
    delay(10*1000);
  }
}

void setup () {
  Serial.begin(57600);
#ifdef AVR
  Wire.begin();
#else
  Wire1.begin(); // Shield I2C pins connect to alt I2C bus on Arduino Due
#endif
  rtc.begin();

  test_time_to_turns();
  test_clock();
  Serial.print("done tests");
  
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

int was_at_goal = 0;
int at_goal = 0;

void loop () {
    was_at_goal = at_goal;
    if(count == 0)
    {  
      DateTime now = rtc.now();
      
      Serial.print(now.year(), DEC);
      Serial.print('/');
      Serial.print(now.month(), DEC);
      Serial.print('/');
      Serial.print(now.day(), DEC);
      Serial.print(' ');
      print_time(now.hour(), now.minute(), now.second());
      Serial.println();
          
      Motor::Position goal;

      
      if(first_second == -1) {
        first_second = now.second();
      }
     
      int second = now.second() - first_second;
      if(second < 0) second += 60; 
      
      time_to_turns(now.hour(), now.minute(), now.second(), goal);
      
      Serial.print("goal: ");
      goal.print();
      motor.setGoal(goal);
      
      Serial.println();
    }
    
    motor.advance();
    count++;
    at_goal = motor.atGoal();
    if(was_at_goal && at_goal) delay(10*1000); //wait 10 seconds
    if(count == 3000 || at_goal) count = 0;
    delay(1);
}

