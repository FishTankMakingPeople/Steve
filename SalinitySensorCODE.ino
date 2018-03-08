#include <SoftwareSerial.h>

// Attach the serial display's RX line to digital pin 2
SoftwareSerial mySerial(3,2); // pin 2 = TX, pin 3 = RX (unused)
char salinitystring[10]; // create string array

int salinity_power_pin = 7; // Digital I/O pin, Global variable
void setup()
{
  mySerial.begin(9600); // set up serial port for 9600 baud
  delay(500); // wait for display to boot up
  clearScreen();
  mySerial.write(254); // cursor to beginning of first line
  mySerial.write(128);

  mySerial.write("Salinity:            "); // clear display + legends
  

Serial.begin (9600);
pinMode (salinity_power_pin, OUTPUT);
}
void loop()
{
int salinity_input_pin = A4; // Analog input pin
int salinity;
salinity = sensor_reading( salinity_power_pin, salinity_input_pin );
Serial.println (salinity);
sprintf(salinitystring,"%4d",salinity); // create strings from the numbers
mySerial.write(254); // cursor to 7th position on first line
mySerial.write(137);
mySerial.write(salinitystring);
}
// ---------------------------------------------------
int sensor_reading( int power_pin, int input_pin ) {
int reading;
digitalWrite (power_pin, HIGH ); // Turn on the sensor
delay(100); // Wait to settle
reading = analogRead (input_pin ); // Read voltage
digitalWrite (power_pin, LOW ); // Turn off the sensor
delay(100); // Wait to settle
return reading;
}

void clearScreen()
{
  //clears the screen, you will use this a lot!
  mySerial.write(0xFE);
  mySerial.write(0x01); 
}
