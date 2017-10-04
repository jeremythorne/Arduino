#include <stepper_28bjy48.h>

// Date and time functions using a DS1307 RTC connected via I2C and Wire lib

#include <Arduino.h>
#include <Wire.h>
#include "RTClib.h"

enum STATE {
  ANIM_BACK,
  ANIM_FWD,
  PLAY,
};

struct Time {
  int hour;
  int minute;
  int second;
};

typedef Stepper_28BJY48 Motor;

RTC_DS1307 rtc;
Motor motor(6,7,8,9, 1);
int first_second = -1;
int count = 0;
int was_at_goal = 0;
int at_goal = 0;
int last_hour = 0;
STATE state = PLAY;

struct Time now()
{
  DateTime now = rtc.now();
  struct Time time;
  time.hour = now.hour();
  time.minute = now.minute();
  time.second = now.second();
  return time;
}

void time_to_turns(struct Time t, Motor::Position & turns)
{
      //one minute cycle
      //goal.turns = second / 5;
      //goal.angle = (second % 5) * 512/5;
      
      //one hour cycle - 6 turns
      //goal.turns = now.minute() / 10;
      //goal.angle = (now.minute() % 10) * 512/10 + now.second() * 512/(600);
      
      //12 hour cycle - 8 turns
      t.hour = t.hour >= 12 ? t.hour - 12 : t.hour;
      turns.turns = t.hour * 0.666 + 0.0111 * t.minute;
      //t.hour -= turns.turns * 3 / 2;
      turns.angle = ((int)(t.hour * 341.3 + t.minute * 5.689)) % 512;
}

void print_time(struct Time t)
{
  Serial.print(t.hour, DEC);
  Serial.print(":");
  Serial.print(t.minute, DEC);
  Serial.print(":");
  Serial.print(t.second, DEC);
}

void next_state()
{
  struct Time t = now();
  
  Motor::Position goal;
  bool set_goal = false;
  if(at_goal)
  {
    STATE new_state = state;
    switch(state)
    {
    case ANIM_BACK:
    {
      new_state = ANIM_FWD;
      struct Time t_plus_15 = t;
      t_plus_15.minute += 15;
      time_to_turns(t_plus_15, goal);
      set_goal = true;
      break;
    }
    case ANIM_FWD:
    {
      new_state = PLAY;
      break;
    }
    case PLAY:
    {
      if(t.hour != last_hour && t.hour != 12 && t.hour != 0)
      {
        new_state = ANIM_BACK;
        struct Time t_minus_15 = t;
        t_minus_15.hour -= 1;
        t_minus_15.minute += 45;
        time_to_turns(t_minus_15, goal);
        last_hour = t.hour;
        set_goal = true;
      }
      break;
    }
    };
    state = new_state;
  }
  
  if(state == PLAY)
  {
    time_to_turns(t, goal);
    set_goal = true;
  }

  if(set_goal)
  {
    print_time(t);
    Serial.print("->");
    goal.print();
    motor.setGoal(goal);
  }
}



void a_test(int hour, int minute, int second)
{
  struct Time t;
  t.hour = hour;
  t.minute = minute;
  t.second = second;
  Motor::Position goal;
  time_to_turns(t, goal);
  print_time(t);
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

void advance_to_time(struct Time t)
{
  Motor::Position goal;
  time_to_turns(t, goal);
  print_time(t);
  Serial.print("->");
  advance_to_goal(goal);
}

void test_time_to_turns()
{
  Serial.println("testing time to turns function:");  
  a_test(0, 0, 0);
  a_test(0, 5, 5);
  a_test(0, 10, 0);
  a_test(0, 20, 0);
  a_test(0, 30, 0);
  a_test(0, 40, 0);
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
      struct Time t = {hour, minute, 0};
      a_test(hour, minute, 0);
      advance_to_time(t);
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
  
  //rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  rtc.adjust(DateTime(2017, 2, 6, 1, 50, 0));
  Serial.print("time now: ");
  print_time(now());
  Serial.println("");

  test_time_to_turns();
  test_clock();
  Serial.print("done tests");
  
  
  if (! rtc.isrunning()) {
    Serial.println("RTC is NOT running!");
    // following line sets the RTC to the date & time this sketch was compiled
    //rtc.adjust(Datestruct Time(F(__DATE__), F(__TIME__)));
    // This line sets the RTC with an explicit date & time, for example to set
    // January 21, 2014 at 3am you would call:
    // rtc.adjust(Datestruct Time(2014, 1, 21, 3, 0, 0));
  }
} 



void loop () {
    was_at_goal = at_goal;
    if(count == 0)
    {  
      next_state();
    }
    
    motor.advance();
    count++;
    at_goal = motor.atGoal();
    if(was_at_goal && at_goal) delay(10*1000); //wait 10 seconds
    if(count == 3000 || at_goal) count = 0;
    delay(1);
}


