#ifndef STEPPER_28BJY48_H
#define STEPPER_28BJY48_H

class Stepper_28BJY48 {
public:
    Stepper_28BJY48(int pin1, int pin2, int pin3, int pin4, int speed);
    struct Position {
        Position() : turns(0), angle(0), step(0) {};
        Position(int t, int a, int s) : turns(t), angle(a), step(s) {};
        Position& operator+=(const Position& rhs);
        Position& operator-=(const Position& rhs);
        friend bool operator<(const Position& lhs, const Position& rhs);
        void print();
        int turns;
   	    int angle;
   	    int step;
    };
    void moveTo(Position & pos);
    void setGoal(Position & goal) {this->goal = goal;};
    void advance();
    bool atGoal();
private:
    void step(int v);
    void setOutput(int out);
    int pin1, pin2, pin3, pin4;
    Position pos, goal;
    int speed;
    unsigned long lastmillis;
};


inline bool operator> (const Stepper_28BJY48::Position& lhs,
    const Stepper_28BJY48::Position& rhs) {return rhs < lhs;};
inline bool operator<=(const Stepper_28BJY48::Position& lhs,
    const Stepper_28BJY48::Position& rhs) {return !(lhs > rhs);};
inline bool operator>=(const Stepper_28BJY48::Position& lhs,
    const Stepper_28BJY48::Position& rhs) {return !(lhs < rhs);};
inline bool operator==(const Stepper_28BJY48::Position& lhs,
    const Stepper_28BJY48::Position& rhs);
inline bool operator!=(const Stepper_28BJY48::Position& lhs,
    const Stepper_28BJY48::Position& rhs) {return !(lhs == rhs);};


#endif