#include <Wire.h>
#include <AccelStepper.h>
#include <MultiStepper.h>
#define address 4
#define STEPPER_MODE 1
#define enPin 8
//Goal of this Arduino: CNC Shield V3 - 1 - Left - Undamaged

String readStr = "";//High leveled could change later
const int stepPin_arr[4]= {2,3,4,12};
const int dirPin_arr[4]= {5,6,7,13};
AccelStepper stepper_1(STEPPER_MODE,stepPin_arr[0],dirPin_arr[0]);
AccelStepper stepper_2(STEPPER_MODE,stepPin_arr[1],dirPin_arr[1]);
AccelStepper stepper_3(STEPPER_MODE,stepPin_arr[2],dirPin_arr[2]);
AccelStepper stepper_4(STEPPER_MODE,stepPin_arr[3],dirPin_arr[3]);
AccelStepper* stepper_list[4] = {&stepper_1,&stepper_2,&stepper_3,&stepper_4};

int i;
String command;
float speed;
float accel = 1000;
int serialRate = 9600;

void setup() {
  Serial.begin(serialRate);
  Wire.begin(address);
  Wire.setClock(400000);
  Wire.onReceive(readReq);
  pinMode(enPin,OUTPUT);
  
  for (i = 0; i < sizeof(stepper_list)/sizeof(stepper_list[0]); i++) {
    pinMode(stepPin_arr[i],OUTPUT);
    pinMode(dirPin_arr[i],OUTPUT);
    stepper_list[i]->setAcceleration(accel);
  }
  //Configures with the ampltiude
  digitalWrite(enPin,LOW);//Enable Stepper Drivers
  Serial.println("Initialized the Code");
}
void readReq()
{
  //Reads in the Command
  while(Wire.available())
  {
    readStr += (char)Wire.read();
  }
  moveReq();
}
void moveReq()
{
  //If prevCommand = Current Command -> only change speed
  
  //Commands go up to 3 CHARS + Value
  command = readStr.substring(0,3);
  speed = readStr.substring(4).toDouble();
  readStr="";//Resets command
  if(command.equals("STR"))
  {
    //Indicates goes straight
    //Serial.println("Going Straight");Serial.println(speed);
    straight(speed);
  }
  else if(command.equals("LEF"))
  {
    //Indicates going left
    //Serial.println("Going Left");Serial.println(speed);
    left(speed);
  }
  else if(command.equals("RIG"))
  {
    //Indicates going right
    //Serial.println("Going Right");Serial.println(speed);
    right(speed);
  }
  // else if(command.equals("SET"))
  // {
  //   //Sets the Amplitude up
  //   Serial.println("Setting fin");
  //   // setAmp();
  //   // //Set real speeds now
  //   // int moveAmount = 100000000;
  //   // for(i = 0 ; i < (sizeof(stepper_list)/(sizeof(stepper_list[0])) - 1); i++)
  //   // {
  //   //   stepper_list[i]->setMaxSpeed(1000);
  //   //   stepper_list[i]->setSpeed(0); //Don't Set Speed yet let it sit at 0 so it doesn't move after undulation set
  //   //   stepper_list[i]->moveTo(moveAmount);
  //   // }
  // }
}
void setAmp()
{
  //Goal: Sets the amplitudes of the Fins
  int moveAmount = 50;
  for(i = 0 ; i < (sizeof(stepper_list)/(sizeof(stepper_list[0]))); i++)
  {
    stepper_list[i]->setMaxSpeed(400);
    stepper_list[i]->setSpeed(400);
    stepper_list[i]->moveTo(moveAmount);
    moveAmount-=50;
  }
  bool dist_reached_1 = (stepper_list[0]->distanceToGo()!=0);
  bool dist_reached_2 = (stepper_list[1]->distanceToGo()!=0);
  bool dist_reached_3 = (stepper_list[2]->distanceToGo()!=0);
  bool dist_reached_4 = (stepper_list[3]->distanceToGo()!=0);

  //Will keep polling until the steppers have reached their distance
  while(dist_reached_1&&dist_reached_2&&dist_reached_3&&dist_reached_4)
  {
    if(dist_reached_1)
      stepper_list[0]->run();
    //if()//This is already at the correct position
    if(dist_reached_3)
      stepper_list[2]->run();
    if(dist_reached_4)
      stepper_list[3]->run();
    delayMicroseconds(400);
  }
}
void straight(double speed)
{
  for(i=0;i<sizeof(stepper_list)/sizeof(stepper_list[0]);i++)
  {
    stepper_list[i]->setSpeed(-speed);//Left size rotates the other way
  }
}
void left(double speed)
{
  for(i=0;i<sizeof(stepper_list)/sizeof(stepper_list[0]);i++)
  {
    stepper_list[i]->setSpeed(speed);//Left size rotates the other way
  }
}
void right(double speed)
{
  for(i=0;i<sizeof(stepper_list)/sizeof(stepper_list[0]);i++)
  {
    stepper_list[i]->setSpeed(-speed);//Right size rotates straight
  }
}
void loop() {
  //Constantly run the motors
  for(i=0;i<sizeof(stepper_list)/sizeof(stepper_list[0]);i++)
  {
    stepper_list[i]->run();
  }
  delayMicroseconds(350);
}
