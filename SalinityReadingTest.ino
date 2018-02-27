#include <SoftwareSerial.h>

// Attach the serial display's RX line to digital pin 2
SoftwareSerial LCDSerial(3,2); // pin 2 = TX, pin 3 = RX (unused)
char salinitystring[20];
char setpointstring[20];
char LCLstring[20];
char UCLstring[20];
int salinity_power_pin = 7;
int solDI_pin = 10;
int solSalt_pin = 9;
int salinity_input_pin = A4;
int setpointPotPin = A0;
int setpointReading;
float sigma = 5.798;
float setpoint;
float LCL;
float UCL;
float salinityReading;
float F = 0.15;
float G = 0.80;
float massInTank = 106.0;
float salt_flowrate = 6.80;
float DI_flowrate = 6.74;
float deadtime = 6000.0;
unsigned long last_salinity_update;
unsigned long cTime;
float DIOpenTime;
float saltOpenTime; 

void setup() {
  Serial.begin(9600);
  LCDSerial.begin(9600);
  delay(500); 
  pinMode(solDI_pin, OUTPUT);
  pinMode(solSalt_pin, OUTPUT);
  pinMode (salinity_power_pin, OUTPUT);
  last_salinity_update = millis();
}

void loop() {
  setDeadband();
  salinityReading = salinity_value(sensor_reading( salinity_power_pin, salinity_input_pin ));
  LCDUpdate();
  if (millis()-last_salinity_update > deadtime) {
    if ( salinityReading>UCL ) {
      DIOpenTime = (massInTank*((G*(setpoint-salinityReading))/((1-F)*(0-salinityReading))))/DI_flowrate;
      cTime = millis();
      digitalWrite(solDI_pin, HIGH); 
      for (unsigned long i = millis(); i < (cTime + (DIOpenTime*1000)); i = millis())
      { 
        LCDSerial.write(254);
        LCDSerial.write(229);
        LCDSerial.write("ON ");
        setDeadband();
        salinityReading = salinity_value(sensor_reading( salinity_power_pin, salinity_input_pin ));
        LCDUpdate();
      }
      digitalWrite(solDI_pin, LOW);    
      last_salinity_update = millis();
    }
    if ( salinityReading<LCL ) {
      saltOpenTime = (massInTank*((G*(setpoint-salinityReading))/((1-F)*(1-salinityReading))))/salt_flowrate;
      cTime = millis();
      digitalWrite(solSalt_pin, HIGH); 
      for (unsigned long i = millis(); i < (cTime + (saltOpenTime*1000)); i = millis())
      { 
        LCDSerial.write(254);
        LCDSerial.write(213);
        LCDSerial.write("ON ");
        setDeadband();
        salinityReading = salinity_value(sensor_reading( salinity_power_pin, salinity_input_pin ));
        LCDUpdate();
      }
      digitalWrite(solSalt_pin, LOW);    
      last_salinity_update = millis();
    }
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
 int rbreak = 570; // separation between linear segments
 int rlimit = 1023; // upper limit of acceptable readings
 float salinity; // Compute salinity from calibration equations
 if ( reading<0 ) {
 // print error message, reading can’t be negative
 } else if ( reading<rbreak ) {
 salinity = .0000822999*reading;
 } else if ( reading<=rlimit ) {
 salinity = (.00000405447*pow(reading,2))-(.00426449*reading)+1.16329;
 } else {
 // do something to be safe
 }
 //Serial.println(salinity);
 return(salinity);
}

void setDeadband() {
  setpointReading = analogRead(setpointPotPin);
  setpointReading = constrain(setpointReading, 5, 987);
  setpointReading = map(setpointReading, 5, 987, 570, 646);
  setpoint = salinity_value(setpointReading);
  LCL = salinity_value(setpointReading-(sigma*3));
  UCL = salinity_value(setpointReading+(sigma*3));
}

void LCDUpdate() {
  clearScreen();
  LCDSerial.write(254);
  LCDSerial.write(128);
  LCDSerial.write("  LCL  SetPt    UCL");
  LCDSerial.write(254);
  LCDSerial.write(148);
  LCDSerial.write("Salty  Salinity   DI"); 
  dtostrf(salinityReading,6,4,salinitystring);
  dtostrf(setpoint,6,4,setpointstring);
  dtostrf(LCL,6,4,LCLstring);
  dtostrf(UCL,6,4,UCLstring);      
  LCDSerial.write(254);
  LCDSerial.write(220);
  LCDSerial.write(salinitystring);
  LCDSerial.write(254);
  LCDSerial.write(199);
  LCDSerial.write(setpointstring);
  LCDSerial.write(254);
  LCDSerial.write(192);
  LCDSerial.write(LCLstring);
  LCDSerial.write(254);
  LCDSerial.write(206);
  LCDSerial.write(UCLstring);
  LCDSerial.write(254);
  LCDSerial.write(213);
  LCDSerial.write("OFF");
  LCDSerial.write(254);
  LCDSerial.write(229);
  LCDSerial.write("OFF");
}

 void clearScreen()
{
  //clears the screen, you will use this a lot!
  LCDSerial.write(0xFE);
  LCDSerial.write(0x01); 
}
