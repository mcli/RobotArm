/*
 * Robot Arm Controller program
 */

/*
 * D3, D5, D6, D9, D10 and D11 are capable of PWM outputs which we can
 * 5, 6, 7, and 8 are are used for the motor driver IC
 * 
 * Let's use 9, 10, and 11 to control the two VEX and HS-422 motors
 */
#include <WiiChuckI2C.h>
#include <I2C.h>
#include <microM.h>
#include <Servo.h>

Servo servo9, servo10, servo11;

void setup()
{
  Serial.begin(115200);  // Initialize serial port for debugging

  microM.Setup();
  servo9.attach(9);
  servo10.attach(10);
  servo11.attach(11);
  
  int status=wiiChuckI2C.init(); // initialize nunchuck
  if (0 != status)
  {
    Serial.println("Error initializing wiiChuckI2C");
  }

  wiiChuckI2C.calibrateJoyCenter();
}

void loop()
{
  static int count=0;
  static int lastStatus=-1;

  WiiChuckButton button;
  
  int accel[3]={0};
  int joy[2]={0};

  // read the nunchuck data
  int readStatus=wiiChuckI2C.read(joy, accel, button);

  if (0==readStatus)            // print the data if retrieval was successful
  {
    static int motorSpeed[2]={0};
    static int motorBrake[2]={0};
    const int multiplier=10;
    // the left motor controller
    for (int i = 0; i < 2; i++)
    {
      if (-20 < joy[i] && 20 >joy[i])            // turn on the brakes
      {
        motorSpeed[i]=0;
        motorBrake[i]=1;
      } else
      {
        motorBrake[i]=0;
        motorSpeed[i]=multiplier*joy[i];
      }
    }
    Serial.print("Joy:  ");
    for (int i = 0; i < 2; i++)
    {
      Serial.print(joy[i], DEC);
      Serial.print(", ");
    }
    Serial.print("Accel:  ");
    for (int i = 0; i< 3; i++)
    {
      if (i) Serial.print(", ");
      Serial.print(accel[i], DEC);
    }
    // if the Z button is pressed, control the pin 9 motor by tilting
    // the Y axis
    int val9=0;
    int val10=0;
    switch(button)
    {
    case BUTTON_Z:
      Serial.print(", [Z Button]");
      val9=map(accel[1], 0, 1023, 0, 180);
      val10=0;
      break;
    case BUTTON_C:
      val9=0;
      Serial.print(", [C Button]");
      val10=map(accel[1], 0, 1023, 0, 180);
      break;
    default:
      val9=0;
      val10=0;
    }
    Serial.println();
    servo9.write(val9);
    servo10.write(val10);
    int val11=joy[0]+114;
    servo11.write(val11);

    microM.Motors(motorSpeed[0],motorSpeed[1],motorBrake[0],motorBrake[1]);

    lastStatus=0;
  } else
  {
    if (lastStatus != readStatus)
    {
      Serial.print("Error Reading:  Status ");
      Serial.println(readStatus);
      lastStatus=readStatus;
    }
  }
}

