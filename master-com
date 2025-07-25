#include <AccelStepper.h>
#include <MultiStepper.h>
#include <Wire.h>
#include <Adafruit_MotorShield.h>
#include <Adafruit_PWMServoDriver.h>
#define MAX_VAL 2147483647
#define STEPPER_MODE 1
#define RC_CH1_INPUT 2
#define RC_CH2_INPUT 3

int i;
String command = "";

const int channel_val[2][4] = {
  { 1844, 1282, 1562, 10 },  //ch1 Up/Down Right 1
  { 1902, 1306, 1603, 10 },  //ch2 Left/Right Right 1
  // { 1972, 1202, 1584, 10 },   //ch3 Up/Down Left 1
  // { 1987, 1206, 1591, 35 },  //ch4 Left/Right Left 1
  // { 1859, 1198, 1464, 35 },  //ch5 Up/Down Left 2
  // { 1725, 1132, 1420, 46 },  //ch6 Left/Right Right 2
  // { 1898, 1318, 1602, 10 },  //ch7 Up/Down 2
  // { 1868, 1281, 1577, 10 }   //ch8 Left/Right Left 2
};
const int address_list[2] = {4,5};//RC_CH3_INPUT,RC_CH4_INPUT,RC_CH8_INPUT
const int receiver_pin_list[2] = {RC_CH1_INPUT,RC_CH2_INPUT};//,RC_CH5_INPUT,RC_CH6_INPUT,RC_CH7_INPUT};
AccelStepper stepper_cr(STEPPER_MODE,10,11);//Acts as stepper4
int control_val[5] = {0,0,0,0,0};
int serialRate = 31250;
void setup() {
  //Init's this as "Controller"/MASTER arduino to control 2 Shields
  Serial.begin(serialRate);
  delay(100);
  Wire.begin();
  Wire.setClock(400000);
  //Activates the Receiver pins as reading
  for (i = 0; i < sizeof(receiver_pin_list) / sizeof(receiver_pin_list[0]); i++) {
    pinMode(receiver_pin_list[i], INPUT);
  }
  //Setting separate driver up 
  pinMode(10,OUTPUT);
  pinMode(11,OUTPUT);
  //setAmp();
  //Sets the real speeds
  // stepper_cr.setAcceleration(1000);
  // stepper_cr.setMaxSpeed(1000);
  // stepper_cr.moveTo(100000000);
  Serial.println("Initialized the Stingray");
  
  //Stepper Driver is grounded already by wires -> No need to ground
}
void setAmp()
{
  //Posibility to use this as one of the right sticks to demonstrate ampltiude sets
  stepper_cr.setMaxSpeed(400);
  stepper_cr.setSpeed(400);
  stepper_cr.moveTo(-100);
  for(i=0;i<sizeof(address_list)/sizeof(address_list[0]);i++)
  {
    Wire.beginTransmission(address_list[i]);
    Wire.write("SET");
    Wire.endTransmission();
  }
  while(stepper_cr.distanceToGo()!=0)
  {
    stepper_cr.run();
    delayMicroseconds(400);
  }
  stepper_cr.setSpeed(0);//Resets the speed
}
int convertValues(int controllerValue, int CHANNEL_NUM) {
  //goal: Probably, to convert all controller values into actual readable values for the speed from 0 - 100
  int MULT_VAL = 1000;
  //1750
  int HIGH_VAL = channel_val[CHANNEL_NUM][0];
  int LOW_VAL = channel_val[CHANNEL_NUM][1];
  int MID_VAL = channel_val[CHANNEL_NUM][2];
  int BIAS = channel_val[CHANNEL_NUM][3];  //Added BIAS because of Controller Deadzone
  int output_val = 0.0;
  //Or we can say if max_val - bias < controllerValue -> setToMaxVal
  if (MID_VAL - BIAS <= controllerValue && MID_VAL + BIAS >= controllerValue) 
  {
    output_val = 0.0;
  } 
  else if (controllerValue > MID_VAL) 
  {
    output_val = (int)(controllerValue - MID_VAL) / (HIGH_VAL - MID_VAL);
  }
  else if (controllerValue < MID_VAL) {
    output_val = (int)(controllerValue - MID_VAL)/(MID_VAL - LOW_VAL);
  }
  return (int)output_val * MULT_VAL;
}
void readControllerValues() {
  //Serial Plotter should be used for this to measure and show values

  //Channels keep varying
  //---- Controller 1 ----
  //Channel 1 = Right Stick -> Up/Down
  //Channel 2 = Right Stick -> Left/Right
  //Channel 3 = Left Stick -> Up/Down
  //Channel 4 = Left Stick -> Left Right
  //---- Controller 2 ----
  //Channel 5 = Left Stick -> Up/Down
  //Channel 6 = Right Stick -> Left/Right
  //Channel 7 = Right Stick -> Up/Down
  //Channel 8 = Left Stick -> Left/Right
  
  int pulse_list[2] = {
    pulseIn(RC_CH1_INPUT, HIGH),pulseIn(RC_CH2_INPUT, HIGH)
  };
  for(i=0;i<sizeof(pulse_list)/sizeof(pulse_list[0]);i++)
  {
    //control_val[i] = pulse_list[i];
    control_val[i] = convertValues(pulse_list[i],i);
  }
  // Serial.print("Channel_1:");Serial.print(control_val[0]);Serial.print(", ");
  // Serial.print("Channel_2:");Serial.print(control_val[1]);Serial.println();
}

void moveControllerValues() {
  int dead_zone = 250.0;//25.0% deadzone changeable
  String convSpeed="";
  String w_command="";
  if (control_val[0] > dead_zone || control_val[0] < -dead_zone) {
    convSpeed = String((int)control_val[0],3);
    command = "S";
  } else if (control_val[1] > dead_zone || control_val[1] < -dead_zone) {
    convSpeed = String((int)control_val[1],3);
    if(control_val[1] > 0)
    {
      command = "L";
    }
    if(control_val[1] < 0)
    {
      command = "R";
    }
    //EXTRA: TILTING???
  }
  //Sees if there was any call to change
  if(!convSpeed.equals(""))
  {
    //Sends the proper command
    w_command = command + convSpeed;
    Serial.println(w_command);
    //This thing here takes forever specifically I2C transmission betweeen two arduinos
    for(i=0;i<sizeof(address_list)/sizeof(address_list[0]);i++)
    {
      Wire.beginTransmission(address_list[i]);
      Wire.write(w_command.c_str());
      int result = Wire.endTransmission();
      if(result!=0)
      {
        Serial.print("I2C Failed with error: ");
        Serial.println(result);
      }
    }
  }
}
void loop() {
  readControllerValues();
  moveControllerValues();
  //stepper_cr.run();//Theres enough delay from reading + moving
  //delayMicroseconds(350);
}
