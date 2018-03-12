#include <SoftwareSerial.h>

// Attach the serial display's RX line to digital pin 2
SoftwareSerial LCDSerial(3,2); // pin 2 = TX, pin 3 = RX (unused)
char salinitystring[20];
char salinitySetpointstring[20];
char salinityLCLstring[20];
char salinityUCLstring[20];
char tempstring[20];
char tempSetpointstring[20];
char tempLCLstring[20];
char tempCyclestring[20];
int salinity_power_pin = 7;
int thermistor_power_pin = 6;
int heater_power_pin = 8;
int solDI_pin = 10;
int solSalt_pin = 9;
int salinity_input_pin = A4;
int thermistor_input_pin = A5;
int setpointSalinityPin = A0;
int setpointTempPin = A1;
int setpointSalinityReading;
int setpointTempReading;
float sigmaSalinity = 5.798;
float sigmaTemp = 0.40;
float setpointSalinity;
float setpointTemp;
float LCLSalinity;
float UCLSalinity;
float LCLTemp;
float salinityReading;
float tempReading;
float F = 0.15;
float GDI = 0.80;
float GSalt = 0.60;
float massInTank = 113.0;
float salt_flowrate = 6.80;
float DI_flowrate = 6.74;
float deadtimeSalinity = 6000.0;
float K = .00001042;
unsigned long last_salinity_update;
float DIOpenTime;
float saltOpenTime; 
float heaterOnTime;
boolean DIOpen = false;
boolean SaltOpen = false;
boolean HeaterOn = false;
float DIOpenTimer;
float SaltOpenTimer;
float HeatOnTimer;
float HeaterCurrCycleSetpoint;

void setup() {
  Serial.begin(9600);
  LCDSerial.begin(9600);
  delay(500); 
  pinMode(solDI_pin, OUTPUT);
  pinMode(solSalt_pin, OUTPUT);
  pinMode(salinity_power_pin, OUTPUT);
  pinMode(thermistor_power_pin, OUTPUT);
  pinMode(heater_power_pin, OUTPUT);
  last_salinity_update = millis();
}

void loop() {
  setDeadband();
  salinityReading = salinity_value(sensor_reading( salinity_power_pin, salinity_input_pin ));
  tempReading = temperature_value(sensor_reading( thermistor_power_pin, thermistor_input_pin ));
  LCDUpdate();
  if ( !DIOpen && !SaltOpen && millis()-last_salinity_update >= deadtimeSalinity ) {
    if ( salinityReading>UCLSalinity ) {
      DIOpenTime = ((massInTank*((GDI*(setpointSalinity-salinityReading))/((1-F)*(0-salinityReading))))/DI_flowrate)*1000;
      //Serial.print(millis()); Serial.print(" "); Serial.println(DIOpenTime);
      digitalWrite(solDI_pin, HIGH);
      DIOpenTimer = millis();
      DIOpen = true;
    }
    else if ( salinityReading<LCLSalinity  ) {
      saltOpenTime = ((massInTank*((GSalt*(setpointSalinity-salinityReading))/((1-F)*(1-salinityReading))))/salt_flowrate)*1000;
      digitalWrite(solSalt_pin, HIGH);
      SaltOpenTimer = millis();
      SaltOpen = true;      
    } 
  }
  else if ( DIOpen && millis()-DIOpenTimer >= DIOpenTime ) {
    digitalWrite(solDI_pin, LOW);
    DIOpen = false;
    last_salinity_update = millis();
  } 
       else if ( SaltOpen && millis()-SaltOpenTimer >= saltOpenTime ) {
         digitalWrite(solSalt_pin, LOW);
         SaltOpen = false;
         last_salinity_update = millis();         
       }
  if ( !HeaterOn && tempReading<LCLTemp ) {
      HeaterCurrCycleSetpoint = setpointTemp;
      heaterOnTime = (setpointTemp-tempReading)/K;
      Serial.print(setpointTemp); Serial.print(" "); Serial.print(tempReading); Serial.print(" "); Serial.println(heaterOnTime);   
      digitalWrite(heater_power_pin, HIGH);
      HeatOnTimer = millis();
      HeaterOn = true;      
    } 
  else if ( (HeaterOn && millis()-HeatOnTimer >= heaterOnTime) || (HeaterOn && abs(HeaterCurrCycleSetpoint-setpointTemp) >= 0.3) || (HeaterOn && tempReading >= setpointTemp) ) {
    digitalWrite(heater_power_pin, LOW);
    HeaterOn = false;
  }
}

int sensor_reading( int power_pin, int input_pin ) {
  int reading;
  digitalWrite (power_pin, HIGH ); // Turn on the sensor
  delay(100); // Wait to settle
  reading = analogRead (input_pin ); // Read voltage
  digitalWrite (power_pin, LOW ); // Turn off the sensor
  delay(100); // Wait to settle
  //Serial.print(reading); Serial.print(" ");
  return reading;
}

float salinity_value(int reading) {
 //Serial.println(reading);
 int rbreak = 506; // separation between linear segments
 int rlimit = 1023; // upper limit of acceptable readings
 float salinity; // Compute salinity from calibration equations
 if ( reading<0 ) {
 // print error message, reading can’t be negative
 } else if ( reading<rbreak ) {
 salinity = .0000946990857*reading;
 } else if ( reading<=rlimit ) {
 salinity = (.00000073873151*pow(reading,2))-(.000131977919*reading)-.0723610358;
 } else {
 // do something to be safe
 }
 //Serial.println(salinity);
 return(salinity);
}

float temperature_value(int reading) {
  //Serial.println(reading);
  float temperature = (.000087188130*pow(reading,2))+(.027187059*reading)-12.705983;
  return(temperature);
}

void setDeadband() {
  setpointSalinityReading = analogRead(setpointSalinityPin);
  setpointSalinityReading = constrain(setpointSalinityReading, 0, 1023);
  setpointSalinityReading = map(setpointSalinityReading, 0, 1023, 200, 800);
  setpointSalinity = salinity_value(setpointSalinityReading);
  LCLSalinity = salinity_value(setpointSalinityReading-(sigmaSalinity*3));
  UCLSalinity = salinity_value(setpointSalinityReading+(sigmaSalinity*3));
  setpointTempReading = analogRead(setpointTempPin);
  setpointTempReading = constrain(setpointTempReading, 0, 1023);
  setpointTempReading = map(setpointTempReading, 0, 1023, 317, 1023);
  setpointTemp = temperature_value(setpointTempReading);
  LCLTemp = temperature_value(setpointTempReading-(sigmaTemp*3));
}

void LCDUpdate() {
  clearScreen();
  LCDSerial.write(254);
  LCDSerial.write(128);
  LCDSerial.write("Salt% DI Temp Heat");
  LCDSerial.write(254);
  LCDSerial.write(148);
  LCDSerial.write("Set   1% Set"); 
  dtostrf(salinityReading,4,2,salinitystring);
  dtostrf(setpointSalinity,4,2,salinitySetpointstring);  
  LCDSerial.write(254);
  LCDSerial.write(192);
  LCDSerial.write(salinitystring);
  LCDSerial.write(254);
  LCDSerial.write(212);
  LCDSerial.write(salinitySetpointstring);
  LCDSerial.write(254);
  LCDSerial.write(217);
  if (SaltOpen) {
    LCDSerial.write("ON");
  }
  else {
    LCDSerial.write("OFF");
  }
  LCDSerial.write(254);
  LCDSerial.write(197);
  if (DIOpen) {
    LCDSerial.write("ON");
  }
  else {
    LCDSerial.write("OFF");
  }
  dtostrf(tempReading,4,1,tempstring);
  dtostrf(setpointTemp,4,1,tempSetpointstring);
  dtostrf((millis()-HeatOnTimer-heaterOnTime)/-1000,3,0,tempCyclestring);  
  LCDSerial.write(254);
  LCDSerial.write(201);
  LCDSerial.write(tempstring);
  LCDSerial.write(254);
  LCDSerial.write(221);
  LCDSerial.write(tempSetpointstring);
  LCDSerial.write(254);
  LCDSerial.write(206);
  if (HeaterOn) {
    LCDSerial.write("ON");
  }
  else {
    LCDSerial.write("OFF");
  } 
  /*LCDSerial.write(254);
  LCDSerial.write(128);
  LCDSerial.write("  LCL  SetPt");
  LCDSerial.write(254);
  LCDSerial.write(148);
  LCDSerial.write("Heater  Temp"); 
  dtostrf(tempReading,5,2,tempstring);
  dtostrf(setpointTemp,5,2,tempSetpointstring);
  dtostrf(LCLTemp,5,2,tempLCLstring);    
  LCDSerial.write(254);
  LCDSerial.write(220);
  LCDSerial.write(tempstring);
  LCDSerial.write(254);
  LCDSerial.write(199);
  LCDSerial.write(tempSetpointstring);
  LCDSerial.write(254);
  LCDSerial.write(192);
  LCDSerial.write(tempLCLstring);
  LCDSerial.write(254);
  LCDSerial.write(213);
  if (HeaterOn) {
    LCDSerial.write("ON");
  }
  else {
    LCDSerial.write("OFF");
  }*/
  /*LCDSerial.write(254);
  LCDSerial.write(128);
  LCDSerial.write("  LCL  SetPt    UCL");
  LCDSerial.write(254);
  LCDSerial.write(148);
  LCDSerial.write("Salty  Salinity   DI"); 
  dtostrf(salinityReading,6,4,salinitystring);
  dtostrf(setpointSalinity,6,4,salinitySetpointstring);
  dtostrf(LCLSalinity,6,4,salinityLCLstring);
  dtostrf(UCLSalinity,6,4,salinityUCLstring);
  //Serial.println(salinityReading,6);      
  LCDSerial.write(254);
  LCDSerial.write(220);
  LCDSerial.write(salinitystring);
  LCDSerial.write(254);
  LCDSerial.write(199);
  LCDSerial.write(salinitySetpointstring);
  LCDSerial.write(254);
  LCDSerial.write(192);
  LCDSerial.write(salinityLCLstring);
  LCDSerial.write(254);
  LCDSerial.write(206);
  LCDSerial.write(salinityUCLstring);
  LCDSerial.write(254);
  LCDSerial.write(213);
  if (SaltOpen) {
    LCDSerial.write("ON");
  }
  else {
    LCDSerial.write("OFF");
  }
  LCDSerial.write(254);
  LCDSerial.write(229);
  if (DIOpen) {
    LCDSerial.write("ON");
  }
  else {
    LCDSerial.write("OFF");
  }*/
}

 void clearScreen()
{
  //clears the screen, you will use this a lot!
  LCDSerial.write(0xFE);
  LCDSerial.write(0x01); 
}
