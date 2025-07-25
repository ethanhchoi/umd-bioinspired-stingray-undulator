#include <Wire.h>
#include <AccelStepper.h>
#include <MultiStepper.h>
#define address 4
#define STEPPER_MODE 1
#define enPin 8
//Goal of this Arduino: CNC Shield V3 - 1 - Left - Undamaged

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

struct CommandPacket{
  char cmd;
  short int speed;
};
CommandPacket r_packet;

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
    stepper_list[i]->moveTo(1000000);
    stepper_list[i]->setMaxSpeed(1000);
    stepper_list[i]->setSpeed(1000);
  }

  digitalWrite(enPin,LOW);//Enable Stepper Drivers
  Serial.println("Initialized the Code");
}
void readReq(int numBytes)
{
  //Reads in the Command
  if(numBytes == sizeof(r_packet))
  {
    Wire.readBytes((char*)&r_packet,sizeof(r_packet));
  }
}
void moveReq()
{
  if(r_packet.cmd=="S")
  {
    //Indicates goes straight
    Serial.print("Straight:");
    Serial.println(speed);
    //straight(speed);
  }
  else if(r_packet.cmd=="T")
  {
    //Indicates going left
    Serial.print("Turn:");Serial.println(speed);
    //turn(speed);
  }
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
void turn(float speed)
{
  for(i=0;i<sizeof(stepper_list)/sizeof(stepper_list[0]);i++)
  {
    stepper_list[i]->setSpeed(speed);//Left size rotates the other way
  }
}
void loop() {
  //Constantly run the motors
  moveReq();
  for(i=0;i<sizeof(stepper_list)/sizeof(stepper_list[0]);i++)
  {
    stepper_list[i]->run();
  }
  delayMicroseconds(350);
}
