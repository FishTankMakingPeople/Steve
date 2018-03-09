int thermistor_power_pin = 6;
int thermistor_input_pin = A5;
int heater_power_pin = 8;

void setup()
{
  Serial.begin(9600);
  pinMode(thermistor_power_pin, OUTPUT);
  pinMode(heater_power_pin, OUTPUT);
}

void loop()
{
  int heater_status;
  int nave = 30, reading;
  long heater_start = 15000, heater_stop = 75000, now;
  float T;
  long start_time = millis();
  
  now = millis() - start_time;

  if ( now >= heater_start & now <= heater_stop ) 
  {
    digitalWrite( heater_power_pin, HIGH );
    heater_status = 1;
  } 
  
  else 
  {
    digitalWrite( heater_power_pin, LOW );
    heater_status = 0;
  }
  
  T = thermistor_reading_ave( thermistor_power_pin, thermistor_input_pin, nave);
  Serial.print(now);
  Serial.print("\t"); Serial.print(heater_status);
  Serial.print("\t"); Serial.println(T);
}

  float thermistor_reading_ave (int thermo_power, int thermo_input, int reading_ave)
  {
    float reading[reading_ave];
    float sum = 0.0;
    for (int i = 0; i < reading_ave; i++)
    {
      digitalWrite(thermo_power, HIGH);
      delay(100);
      reading[i] = analogRead(thermo_input);
      digitalWrite(thermo_power, LOW);
      delay(100);

      sum = sum + reading[i];      
    }
    float average = sum / reading_ave;
    float tempC = (((.000087188130*pow(average,2))-(.027187059*average)-12.705983)-32.0)*(5.0/9.0);
    return tempC;
  }
  
