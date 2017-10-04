#include "stepper_28bjy48.h"

#include <assert.h>

#if (ARDUINO >= 100)
 #include <Arduino.h> // capital A so it is error prone on case-sensitive filesystems
#else
 #include <WProgram.h>
#endif

namespace {
    const int countsperrev = 512;
    const int lookup[8] = {B01000, B01100, B00100, B00110, B00010, B00011, B00001, B01001};
}; //anon

Stepper_28BJY48::Stepper_28BJY48(int pin1, int pin2, int pin3, int pin4, int speed)
{
    this->pin1 = pin1;
    this->pin2 = pin2;
    this->pin3 = pin3;
    this->pin4 = pin4;
    this->speed = speed;
    pinMode(pin1, OUTPUT);
    pinMode(pin2, OUTPUT);
    pinMode(pin3, OUTPUT);
    pinMode(pin4, OUTPUT);
    pos = Position(8, 0, 0);
    lastmillis = millis();
}

void Stepper_28BJY48::advance()
{
    int v = goal > pos ? 1 : -1;
    unsigned long time = millis();
    if(!atGoal() && (time - lastmillis >= speed)) step(v);
    lastmillis = time;
}
bool Stepper_28BJY48::atGoal()
{
	return pos == goal;
};

void Stepper_28BJY48::moveTo(Position & goal)
{
    Serial.print("goal:");
    goal.print();
    int v = goal > pos ? 1 : -1;
    while(pos != goal) { step(v); delay(speed); }
}

void Stepper_28BJY48::step(int v)
{
    //assert(v == 1 || v == -1);
    if(v == 1) pos += Position(0, 0, 1);
    else pos -= Position(0, 0, 1);
    if(pos.step == 0 && pos.angle % 8 == 0) {
        pos.print();
    }
    setOutput(pos.step);
}

void Stepper_28BJY48::setOutput(int out)
{
  digitalWrite(pin1, bitRead(lookup[out], 0));
  digitalWrite(pin2, bitRead(lookup[out], 1));
  digitalWrite(pin3, bitRead(lookup[out], 2));
  digitalWrite(pin4, bitRead(lookup[out], 3));
}

void Stepper_28BJY48::Position::print()
{
    Serial.print(turns);
    Serial.print(".");
    Serial.print(angle);
    Serial.print(".");
    Serial.println(step);
}

Stepper_28BJY48::Position& 
Stepper_28BJY48::Position::operator+=(const Stepper_28BJY48::Position& rhs)
{
    //assert(rhs.step >= 0 && rhs.step < 8);
    //assert(rhs.angle >=0 && rhs.angle < countsperrev);
    step += rhs.step;
    if(step >= 8) {
        step -= 8;
        angle ++;
    }
    angle += rhs.angle;
    if(angle >= countsperrev) {
        angle -= countsperrev;
        turns ++;
    }
    turns += rhs.turns;
}

Stepper_28BJY48::Position&
Stepper_28BJY48::Position::operator-=(const Stepper_28BJY48::Position& rhs)
{
    //assert(rhs.step >= 0 && rhs.step < 8);
    //assert(rhs.angle >=0 && rhs.angle < countsperrev);
    step -= rhs.step;
    if(step < 0) {
        step += 8;
        angle --;
    }
    angle -= rhs.angle;
    if(angle < 0) {
        angle += countsperrev;
        turns --;
    }
    turns -= rhs.turns;
}


bool operator<(const Stepper_28BJY48::Position& lhs, const Stepper_28BJY48::Position& rhs)
{
    return lhs.turns < rhs.turns ||
        (lhs.turns == rhs.turns && (lhs.angle < rhs.angle ||
            (lhs.angle == rhs.angle && lhs.step < rhs.step)));
}

bool operator==(const Stepper_28BJY48::Position& lhs, const Stepper_28BJY48::Position& rhs)
{
    return lhs.turns == rhs.turns &&
        lhs.angle == rhs.angle &&
        lhs.step == rhs.step;
}
