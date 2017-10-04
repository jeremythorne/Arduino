// a simple car with two stepper motors

//declare variables for the motor pins
struct motor {
  int pin1;
  int pin2;
  int pin3;
  int pin4;
  int revs;
  int angle;
  int step;
};

struct motor left;
struct motor right;

int motorSpeed = 3;  //variable to set stepper speed
int count = 0;          // count of steps made
int countsperrev = 512; // number of steps per full revolution
int lookup[8] = {B01000, B01100, B00100, B00110, B00010, B00011, B00001, B01001};

void setupMotor(struct motor *m)
{
  pinMode(m->pin1, OUTPUT);
  pinMode(m->pin2, OUTPUT);
  pinMode(m->pin3, OUTPUT);
  pinMode(m->pin4, OUTPUT);
  m->revs = 0;
  m->angle = 0;
  m->step = 0;
}  

//////////////////////////////////////////////////////////////////////////////
void setup() {
  right.pin1 = 8;    // Blue   - 28BYJ48 pin 1
  right.pin2 = 9;    // Pink   - 28BYJ48 pin 2
  right.pin3 = 10;    // Yellow - 28BYJ48 pin 3
  right.pin4 = 11;    // Orange - 28BYJ48 pin 4
  
  left.pin1 = 4;
  left.pin2 = 5;
  left.pin3 = 6;
  left.pin4 = 7;
  setupMotor(&left);
  setupMotor(&right);
  //declare the motor pins as outputs
  Serial.begin(9600);
}

//////////////////////////////////////////////////////////////////////////////

void jos_instructions()
{
turnright(4);
forward(1);
backward(1);
}

void daddys_instructions()
{
 forward(1);
  backward(1);
  forward(1);
  turnright(1);
}

void loop(){
  jos_instructions();
 
}

//////////////////////////////////////////////////////////////////////////////
//set pins to ULN2003 high in sequence from 1 to 4
//delay "motorSpeed" between each pin setting (to determine speed)

struct vel {
  int left;
  int right;
};

void forward(int turns)
{
  struct vel v = {1, -1};
  doturns(&v, turns);
}

void backward(int turns)
{
  struct vel v = {-1, 1};
  doturns(&v, turns);
}

void turnright(int turns)
{
  struct vel v = {-1, -1};
  doturns(&v, turns);
}

void turnleft(int turns)
{
  struct vel v = {1, 1};
  doturns(&v, turns);
}

void doturns(struct vel *v, int turns)
{
  int goal = right.revs + v->right * turns;
  Serial.println(goal);
  while(!(right.revs == goal && right.angle == 0))
  {
    advance(v);
    //Serial.print(right.revs);
    //Serial.print(" ");
    //Serial.print(right.angle);
    //Serial.print(" ");
    //Serial.println(right.step);
  }
}

void advance(struct vel *v)
{
    step(&left, v->left);
    step(&right, v->right);
    delay(motorSpeed);
}

void step(struct motor *m, int v)
{ 
  if(v > 0)
  { 
    m->step += 1;
    if(m->step == 8)
    {
      m->step = 0;
      m->angle += 1;
      if(m->angle == countsperrev)
      {
        m->angle = 0;
        m->revs += 1;
      }
    }
  }
  else
  {
    m->step -= 1;
    if(m->step < 0)
    {
      m->step = 7;
      m->angle -= 1;
      if(m->angle == -1)
      {
        m->angle = countsperrev - 1;
        m->revs -= 1;
      }
    }
  }
  
  setOutput(m, m->step);
}

void setOutput(struct motor * m, int out)
{
  digitalWrite(m->pin1, bitRead(lookup[out], 0));
  digitalWrite(m->pin2, bitRead(lookup[out], 1));
  digitalWrite(m->pin3, bitRead(lookup[out], 2));
  digitalWrite(m->pin4, bitRead(lookup[out], 3));
}