#include <Servo.h>

Servo servoLeft;
Servo servoRight;
Servo servoLock;
int pressurePin = 8;
int motionPin = 2;
int servoLeftPin = 9;
int servoRightPin = 11;
int servoLockPin = 6;
int accelerometerPin = 3;

void setup() {
  Serial.begin(9600); 
}

void loop() {
  if (level())
  {
    if (footPressure())
    {
      if (infrared())
      {
        openLid();
        while(footPressure() && level()) {}
        closeLid();
      }
    }
  }
}

  bool footPressure() {
    if(digitalRead(pressurePin)==HIGH){
      return true;
    }
    else {
      return false;
    }
  }
  
  bool infrared() {
    return true; //for testing
    
    //return true if sensor is activated
    /*if(digitalRead(motionPin)==HIGH) {
      return true;
    }
    else {
      return false;
    }*/
  }
  
  void lock() {
    servoLock.attach(servoLockPin);
    servoLock.write(78);
    delay(500);
    servoLock.detach();
  }
  
  void unlock() {
    servoLock.attach(servoLockPin);
    servoLock.write(0);
    delay(500);
    servoLock.detach();
  }
  
  void closeLid() {
    servoRight.attach(servoRightPin);
    servoLeft.attach(servoLeftPin);
    servoRight.write(159);
    servoLeft.write(19);
    delay(500);
    servoRight.detach();
    servoLeft.detach();
    lock();        
  }
  
  void openLid() {
    unlock();
    servoRight.attach(servoRightPin);
    servoLeft.attach(servoLeftPin);
    servoRight.write(84);
    servoLeft.write(163);
    delay(500);
    servoRight.detach();
    servoLeft.detach();          
  }
  
  bool level() {
    //accelerometer currently modeled as a pushbutton
    if(digitalRead(accelerometerPin)==LOW) {
      Serial.println(true);
      return true;
    }
    else {
      Serial.println(false);
      return false;
      //output hazzard to LCD
      //innitiate buzzer
    }

  }

