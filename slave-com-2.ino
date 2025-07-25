#include <Wire.h>
#include <AccelStepper.h>
#include <MultiStepper.h>
#define address 5
#define STEPPER_MODE 1
#define enPin 8
//Goal of this Arduino: CNC Shield V3 - 2 - Right - Damaged

String readStr = "";//High leveled could change later
const int stepPin_arr[3]= {2,3,4};
const int dirPin_arr[3]= {5,6,7};
AccelStepper stepper_1(STEPPER_MODE,stepPin_arr[0],dirPin_arr[0]);
AccelStepper stepper_2(STEPPER_MODE,stepPin_arr[1],dirPin_arr[1]);
AccelStepper stepper_3(STEPPER_MODE,stepPin_arr[2],dirPin_arr[2]);
AccelStepper* stepper_list[3] = {&stepper_1,&stepper_2,&stepper_3};

int i;
String command;
float speed;
float accel = 1000;
int serialRate = 31250;

void setup() {
  Serial.begin(serialRate);
  Wire.begin(address);
  Wire.setClock(400000);
  Wire.onReceive(readReq);
  pinMode(enPin,OUTPUT);
  //Configures with the ampltiude
  //setAmp();
  for (i = 0; i < sizeof(stepper_list)/sizeof(stepper_list[0]); i++) {
    pinMode(stepPin_arr[i],OUTPUT);
    pinMode(dirPin_arr[i],OUTPUT);
    stepper_list[i]->setAcceleration(accel);
    stepper_list[i]->moveTo(1000000);
    stepper_list[i]->setMaxSpeed(2000);
    stepper_list[i]->setSpeed(1000);
  }
  
  digitalWrite(enPin,LOW);//Enable Stepper Drivers
  Serial.println("Initialized the Code");
}
void setAmp()
{
  int moveAmount = 50;
  for(i = 0 ; i < (sizeof(stepper_list)/(sizeof(stepper_list[0]))); i++)
  {
    stepper_list[i]->setMaxSpeed(400);
    stepper_list[i]->setSpeed(400);
    stepper_list[i]->moveTo(moveAmount);
    moveAmount-=50;
  }
  //
  bool dist_reached_1 = (stepper_list[0]->distanceToGo()!=0);
  bool dist_reached_3 = (stepper_list[2]->distanceToGo()!=0);

  //Will keep polling until the steppers have reached their distance
  while(dist_reached_3&&dist_reached_1)
  {
    if(dist_reached_3)
      stepper_list[2]->run();
    if(dist_reached_1)
      stepper_list[0]->run();
    delayMicroseconds(400);
  }
}
void readReq(int numBytes)
{
  //Reads in the Command
  while(Wire.available())
  {
    readStr += (char)Wire.read();
  }
  Serial.println(readStr);
  moveReq();
  //If prevCommand = Current Command -> only change speed
  //Commands go up to 3 CHARS + Value
}
void moveReq()
{
  command = readStr.substring(0,1);
  speed = readStr.substring(1).toInt();
  readStr="";//Resets command
  Serial.println(command);
  Serial.println(speed);
  if(command=="S")
  {
    //Indicates goes straight
    Serial.println("Straight");
    Serial.println(speed);
    
    //straight(speed);
  }
  else if(command=="L")
  {
    //Indicates going left
    Serial.println("Left");Serial.println(speed);
    //left(speed);
  }
  else if(command=="R")
  {
    //Indicates going right
    Serial.println("Right");Serial.println(speed);
    //right(speed);
  }
  else if(command=="E")
  {
    //Sets the Amplitude up
    Serial.println("Setting Fin");
    // setAmp();
    // //Set real speeds now
    // int moveAmount = 100000000;
    // for(i = 0 ; i < (sizeof(stepper_list)/(sizeof(stepper_list[0])) - 1); i++)
    // {
    //   stepper_list[i]->setMaxSpeed(1000);
    //   stepper_list[i]->setSpeed(0); //Don't Set Speed yet let it sit at 0 so it doesn't move after undulation set
    //   stepper_list[i]->moveTo(moveAmount);
    // }
  }
}
void straight(float speed)
{
  for(i=0;i<sizeof(stepper_list)/sizeof(stepper_list[0]);i++)
  {
    stepper_list[i]->setSpeed(speed);//Left size rotates the other way
  }
}
void left(float speed)
{
  for(i=0;i<sizeof(stepper_list)/sizeof(stepper_list[0]);i++)
  {
    stepper_list[i]->setSpeed(-speed);//Left size rotates the other way
  }
}
void right(float speed)
{
  for(i=0;i<sizeof(stepper_list)/sizeof(stepper_list[0]);i++)
  {
    stepper_list[i]->setSpeed(speed);//Right size rotates straight
  }
}
void loop() {
  //Constantly run the motors
  //Serial.println(stepper_list[i]->speed());
  for(i=0;i<sizeof(stepper_list)/sizeof(stepper_list[0]);i++)
  {
    stepper_list[i]->run();
  }
  delayMicroseconds(400);
}
