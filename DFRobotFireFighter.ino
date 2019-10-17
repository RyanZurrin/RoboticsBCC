/******************************************
  Arduino DFRobot firefighting Robot code
  Copied and modified by Ryan Zurrin 3/29/19
******************************************/

#define E1 5  //M1 Speed Control
#define E2 6  //M2 Speed Control
#define M1 4  //M1 dir Control
#define M2 7  //M1 dir Control
#define IR1 11  //line tracking sensor 1
#define IR2 12  //line tracking sensor 2
#define IR3 13  //line tracking sensor 3
#define flame_L A0  //flame sensor in left
#define flame_F A1  //flame sensor in front
#define flame_R A2  //flame sensor in right
#define trig1 2  //ultrasonic sensor in left
#define echo1 3
#define trig2 8  //ultrasonic sensor in front
#define echo2 9
#define trig3 0  //ultrasonic sensor in right
#define echo3 1
#define fan 3  //extinguishing fan
/*#define mic A3  //microphone*/

int ldistance, fdistance, rdistance;
int Lspeed = 180; //basic speed for left motor
int Rspeed = 180; //basic speed for right motor
int flag = 0;  //tasks flag: searching room, judging fire, a extinguishing fire, return
int flag2;
int room = 0;  //room number

void setup() {
  pinMode(E1, OUTPUT);
  pinMode(E2, OUTPUT);
  pinMode(M1, OUTPUT);
  pinMode(M2, OUTPUT);
  pinMode(echo1, INPUT);
  pinMode(echo2, INPUT);
  pinMode(echo3, INPUT);
  pinMode(trig1, OUTPUT);
  pinMode(trig2, OUTPUT);
  pinMode(trig3, OUTPUT);
  pinMode(IR1, INPUT);
  pinMode(IR2, INPUT);
  pinMode(IR3, INPUT);
  pinMode(fan, OUTPUT);
}
/****************************************
  Functions for motor moving
*****************************************/
void stopp(float c) {  //Stop
  digitalWrite(E1, LOW);
  digitalWrite(E2, LOW);
  delay(c * 100);  //duration 
}
void advance(float c) {  //forward
  analogWrite(E1, Rspeed + 10);  //PWM speed control
  digitalWrite(M1, HIGH);
  analogWrite(E2, Lspeed + 10);
  digitalWrite(M2, HIGH);
  delay(c * 100);  //duration
}
void back(float c) { //backwards
  analogWrite(E1, Rspeed);  //PWM speed control
  digitalWrite(M1, LOW);
  analogWrite(E2, Lspeed);
  digitalWrite(M2, LOW);
  delay(c * 100);  //duration
}
void left(float c) {  //turn left one motor working
  analogWrite(E1, Rspeed);  //PWM speed control (one motor working)
  digitalWrite(M1, LOW);
  analogWrite(E2, LOW);  
  digitalWrite(M2, HIGH);
  delay(c * 100);  //duration
}
void right(float c) {  //turn right (one motor working)
  analogWrite(E1, LOW);  //PWM speed control (one motor working)
  digitalWrite(M1, HIGH);
  analogWrite(E2, Lspeed);
  digitalWrite(M2,LOW);
  delay(c * 100);  //duration
}
void turnL(float c) {  //turn left (two motors working)
  analogWrite(E1, Rspeed);  //PWM speed control 
  digitalWrite(M1, LOW);
  analogWrite(E2, Lspeed);  
  digitalWrite(M2, HIGH);
  delay(c * 100);  //duration
}
void turnR(float c) {  //turn right (two motors working)
  analogWrite(E1, Rspeed);  //PWM speed control 
  digitalWrite(M1, HIGH);
  analogWrite(E2, Lspeed);  
  digitalWrite(M2, LOW);
  delay(c * 100);  //duration
}
/****************************************
  Functions for Ultra Sonic Sensor
*****************************************/
float UltraFront() {
  float m;
  digitalWrite(trig1, LOW);  //Clears the trigger Pin
  delayMicroseconds(2);
//sets trigger pin to high state for 10microseconds
  digitalWrite(trig1, HIGH);
  delayMicroseconds(10);
  digitalWrite(trig1, LOW);
  m = pulseIn(echo1, HIGH);
  m = m * 0.034 / 2; //Calculates the distance
  return m; // returns the distance
}
float UltraLeft() {
  float m;
  digitalWrite(trig2, LOW);  //Clears the trigger Pin
  delayMicroseconds(2);
//sets trigger pin to high state for 10microseconds
  digitalWrite(trig2, HIGH);
  delayMicroseconds(10);
  digitalWrite(trig2, LOW);
  m = pulseIn(echo2, HIGH);
  m = m * 0.034 / 2; //Calculates the distance
  return m; // returns the distance
}
float UltraRight() {
  float m;
  digitalWrite(trig3, LOW);  //Clears the trigger Pin
  delayMicroseconds(2);
//sets trigger pin to high state for 10microseconds
  digitalWrite(trig3, HIGH);
  delayMicroseconds(10);
  digitalWrite(trig3, LOW);
  m = pulseIn(echo3, HIGH);
  m = m * 0.034 / 2; //Calculates the distance
  return m; // returns the distance
}
/****************************************
  Function for extinguishing fire
*****************************************/
void fans() {
  int m;
  m = analogRead(flame_L);  //if the fire on the left side
  if (m < 800) {
    turnR(0.6);  //adjusts the motor to right
    stopp(1);
    m = analogRead(flame_L);
  } while (m > 800);
  stopp(20);

   m = analogRead(flame_R);  //if the fire on the right side
  if (m < 800) {
    turnL(0.6);  //adjusts the motor to left
    stopp(1);
    m = analogRead(flame_R);
  } while (m > 800);
  stopp(20);  

  if (analogRead(flame_F) < 800){         // if the fire in the front
    analogWrite(fan,250);
    delay(4000);
    analogWrite(fan,0);
 }
  stopp(10);
  
  flag2 = 1;      // The fire is in this room
  stopp(10);
}
/**********************************************
 Function for left-hand law (searching room)
***********************************************/
void Left_rule1() {
  do{ 
    fdistance = UltraFront();  
    ldistance = UltraLeft();
    if(fdistance<21)
      turnR(0.3);
     else if(ldistance<10)
      right(0);
     else if(ldistance<13)
      advance (0);
     else
      left(0);
   }
   while(digitalRead(IR1)==LOW&&digitalRead(IR2)==LOW&&digitalRead(IR3)==LOW);
   // searching the room and enter in
   if(digitalRead(IR1)==HIGH||digitalRead(IR2)==HIGH||digitalRead(IR3)==HIGH){
    back(1);
    stopp(20);
    room++;    // record the room
    advance(1.5);   //enter the room
   }
   flag = 1;
}
/**********************************************
 Function for left-hand law (searching fire)
***********************************************/
void Left_rule2() {
  do
  { fdistance = UltraFront();
    ldistance=UltraLeft();
    if(fdistance<21)
      turnR(0.3);
     else if(ldistance<10)
      right(0);
     else if(ldistance<13)
      advance(0);
      else
        left(0);
   }
   while(digitalRead(IR1)==LOW&&digitalRead(IR2)==LOW&&digitalRead(IR3)==LOW);
   // searching the fire
   if(digitalRead(IR1)==HIGH||digitalRead(IR2)==HIGH||digitalRead(IR3)==HIGH){
     back(1);
      stopp(40);
      if(analogRead(flame_L)<800||analogRead(flame_F)<800||analogRead(flame_R)<800){
       fans();// it is fire
        advance(1.0);
       }
       else
        advance(2.1);// no fire
     }
     flag = 2;
}
/*******************************************
 Function for left hand law (leaving room)
 *******************************************/
void Left_rule3(){
  do{
    fdistance = UltraFront();
    ldistance = UltraLeft();
    if (fdistance < 21)
      turnR(0.3);
    else if (ldistance < 10)
      right(0);
    else if (ldistance < 13)
      advance(0);
    else
      left(0);   
  }
    while (digitalRead(IR1) == LOW && digitalRead(IR2) == LOW && digitalRead(IR3) == LOW);
  if (digitalRead(IR1) == HIGH || digitalRead(IR2) == HIGH || digitalRead(IR3) == HIGH){
    stopp(10);   //preparing to leave
    advance(1.0);
    if (flag2 == 0) // if no fire, continue to search
      flag = 0;
    if (flag2 == 1)  // it is fire, go back home
      flag = 3;
  }
}
/***********************************************************
 Function for back (in room 3 or 4)
************************************************************/
void Left_rule_Back(){
  if (room == 3){
    do{
      fdistance = UltraFront();
      ldistance =  UltraLeft();
      if (fdistance < 19)
        turnR(0.3);
      else if (ldistance < 10)
        right(0);
      else if (ldistance < 12)
        advance(0);
      else
        left(0);       
    }
    while (digitalRead(IR1)== LOW && digitalRead(IR2) ==LOW && digitalRead(IR3) == LOW);
    
    stopp(20);
    back(8);
    turnR(6);
    do {
      advance(0);
    }
    while (digitalRead(IR1) == LOW && digitalRead(IR2) == LOW && digitalRead(IR3) == LOW);

    back(1);
    stopp(20);
    delay(10);
    stopp(50);
  }

  if (room == 4){
    advance(8);
    turnL(7);
    do {
      advance(0);
    }
    while (digitalRead(IR1) == LOW && digitalRead(IR2) == LOW && digitalRead(IR3) == LOW);
    do {
      advance(0);
    }
    while (digitalRead(IR1) == LOW && digitalRead(IR2) == LOW && digitalRead(IR3) == LOW);
    back(1);
    stopp(20);
    delay(10);
    stopp(50);
  }
}
/******************************************************************
 Function for back (in room 1 or room 2)
******************************************************************/
void Right_rule_Back(){
  if (room == 1){
    advance(6);
    turnR(5);
    do {
      advance(0);
    }
    while (digitalRead(IR1) == LOW && digitalRead(IR2) == LOW && digitalRead(IR3) == LOW);
    back(1);
    stopp(20);
    delay(10);
    stopp(50);
  }
  if (room == 2){
    advance(1);
    do{
      fdistance = UltraFront();
      rdistance = UltraRight();
      if (fdistance < 19)
        turnL(0.3);
      else if (rdistance < 10)
        left(0);
      else if (rdistance < 13)
        advance(0);
      else
        right(0);
    }
    while (digitalRead(IR1) == LOW && digitalRead(IR2) == LOW && digitalRead(IR3) == LOW);
    back(1);
    stopp(20);
    delay(10);
    stopp(50);
  }
}
/***************************************************************************
 Main Function
***************************************************************************/
void loop(){
  switch (flag){
    case 0:
      Left_rule1();
      break;
    case 1:
      Left_rule2();
      break;
    case 2:
      Left_rule3();
      break;
    case 3:
      if (room == 1 || room == 2)
        Right_rule_Back();
      else if (room == 3 || room == 4)
        Left_rule_Back();
      break;
  }
}
  
